#include "../include/IpUtils.hpp"

#include <cassert>
#include <iostream>

int main() {
    // Valid private IP ranges
    assert(net::isPrivateIPv4("10.0.0.1"));
    assert(net::isPrivateIPv4("10.255.255.255"));
    assert(net::isPrivateIPv4("172.16.0.0"));
    assert(net::isPrivateIPv4("172.31.255.255"));
    assert(net::isPrivateIPv4("192.168.0.0"));
    assert(net::isPrivateIPv4("192.168.255.255"));

    // Not private
    assert(!net::isPrivateIPv4("8.8.8.8"));
    assert(!net::isPrivateIPv4("172.15.0.1"));
    assert(!net::isPrivateIPv4("172.32.0.1"));
    assert(!net::isPrivateIPv4("192.167.0.1"));

    // Invalid address
    assert(!net::isPrivateIPv4("256.0.0.1"));
    assert(!net::isPrivateIPv4("abc.def.ghi.jkl"));
    assert(!net::isPrivateIPv4("10.0.0"));
    assert(!net::isPrivateIPv4(""));

    std::cout << "All IpUtils tests passed\n";
    return 0;
}
