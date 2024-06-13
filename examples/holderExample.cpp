/**
 * @file
 * @author Daniel Pelanek <xpeland00@vutbr.cz>
 * @brief Shows how to use the holder class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>
#include <telemetry.hpp>

// Object that collects it's own telemetry or owns
// telemetry files or directories.
class IOwnTelemetryFiles {
public:
	void configTelemetry(const std::shared_ptr<telemetry::Directory>& rootDir)
	{
		// Create a telemetry file
		const telemetry::FileOps fileOps = {nullptr, nullptr};

		auto someFile = rootDir->addFile("someFile", fileOps);

		// We also need to add the file to the holder.
		m_telemetryHolder.add(someFile);
	}

	// If you want to disable callback of files before the
	// is destroyed.
	void disableFiles() { m_telemetryHolder.disableFiles(); }

private:
	telemetry::Holder m_telemetryHolder;
};

int main()
{
	// Same as basic example
	std::shared_ptr<telemetry::Directory> telemetryRootNode;
	telemetryRootNode = telemetry::Directory::create();

	std::string fusePath = "fusedir";

	telemetry::appFs::AppFsFuse fuse
		= telemetry::appFs::AppFsFuse(telemetryRootNode, fusePath, true, true);
	fuse.start();

	// The files callbacks get disabled when the telemetry holder
	// is destroyed and if the files and dirs don't have another reference
	// elsewhere they get destroyed too.
	IOwnTelemetryFiles object;

	// Configuration of telemetry inside of the object.
	object.configTelemetry(telemetryRootNode);

	// Disable callbacks before the objects destructor
	object.disableFiles();
}
