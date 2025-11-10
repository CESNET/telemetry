/**
 * @file
 * @brief Implementation of FUSE callback functions for filesystem operations
 * @author Pavel Siska <siska@cesnet.cz>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fuse3/fuse_lowlevel.h>
#include <iostream>
#include <poll.h>
#include <telemetry.hpp>
#include <unistd.h>

namespace telemetry::appFs {

static std::string fileContentToString(const std::shared_ptr<File>& file)
{
	const Content content = file->read();
	return contentToString(content) + "\n";
}

static off_t getMaxFileSize(const std::shared_ptr<File>& file)
{
	const size_t blockSize = BUFSIZ;

	if (!file->hasRead()) {
		return blockSize;
	}

	constexpr double requiredBlockEmptyCapacityMultiplier = 0.5;
	constexpr auto requiredCapacity = static_cast<size_t>(
		static_cast<double>(blockSize) * requiredBlockEmptyCapacityMultiplier);

	const size_t contentSize = fileContentToString(file).size();
	const size_t blockSizeMultiplier = ((contentSize + requiredCapacity) / blockSize) + 1;

	return static_cast<off_t>(blockSizeMultiplier * blockSize);
}

static void setSymlinkAttr(struct stat* stbuf)
{
	const mode_t symlinkMode = 0777;
	stbuf->st_mode = S_IFLNK | symlinkMode;
	stbuf->st_nlink = 1;
	stbuf->st_size = BUFSIZ;
	stbuf->st_mtime = time(nullptr);
}

static void setFileAttr(const std::shared_ptr<File>& file, struct stat* stbuf)
{
	stbuf->st_mode = S_IFREG;

	if (file->hasRead()) {
		const mode_t readMode = 0444;
		stbuf->st_mode |= readMode;
	}

	if (file->hasClear()) {
		const mode_t writeMode = 0222;
		stbuf->st_mode |= writeMode;
	}

	stbuf->st_nlink = 1;
	stbuf->st_size = getMaxFileSize(file);
	stbuf->st_mtime = time(nullptr);
}

static void setDirectoryAttr(struct stat* stbuf)
{
	const mode_t readExecuteMode = 0555;
	stbuf->st_mode = S_IFDIR | readExecuteMode;
	stbuf->st_nlink = 2;
	stbuf->st_mtime = time(nullptr);
}

static std::shared_ptr<Directory> getRootDirectory()
{
	return *reinterpret_cast<std::shared_ptr<Directory>*>(fuse_get_context()->private_data);
}

static int fuseGetAttr(const char* path, struct stat* stbuf, struct fuse_file_info* fileInfo)
{
	(void) fileInfo;

	std::memset(stbuf, 0, sizeof(struct stat));

	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

	if (utils::isSymlink(node)) {
		setSymlinkAttr(stbuf);
		return 0;
	}

	if (utils::isFile(node)) {
		setFileAttr(std::dynamic_pointer_cast<File>(node), stbuf);
		return 0;
	}

	if (utils::isDirectory(node)) {
		setDirectoryAttr(stbuf);
		return 0;
	}

	return -ENOENT;
}

static int getAttrCallback(const char* path, struct stat* stbuf, struct fuse_file_info* fileInfo)
{
	try {
		return fuseGetAttr(path, stbuf, fileInfo);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int fuseOpen(const char* path, struct fuse_file_info* fileInfo)
{
	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

	if (!utils::isFile(node)) {
		return -ENOENT;
	}

	fileInfo->fh = reinterpret_cast<uint64_t>(new std::string());

	return 0;
}

static int openCallback(const char* path, struct fuse_file_info* fileInfo)
{
	try {
		return fuseOpen(path, fileInfo);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int fuseRelease(const char* path, struct fuse_file_info* fileInfo)
{
	(void) path;

	if (fileInfo->fh != reinterpret_cast<uint64_t>(nullptr)) {
		// NOLINTBEGIN (performance-no-int-to-ptr, integer to pointer cast)
		delete reinterpret_cast<std::string*>(fileInfo->fh);
		// NOLINTEND
	}

	return 0;
}

static int releaseCallback(const char* path, struct fuse_file_info* fileInfo)
{
	try {
		return fuseRelease(path, fileInfo);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int fuseReadDir(
	const char* path,
	void* buffer,
	fuse_fill_dir_t filler,
	off_t offset,
	struct fuse_file_info* fileInfo,
	enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fileInfo;
	(void) flags;

	const fuse_fill_dir_flags dirFlag = {};

	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

	if (!utils::isDirectory(node)) {
		return -ENOENT;
	}

	filler(buffer, ".", nullptr, 0, dirFlag);
	filler(buffer, "..", nullptr, 0, dirFlag);

	auto directory = std::dynamic_pointer_cast<Directory>(node);
	for (const auto& entry : directory->listEntries()) {
		filler(buffer, entry.c_str(), nullptr, 0, dirFlag);
	}

	return 0;
}

static int readDirCallback(
	const char* path,
	void* buffer,
	fuse_fill_dir_t filler,
	off_t offset,
	struct fuse_file_info* fileInfo,
	enum fuse_readdir_flags flags)
{
	try {
		return fuseReadDir(path, buffer, filler, offset, fileInfo, flags);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int readFile(
	const std::shared_ptr<File>& file,
	char* buffer,
	size_t size,
	off_t offset,
	struct fuse_file_info* fileInfo)
{
	if (!file->hasRead()) {
		return -ENOTSUP;
	}

	// NOLINTNEXTLINE (performance-no-int-to-ptr, integer to pointer cast)
	std::string& cacheBuffer = *reinterpret_cast<std::string*>(fileInfo->fh);

	if (cacheBuffer.empty()) {
		cacheBuffer = fileContentToString(file);
	}

	const auto uOffset = static_cast<size_t>(offset);

	if (uOffset >= cacheBuffer.size()) {
		return 0;
	}

	const size_t length = std::min(size, cacheBuffer.size() - uOffset);
	std::memcpy(buffer, cacheBuffer.c_str() + uOffset, length);

	return static_cast<int>(length);
}

static int
fuseRead(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* fileInfo)
{
	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

	if (!utils::isFile(node)) {
		return -ENOENT;
	}

	return readFile(std::dynamic_pointer_cast<File>(node), buffer, size, offset, fileInfo);
}

static int readCallback(
	const char* path,
	char* buffer,
	size_t size,
	off_t offset,
	struct fuse_file_info* fileInfo)
{
	try {
		return fuseRead(path, buffer, size, offset, fileInfo);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int fuseWrite(
	// NOLINTBEGIN
	const char* path,
	const char* buffer,
	size_t size,
	off_t offset,
	// NOLINTEND
	struct fuse_file_info* fileInfo)
{
	(void) buffer;
	(void) offset;
	(void) fileInfo;

	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

	if (!utils::isFile(node)) {
		return -ENOENT;
	}

	auto file = std::dynamic_pointer_cast<File>(node);

	if (!file->hasClear()) {
		return -ENOTSUP;
	}

	file->clear();

	return static_cast<int>(size);
}

static int writeCallback(
	// NOLINTBEGIN
	const char* path,
	const char* buffer,
	size_t size,
	off_t offset,
	// NOLINTEND
	struct fuse_file_info* fileInfo)
{
	try {
		return fuseWrite(path, buffer, size, offset, fileInfo);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -EINVAL;
	}
}

static int readlinkCallback(const char* path, char* buffer, size_t size)
{
	auto node = utils::getNodeFromPath(getRootDirectory(), path);

	if (!utils::isSymlink(node)) {
		return -ENOENT;
	}

	const auto targetNode = std::dynamic_pointer_cast<Symlink>(node)->getTarget();
	if (targetNode == nullptr) {
		return -ENOENT;
	}

	const std::filesystem::path targetPath = targetNode->getFullPath();

	const std::string relativeTargetPath
		= std::filesystem::relative(targetPath, std::filesystem::path(path).parent_path()).string();

	if (size < relativeTargetPath.size() + 1) {
		return -ENAMETOOLONG;
	}

	std::memcpy(buffer, relativeTargetPath.c_str(), relativeTargetPath.size());
	buffer[relativeTargetPath.size()] = '\0';

	return 0;
}

static void setFuseOperations(struct fuse_operations* fuseOps)
{
	fuseOps->getattr = getAttrCallback;
	fuseOps->readdir = readDirCallback;
	fuseOps->open = openCallback;
	fuseOps->read = readCallback;
	fuseOps->write = writeCallback;
	fuseOps->release = releaseCallback;
	fuseOps->readlink = readlinkCallback;
}

class AppFsFuseBuffer {
public:
	AppFsFuseBuffer() = default;

	~AppFsFuseBuffer()
	{
		// NOLINTNEXTLINE (cppcoreguidelines-no-malloc)
		free(m_buffer.mem);
	}

	fuse_buf* getBuffer() { return &m_buffer; }

private:
	fuse_buf m_buffer {};
};

static void fuseLoop(struct fuse_session* session)
{
	AppFsFuseBuffer buffer;

	do {
		const int ret = fuse_session_receive_buf(session, buffer.getBuffer());
		if (ret == -EINTR) {
			continue;
		}
		if (ret == -EAGAIN) {
			return;
		}
		if (ret < 0) {
			throw std::runtime_error(
				"fuse_session_receive_buf() has failed: " + std::to_string(ret));
		}
		fuse_session_process_buf(session, buffer.getBuffer());
		return;
	} while (true);
}

static void setupFuseSessionFd(struct fuse* fuse)
{
	struct fuse_session* session = fuse_get_session(fuse);
	const int sessionFd = fuse_session_fd(session);

	int ret = fcntl(sessionFd, F_GETFL, 0);
	if (ret < 0) {
		throw std::runtime_error(
			"failed to F_GETFL on fuse file-descriptor: " + std::to_string(ret));
	}

	ret = fcntl(sessionFd, F_SETFL, ret | O_NONBLOCK);
	if (ret < 0) {
		throw std::runtime_error(
			"failed to F_SETFL on fuse file-descriptor: " + std::to_string(ret));
	}
}

static void pollableFuseLoop(struct fuse* fuse)
{
	struct fuse_session* session = fuse_get_session(fuse);

	struct pollfd pfd;
	pfd.fd = fuse_session_fd(session);
	pfd.events = POLLIN;

	while (fuse_session_exited(session) == 0) {
		const int pollTimeout = 500;
		const int pollResult = poll(&pfd, 1, pollTimeout);
		if (pollResult == -1) {
			// NOLINTNEXTLINE (concurrency-mt-unsafe, function is not thread safe)
			throw std::runtime_error("poll failed: " + std::string(strerror(pollResult)));
		}

		if (pollResult == 0) {
			continue;
		}

		if ((pfd.revents & POLLIN) != 0) {
			fuseLoop(session);
		}
	}
}

static void tryCatchPollableFuseLoop(struct fuse* fuse)
{
	try {
		pollableFuseLoop(fuse);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

static void fillFuseArgs(struct fuse_args* fuseArgs)
{
	const std::string fuseUID = "uid=" + std::to_string(getuid());
	const std::string fuseGID = "gid=" + std::to_string(getgid());

	fuse_opt_add_arg(fuseArgs, "appfs");
	fuse_opt_add_arg(fuseArgs, "-o");
	fuse_opt_add_arg(fuseArgs, fuseUID.c_str());
	fuse_opt_add_arg(fuseArgs, "-o");
	fuse_opt_add_arg(fuseArgs, fuseGID.c_str());
	fuse_opt_add_arg(fuseArgs, "-o");
	fuse_opt_add_arg(fuseArgs, "allow_other");
	fuse_opt_add_arg(fuseArgs, "-o");
	fuse_opt_add_arg(fuseArgs, "attr_timeout=0");
}

static void createDirectories(const std::string& path)
{
	if (std::filesystem::exists(path)) {
		return;
	}

	std::error_code errorCode;
	if (!std::filesystem::create_directories(path, errorCode)) {
		throw std::runtime_error(
			"Failed to create directory (" + path + "). Error: " + errorCode.message());
	}
}

static void fuserUnmount(const std::string& mountPoint)
{
	const std::string whichCommand = "which fusermount3 > /dev/null 2>&1";

	// NOLINTNEXTLINE (concurrency-mt-unsafe, std::system function is not thread safe)
	const int ret = std::system(whichCommand.c_str());
	if (ret != 0) {
		std::cerr << "fusermount3 is not found. Unable to unmount '" + mountPoint << "'\n";
		return;
	}

	const std::string fusermountCommand = "fusermount3 -u " + mountPoint + " > /dev/null 2>&1";

	// NOLINTNEXTLINE (concurrency-mt-unsafe, std::system function is not thread safe)
	(void) !std::system(fusermountCommand.c_str());
}

class FuseArgs {
public:
	FuseArgs()
		: m_fuseArgs(FUSE_ARGS_INIT(0, nullptr))
	{
	}

	struct fuse_args* get() { return &m_fuseArgs; }

	~FuseArgs() { fuse_opt_free_args(&m_fuseArgs); }

private:
	struct fuse_args m_fuseArgs;
};

AppFsFuse::AppFsFuse(
	std::shared_ptr<Directory> rootDirectory,
	const std::string& mountPoint,
	bool tryToUnmountOnStart,
	bool createMountPoint)
{
	m_rootDirectory = std::move(rootDirectory);
	if (m_rootDirectory == nullptr) {
		throw std::runtime_error("Root directory is not set.");
	}

	FuseArgs fuseArgs;
	fillFuseArgs(fuseArgs.get());

	struct fuse_operations fuseOps = {};
	setFuseOperations(&fuseOps);

	/**
	 * If tryToUnmountOnStart is true, this code attempts to unmount the specified mount point using
	 * fusermount3 binary. This is necessary because if the application terminates unexpectedly, the
	 * filesystem might remain mounted, which can prevent proper status checking and cause
	 * subsequent attempts to mount it to fail. By forcing an unmount here, we ensure the mount
	 * point is in a clean state before proceeding.
	 */
	if (tryToUnmountOnStart) {
		fuserUnmount(mountPoint);
	}

	if (createMountPoint) {
		createDirectories(mountPoint);
	}

	m_fuse.reset(fuse_new(fuseArgs.get(), &fuseOps, sizeof(fuseOps), (void*) &m_rootDirectory));
	if (m_fuse == nullptr) {
		throw std::runtime_error("fuse_new() has failed.");
	}

	const int ret = fuse_mount(m_fuse.get(), mountPoint.c_str());
	if (ret < 0) {
		throw std::runtime_error("fuse_mount() has failed.");
	}

	setupFuseSessionFd(m_fuse.get());
}

void AppFsFuse::start()
{
	if (m_isStarted) {
		throw std::runtime_error("AppFsFuse::start() has already been called");
	}

	m_fuseThread = std::thread([&]() { tryCatchPollableFuseLoop(m_fuse.get()); });
	m_isStarted = true;
}

void AppFsFuse::stop()
{
	unmount();
	fuse_exit(m_fuse.get());

	if (m_fuseThread.joinable()) {
		m_fuseThread.join();
	}
}

void AppFsFuse::unmount()
{
	if (m_fuse != nullptr) {
		fuse_unmount(m_fuse.get());
	}
}

AppFsFuse::~AppFsFuse()
{
	stop();
}

} // namespace telemetry::appFs
