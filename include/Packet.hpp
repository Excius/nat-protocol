#pragma once

#include <cstdint>
#include <string>

enum class Protocol { TCP, UDP };

class Packet {
private:
	std::string sourceIp;
	uint16_t sourcePort;

	std::string destinationIp;
	uint16_t destinationPort;

	Protocol protocol;


public:
	// Constructor
	Packet(const std::string &srcIp, uint16_t srcPort, const std::string &destIp, uint16_t destPort, Protocol proto);

	// Getters
	[[nodiscard]] const std::string &getSourceIp() const;
	[[nodiscard]] uint16_t getSourcePort() const;

	[[nodiscard]] const std::string &getDestinationIp() const;
	[[nodiscard]] uint16_t getDestinationPort() const;

	[[nodiscard]] const Protocol &getProtocol() const;

	// Setters
	void setSourceIp(const std::string &srcIp);
	void setSourcePort(uint16_t srcPort);
	void setDestinationIp(const std::string &destIp);
	void setDestinationPort(uint16_t destPort);

	// Utility
	void print() const;
};
