#pragma once
#include <chrono>
#include <cstdint>
#include <string>

class NatEntry {
private:
	std::string publicIp;
	uint16_t publicPort;
	std::string privateIp;
	uint16_t privatePort;
	std::chrono::steady_clock::time_point lastUsed;

public:
	NatEntry(const std::string &publicIp, uint16_t publicPort, const std::string &privateIp, uint16_t privatePort);

	[[nodiscard]] const std::string &getPublicIp() const;
	[[nodiscard]] const std::string &getPrivateIp() const;
	[[nodiscard]] uint16_t getPublicPort() const;
	[[nodiscard]] uint16_t getPrivatePort() const;
	[[nodiscard]] std::chrono::steady_clock::time_point getLastUsed() const;

	void updateTimestamp();
	void print() const;
};
