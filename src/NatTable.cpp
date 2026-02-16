#include "NatTable.hpp"

#include <iostream>
#include <ostream>

NatTable::NatTable(const std::string &publicIp) : publicIp(publicIp), nextAvailablePort(40001) {}

NatEntry *NatTable::findByPrivate(const std::string &privateIp, uint16_t privatePort) {
	PrivateKey key{privateIp, privatePort};

	auto it = outboundTraffic.find(key);
	if (it == outboundTraffic.end()) {
		return nullptr;
	}
	it->second.updateTimestamp();
	return &(it->second);
}

NatEntry *NatTable::findByPublicPort(uint16_t publicPort) {

	auto itInbound = inboundTraffic.find(publicPort);
	if (itInbound == inboundTraffic.end())
		return nullptr;

	auto itOutbound = outboundTraffic.find(itInbound->second);
	if (itOutbound == outboundTraffic.end())
		return nullptr;

	itOutbound->second.updateTimestamp();
	return &(itOutbound->second);
}

NatEntry *NatTable::createMapping(const std::string &privateIp, uint16_t privatePort) {
	PrivateKey key{privateIp, privatePort};

	NatEntry *natEntry = findByPrivate(privateIp, privatePort);
	if (natEntry != nullptr) {
		natEntry->updateTimestamp();
		return natEntry;
	}

	uint16_t allocatedPort = nextAvailablePort;
	if (allocatedPort == 65535) {
		return nullptr;
	}

	NatEntry entry(publicIp, allocatedPort, privateIp, privatePort);
	auto result = outboundTraffic.emplace(key, entry);

	inboundTraffic.emplace(allocatedPort, key);
	nextAvailablePort++;

	return &(result.first->second);
}

void NatTable::removeExpired() {
	constexpr std::chrono::seconds TIMEOUT(60);
	auto now = std::chrono::steady_clock::now();

	for (auto it = outboundTraffic.begin(); it != outboundTraffic.end();) {
		if (now - it->second.getLastUsed() > TIMEOUT) {
			uint16_t publicPort = it->second.getPublicPort();

			std::cout << "[EXPIRE] ";
			it->second.print();
			std::cout << std::endl;

			inboundTraffic.erase(publicPort);
			it = outboundTraffic.erase(it);
		} else {
			++it;
		}
	}
}

void NatTable::printTable() const {
	std::cout << "-------------- NAT TABLE --------------" << std::endl;

	if (outboundTraffic.empty()) {
		std::cout << "(empty)" << std::endl;
	}

	for (const auto &it: outboundTraffic) {
		it.second.print();
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------" << std::endl;
	std::cout << std::endl;
}
