/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Holder of telemetry nodes.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/file.hpp>
#include <telemetry/holder.hpp>

namespace telemetry {

Holder::~Holder()
{
	disableFiles();
}

void Holder::add(const std::shared_ptr<Node>& node)
{
	m_entries.emplace_back(node);
}

void Holder::disableFiles()
{
	for (auto& item : m_entries) {
		File* ref = dynamic_cast<File*>(item.get());
		if (ref == nullptr) {
			continue;
		}

		ref->disable();
	}
}

} // namespace telemetry
