#include "NatTable.hpp"

#include <iostream>
#include <ostream>

constexpr uint16_t PORT_START = 40001;
constexpr uint16_t PORT_END = 65535;

NatTable::NatTable(const std::string &publicIp, const std::chrono::seconds timeout) :
		publicIp(publicIp), nextAvailablePort(PORT_START), timeoutDuration{timeout} {}

NatEntry *NatTable::findByPrivate(const std::string &privateIp, const uint16_t privatePort) {
	const PrivateKey key{privateIp, privatePort};

	auto it = outboundTraffic.find(key);
	if (it == outboundTraffic.end()) {
		return nullptr;
	}
	it->second.updateTimestamp();
	return &(it->second);
}

NatEntry *NatTable::findByPublicPort(const uint16_t publicPort) {

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
		return natEntry;
	}

	if (outboundTraffic.size() >= (PORT_END - PORT_START + 1)) {
		// Table size overflow
		return nullptr;
	}

	if (nextAvailablePort > PORT_END) {
		// port pool exhausted
		nextAvailablePort = PORT_START;
	}

	uint16_t allocatedPort = nextAvailablePort;

	while (inboundTraffic.find(allocatedPort) != inboundTraffic.end()) {
		allocatedPort++;

		if (allocatedPort > PORT_END) {
			allocatedPort = PORT_START;
		}

		if (allocatedPort == nextAvailablePort) {
			// Cycle complete -> no port available
			return nullptr;
		}
	}

	auto result = outboundTraffic.emplace(key, NatEntry{publicIp, allocatedPort, privateIp, privatePort});

	if (!result.second) {
		return &(result.first->second);
	}

	inboundTraffic.emplace(allocatedPort, key);
	nextAvailablePort = allocatedPort + 1;

	return &(result.first->second);
}

void NatTable::removeExpired() {
	const std::chrono::seconds TIMEOUT{timeoutDuration};
	const auto now = std::chrono::steady_clock::now();

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
