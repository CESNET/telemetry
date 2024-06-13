/**
 * @file
 * @author Daniel Pelanek <xpeland00@vutbr.cz>
 * @brief Shows the basic usage of telemetry with fuse
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>
#include <memory>
#include <telemetry.hpp>

// example help
#include <atomic>
#include <chrono>
#include <csignal>
std::atomic<bool> g_gotSIGINT(false);
void signalHandler(int signum)
{
	(void) signum;
	g_gotSIGINT.store(true);
}

// The return value has to be telemetry::Content or one of its variants.
telemetry::Content
getTimeElapsed(const std::chrono::time_point<std::chrono::system_clock>& startTime)
{
	auto now = std::chrono::system_clock::now();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

	// telemetry::Content is std::variant<Scalar, ScalarWithUnit, Array, Dict>.
	// How they are used is shown in another example.
	return telemetry::Scalar(timeElapsed);
}

// Resets start time to the current time.
void clearTime(std::chrono::time_point<std::chrono::system_clock>& startTime)
{
	startTime = std::chrono::system_clock::now();
}

int main()
{
	// Creating root dir for filesystem.
	std::shared_ptr<telemetry::Directory> telemetryRootNode;
	telemetryRootNode = telemetry::Directory::create();

	// The path to root dir is local to where the program is called from.
	std::string fusePath = "fusedir";

	// Linking root dir to the chosen directory on disk.
	// If the fourth argument is true it means the directory on disk doesn't need to
	// exist before starting program.
	telemetry::appFs::AppFsFuse fuse
		= telemetry::appFs::AppFsFuse(telemetryRootNode, fusePath, true, true);
	fuse.start();
	// The filesystem is still just empty.
	// /

	// So let's a directory named input into the root dir.
	std::shared_ptr<telemetry::Directory> inputDir = telemetryRootNode->addDir("input");
	// Now the filesystem looks like this.
	// /
	// └─ input/

	// Every file can have two lambdas attached to it.
	//
	// One for reading -> What gets called when something tries to read the file on disk.
	// Here we write the return value of getTime to the file.
	//
	// One for clearing -> What gets called when you want to reset telemetry data.
	// In this case we reset the startTime.
	auto startTime = std::chrono::system_clock::now();
	const telemetry::FileOps fileOps
		= {[&startTime]() { return getTimeElapsed(startTime); },
		   [&startTime]() { return clearTime(startTime); }};

	// The read and clear functions are optional. In the case you don't use them pass
	// a null pointer instead.
	const telemetry::FileOps anotherFileOps = {nullptr, nullptr};

	// Files be put into the root directory.
	const std::shared_ptr<telemetry::File> timeFile = telemetryRootNode->addFile("time", fileOps);
	// Now it looks like this.
	// /
	// ├─ input/
	// └─ time

	// Or into another directory.
	const std::shared_ptr<telemetry::File> anotherTimeFile
		= inputDir->addFile("time", anotherFileOps);
	// Now it looks like this.
	// /
	// ├─ input/
	// │  └─ time
	// └─ time

	// Waiting for ctrl+c. In the meantime you can open another terminal and
	// navigate to the newly linked directory. Just reading the new time file
	// should print the time elapsed in seconds since the start of the program.
	std::signal(SIGINT, signalHandler);
	while (!g_gotSIGINT.load()) {};
}
