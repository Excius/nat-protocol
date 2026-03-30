#pragma once

#include <string>

namespace net {

	// Returns true if the given string is a valid IPv4 address (dotted quad).
	bool isValidIPv4(const std::string &ip);

	// Returns true if the given string is a valid IPv4 address in a private address range
	// (RFC 1918: 10/8, 172.16/12, 192.168/16).
	bool isPrivateIPv4(const std::string &ip);
} // namespace net
