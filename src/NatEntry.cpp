#include "NatEntry.hpp"

#include <iostream>

NatEntry::NatEntry(const std::string &publicIp, uint16_t publicPort, const std::string &privateIp,
                   uint16_t privatePort) :
		publicIp(publicIp), publicPort(publicPort), privateIp(privateIp), privatePort(privatePort),
		lastUsed(std::chrono::steady_clock::now()) {}


const std::string &NatEntry::getPublicIp() const { return publicIp; }
uint16_t NatEntry::getPublicPort() const { return publicPort; }
const std::string &NatEntry::getPrivateIp() const { return privateIp; }
uint16_t NatEntry::getPrivatePort() const { return privatePort; }
std::chrono::steady_clock::time_point NatEntry::getLastUsed() const { return lastUsed; }

void NatEntry::updateTimestamp() { this->lastUsed = std::chrono::steady_clock::now(); }

void NatEntry::print() const {
	std::cout << this->privateIp << ":" << this->privatePort << " -> " << this->publicIp << ":" << this->publicPort;
}
