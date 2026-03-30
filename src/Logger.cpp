#include "Logger.hpp"
#include "NatEntry.hpp"
#include "Packet.hpp"

#include <iostream>

namespace net {
	namespace log {

		std::string tag(Level level) {
			switch (level) {
				case Level::MAP:
					return "[MAP]";
				case Level::OUT:
					return "[OUT]";
				case Level::IN:
					return "[IN ]"; // padded to match requested format
				case Level::DROP:
					return "[DROP]";
				case Level::EXPIRE:
					return "[EXPIRE]";
				case Level::ERROR:
					return "[ERROR]";
			}
			return "[UNKNOWN]";
		}

		void packet(Level level, const Packet &packet) {
			std::cout << tag(level) << " ";
			packet.print();
		}

		void entry(Level level, const NatEntry &entry) {
			std::cout << tag(level) << " ";
			entry.print();
			std::cout << std::endl;
		}

		void message(Level level, const std::string &msg) { std::cout << tag(level) << " " << msg << std::endl; }

	} // namespace log
} // namespace net
