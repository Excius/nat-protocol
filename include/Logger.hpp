#pragma once

#include <string>

class Packet;
class NatEntry;

namespace net {
	namespace log {

		enum class Level {
			MAP,
			OUT,
			IN,
			DROP,
			EXPIRE,
			ERROR,
		};

		// Logs a packet with a formatted tag (e.g., [MAP], [OUT], [IN ], [DROP]).
		void packet(Level level, const Packet &packet);

		// Logs a NAT entry mapping with a formatted tag (e.g., [MAP]).
		void entry(Level level, const NatEntry &entry);

		// Logs a message with a tag.
		void message(Level level, const std::string &msg);

		// Returns the textual tag for the level.
		std::string tag(Level level);

	} // namespace log
} // namespace net
