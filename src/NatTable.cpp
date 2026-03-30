#include "NatTable.hpp"
#include "Logger.hpp"

#include <iostream>
#include <ostream>

constexpr uint16_t PORT_START = 40001;
constexpr uint16_t PORT_END = 65535;

NatTable::NatTable(const std::string &publicIp, const std::chrono::seconds timeout, const bool enableBackgroundCleanup,
                   const std::chrono::seconds cleanupInterval) :
		publicIp(publicIp), nextAvailablePort(PORT_START), timeoutDuration{timeout}, cleanupInterval(cleanupInterval) {
	if (enableBackgroundCleanup) {
		startBackgroundCleanup(cleanupInterval);
	}
}

NatTable::~NatTable() { stopBackgroundCleanup(); }

void NatTable::startBackgroundCleanup(std::chrono::seconds interval) {
	std::lock_guard<std::mutex> lock(mtx);
	if (cleanupThread.joinable()) {
		return;
	}

	cleanupInterval = interval;
	stopCleanup.store(false, std::memory_order_release);
	cleanupThread = std::thread(&NatTable::cleanupLoop, this);
}

void NatTable::stopBackgroundCleanup() {
	stopCleanup.store(true, std::memory_order_release);
	if (cleanupThread.joinable()) {
		cleanupThread.join();
	}
}

void NatTable::cleanupLoop() {
	while (!stopCleanup.load(std::memory_order_acquire)) {
		std::this_thread::sleep_for(cleanupInterval);
		removeExpired();
	}
}

NatEntry *NatTable::findByPrivate(const std::string &privateIp, const uint16_t privatePort) {
	std::lock_guard<std::mutex> lock(mtx);
	const PrivateKey key{privateIp, privatePort};

	auto it = outboundTraffic.find(key);
	if (it == outboundTraffic.end()) {
		return nullptr;
	}
	it->second.updateTimestamp();
	return &(it->second);
}

NatEntry *NatTable::findByPublicPort(const uint16_t publicPort) {
	std::lock_guard<std::mutex> lock(mtx);

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
	std::lock_guard<std::mutex> lock(mtx);
	PrivateKey key{privateIp, privatePort};

	NatEntry *natEntry = findByPrivate(privateIp, privatePort);
	if (natEntry != nullptr) {
		return natEntry;
	}

	if (outboundTraffic.size() >= (PORT_END - PORT_START + 1)) {
		// Table size overflow
		return nullptr;
	}

	uint16_t allocatedPort;
	if (!freePortPool.empty()) {
		allocatedPort = freePortPool.front();
		freePortPool.pop();

	} else {

		if (nextAvailablePort > PORT_END) {
			// port pool exhausted
			nextAvailablePort = PORT_START;
		}

		allocatedPort = nextAvailablePort;

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

		// Handling the next available port
		nextAvailablePort = allocatedPort + 1;
		if (nextAvailablePort > PORT_END) {
			nextAvailablePort = PORT_START;
		}
	}

	auto result = outboundTraffic.emplace(key, NatEntry{publicIp, allocatedPort, privateIp, privatePort});

	if (!result.second) {
		return &(result.first->second);
	}

	inboundTraffic.emplace(allocatedPort, key);

	return &(result.first->second);
}

void NatTable::removeExpired() {
	std::lock_guard<std::mutex> lock(mtx);

	const std::chrono::seconds TIMEOUT{timeoutDuration};
	const auto now = std::chrono::steady_clock::now();

	for (auto it = outboundTraffic.begin(); it != outboundTraffic.end();) {
		if (now - it->second.getLastUsed() > TIMEOUT) {
			uint16_t publicPort = it->second.getPublicPort();

			net::log::entry(net::log::Level::EXPIRE, it->second);

			inboundTraffic.erase(publicPort);
			it = outboundTraffic.erase(it);

			freePortPool.push(publicPort);
		} else {
			++it;
		}
	}
}

void NatTable::printTable() const {
	std::lock_guard<std::mutex> lock(mtx);

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
