#include "Packet.hpp"
#include <iostream>

Packet::Packet(const std::string &srcIp, uint16_t srcPort, const std::string &destIp, uint16_t destPort,
               Protocol proto) :
		sourceIp(srcIp), sourcePort(srcPort), destinationIp(destIp), destinationPort(destPort), protocol(proto) {}

const std::string &Packet::getSourceIp() const { return sourceIp; }

uint16_t Packet::getSourcePort() const { return sourcePort; }

const std::string &Packet::getDestinationIp() const { return destinationIp; }

uint16_t Packet::getDestinationPort() const { return destinationPort; }

const Protocol &Packet::getProtocol() const { return protocol; }

void Packet::setSourceIp(const std::string &srcIp) { sourceIp = srcIp; }

void Packet::setSourcePort(uint16_t srcPort) { sourcePort = srcPort; }

void Packet::setDestinationIp(const std::string &destIp) { destinationIp = destIp; }

void Packet::setDestinationPort(uint16_t destPort) { destinationPort = destPort; }

void Packet::print() const {
	std::cout << (protocol == Protocol::TCP ? "TCP" : "UDP") << " " << sourceIp << ":" << sourcePort << " -> "
						<< destinationIp << ":" << destinationPort << std::endl;
}
