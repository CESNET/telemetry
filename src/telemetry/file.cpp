/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Telemetry file
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/file.hpp>

#include <mutex>
#include <stdexcept>
#include <string>

namespace telemetry {

File::File(const std::shared_ptr<Node>& parent, std::string_view name, FileOps ops)
	: Node(parent, name)
	, m_ops(std::move(ops))
{
	/*
	 * Note: The file CANNOT be added to the parent as an entry here, since
	 * this object hasn't been fully initialized yet and shared_from_this()
	 * doesn't work in constructors.
	 */
}

bool File::hasRead()
{
	const std::lock_guard lock(getMutex());
	return bool {m_ops.read};
}

bool File::hasClear()
{
	const std::lock_guard lock(getMutex());
	return bool {m_ops.clear};
}

Content File::read()
{
	const std::lock_guard lock(getMutex());

	if (!m_ops.read) {
		const std::string err = "File::read('" + getFullPath() + "') operation not supported";
		throw TelemetryException(err);
	}

	return m_ops.read();
}

void File::clear()
{
	const std::lock_guard lock(getMutex());

	if (!m_ops.clear) {
		const std::string err = "File::clear('" + getFullPath() + "') operation not supported";
		throw TelemetryException(err);
	}

	m_ops.clear();
}

void File::disable()
{
	const std::lock_guard lock(getMutex());
	m_ops = {};
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testFile.cpp"
#endif
