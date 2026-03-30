#pragma once
#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "NatEntry.hpp"

struct PrivateKey {
	std::string ip;
	uint16_t port;

	bool operator<(const PrivateKey &other) const {
		if (ip < other.ip)
			return true;
		if (ip > other.ip)
			return false;
		return port < other.port;
	}
};

class NatTable {
private:
	std::string publicIp;
	uint16_t nextAvailablePort;
	std::chrono::seconds timeoutDuration;

	std::map<PrivateKey, NatEntry> outboundTraffic;
	std::map<uint16_t, PrivateKey> inboundTraffic;

	std::queue<uint16_t> freePortPool;

	// Background cleanup thread
	mutable std::mutex mtx;
	std::atomic<bool> stopCleanup{false};
	std::thread cleanupThread;
	std::chrono::seconds cleanupInterval;

	void cleanupLoop();

public:
	explicit NatTable(const std::string &publicIp, std::chrono::seconds timeout = std::chrono::seconds(60),
	                  bool enableBackgroundCleanup = false,
	                  std::chrono::seconds cleanupInterval = std::chrono::seconds(5));
	~NatTable();

	// Background cleanup control
	void startBackgroundCleanup(std::chrono::seconds interval = std::chrono::seconds(5));
	void stopBackgroundCleanup();

	NatEntry *findByPrivate(const std::string &privateIp, uint16_t privatePort);
	NatEntry *findByPublicPort(uint16_t publicPort);
	NatEntry *createMapping(const std::string &privateIp, uint16_t privatePort);
	void removeExpired();
	void printTable() const;
};
