#pragma once
#include <cstdint>
#include <map>
#include <string>
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
	std::map<PrivateKey, NatEntry> outboundTraffic;
	std::map<uint16_t, PrivateKey> inboundTraffic;

public:
	explicit NatTable(const std::string &publicIp);

	NatEntry *findByPrivate(const std::string &privateIp, uint16_t privatePort);
	NatEntry *findByPublicPort(uint16_t publicPort);
	NatEntry *createMapping(const std::string &privateIp, uint16_t privatePort);
	void removeExpired();
	void printTable() const;
};
