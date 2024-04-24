/**
 * @file
 * @brief Implementation of FUSE callback functions for filesystem operations
 * @author Pavel Siska <siska@cesnet.cz>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>

#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/mount.h>
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
	constexpr size_t requiredCapacity = blockSize * requiredBlockEmptyCapacityMultiplier;

	const size_t contentSize = fileContentToString(file).size();
	const size_t blockSizeMultiplier = (contentSize + requiredCapacity) / blockSize + 1;

	return static_cast<off_t>(blockSizeMultiplier * blockSize);
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

std::shared_ptr<Directory> getRootDirectory()
{
	return *reinterpret_cast<std::shared_ptr<Directory>*>(fuse_get_context()->private_data);
}

static int fuseGetAttr(const char* path, struct stat* stbuf, struct fuse_file_info* fileInfo)
{
	(void) fileInfo;

	std::memset(stbuf, 0, sizeof(struct stat));

	const std::shared_ptr<Directory> rootDirectory = getRootDirectory();
	auto node = utils::getNodeFromPath(rootDirectory, path);

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

	// NOLINTBEGIN (performance-no-int-to-ptr, integer to pointer cast)
	std::string& cacheBuffer = *reinterpret_cast<std::string*>(fileInfo->fh);
	// NOLINTEND

	if (cacheBuffer.empty()) {
		cacheBuffer = fileContentToString(file);
	}

	if (static_cast<size_t>(offset) >= cacheBuffer.size()) {
		return 0;
	}

	const size_t length = std::min(size, cacheBuffer.size() - offset);
	std::memcpy(buffer, cacheBuffer.c_str() + offset, length);

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

static void setFuseOperations(struct fuse_operations* fuseOps)
{
	fuseOps->getattr = getAttrCallback;
	fuseOps->readdir = readDirCallback;
	fuseOps->open = openCallback;
	fuseOps->read = readCallback;
	fuseOps->write = writeCallback;
	fuseOps->release = releaseCallback;
}

static void runFuseLoop(struct fuse* fuse)
{
	try {
		const int ret = fuse_loop(fuse);
		if (ret < 0) {
			throw std::runtime_error("fuse_loop() is not running...");
		}
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
	bool tryToUnmountOnStart)
{
	m_rootDirectory = std::move(rootDirectory);
	if (m_rootDirectory == nullptr) {
		throw std::runtime_error("Root directory is not set.");
	}

	FuseArgs fuseArgs;
	fillFuseArgs(fuseArgs.get());

	struct fuse_operations fuseOps = {};
	setFuseOperations(&fuseOps);

	m_fuse.reset(fuse_new(fuseArgs.get(), &fuseOps, sizeof(fuseOps), (void*) &m_rootDirectory));
	if (m_fuse == nullptr) {
		throw std::runtime_error("fuse_new() has failed.");
	}

	int ret = fuse_mount(m_fuse.get(), mountPoint.c_str());
	if (ret < 0 && tryToUnmountOnStart) {
		ret = umount2(mountPoint.c_str(), MNT_FORCE | UMOUNT_NOFOLLOW);
		if (ret < 0) {
			throw std::runtime_error("umount of " + mountPoint + " has failed.");
		}

		ret = fuse_mount(m_fuse.get(), mountPoint.c_str());
		if (ret < 0) {
			throw std::runtime_error("fuse_mount() has failed again.");
		}
	}

	if (ret < 0) {
		throw std::runtime_error("fuse_mount() has failed.");
	}
}

void AppFsFuse::start()
{
	if (m_isStarted) {
		throw std::runtime_error("AppFsFuse::start() has already been called");
	}

	m_fuseThread = std::thread([&]() { runFuseLoop(m_fuse.get()); });
	m_isStarted = true;
}

void AppFsFuse::stop()
{
	unmount();
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
