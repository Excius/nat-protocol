#include "NatRouter.hpp"

#include <iostream>

NatRouter::NatRouter(NatTable &table) : table(table) {}

void NatRouter::handleOutbound(Packet &packet) const {
	table.removeExpired();

	const NatEntry *entry = table.findByPrivate(packet.getSourceIp(), packet.getSourcePort());

	if (entry == nullptr) {
		entry = table.createMapping(packet.getSourceIp(), packet.getSourcePort());
		if (entry == nullptr) {
			std::cout << "[DROP: TABLE_FULL] ";
			packet.print();
			std::cout << std::endl;
			return;
		}

		std::cout << "[MAP] ";
		entry->print();
		std::cout << std::endl;
	}

	packet.setSourceIp(entry->getPublicIp());
	packet.setSourcePort(entry->getPublicPort());

	std::cout << "[OUT] ";
	packet.print();
	std::cout << std::endl;
}


void NatRouter::handleInbound(Packet &packet) const {
	table.removeExpired();

	const NatEntry *const entry = table.findByPublicPort(packet.getDestinationPort());
	if (entry == nullptr) {
		std::cout << "[DROP: NO_MAPPING] ";
		packet.print();
		std::cout << std::endl;
		return;
	}

	packet.setDestinationIp(entry->getPrivateIp());
	packet.setDestinationPort(entry->getPrivatePort());

	std::cout << "[IN] ";
	packet.print();
	std::cout << std::endl;
}
