#include "IpUtils.hpp"

#include <array>
#include <cctype>
#include <sstream>

namespace net {

static bool parseIpv4(const std::string &ip, std::array<int, 4> &outOctets) {
    std::istringstream ss(ip);
    std::string token;
    int idx = 0;

    while (std::getline(ss, token, '.')) {
        if (idx >= 4) {
            return false;
        }

        if (token.empty() || token.size() > 3) {
            return false;
        }

        // No leading zeros unless the number is zero.
        if (token.size() > 1 && token[0] == '0') {
            return false;
        }

        int value = 0;
        for (char c : token) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
            value = value * 10 + (c - '0');
        }

        if (value < 0 || value > 255) {
            return false;
        }

        outOctets[idx++] = value;
    }

    return idx == 4;
}

bool isValidIPv4(const std::string &ip) {
    std::array<int, 4> octets{};
    return parseIpv4(ip, octets);
}

bool isPrivateIPv4(const std::string &ip) {
    std::array<int, 4> octets{};
    if (!parseIpv4(ip, octets)) {
        return false;
    }

    const int a = octets[0];
    const int b = octets[1];

    // RFC 1918 private ranges
    // 10.0.0.0/8
    if (a == 10) {
        return true;
    }

    // 172.16.0.0/12
    if (a == 172 && b >= 16 && b <= 31) {
        return true;
    }

    // 192.168.0.0/16
    if (a == 192 && b == 168) {
        return true;
    }

    return false;
}

} // namespace net
