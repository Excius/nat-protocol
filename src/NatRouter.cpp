#include "NatRouter.hpp"
#include "IpUtils.hpp"
#include "Logger.hpp"

NatRouter::NatRouter(NatTable &table) : table(table) {}

void NatRouter::handleOutbound(Packet &packet) const {
	if (!net::isPrivateIPv4(packet.getSourceIp())) {
		net::log::message(net::log::Level::DROP, "INVALID_PRIVATE_IP");
		net::log::packet(net::log::Level::DROP, packet);
		return;
	}

	table.removeExpired();

	const NatEntry *entry = table.findByPrivate(packet.getSourceIp(), packet.getSourcePort());

	if (entry == nullptr) {
		entry = table.createMapping(packet.getSourceIp(), packet.getSourcePort());
		if (entry == nullptr) {
			net::log::message(net::log::Level::DROP, "TABLE_FULL");
			net::log::packet(net::log::Level::DROP, packet);
			return;
		}

		net::log::entry(net::log::Level::MAP, *entry);
	}

	packet.setSourceIp(entry->getPublicIp());
	packet.setSourcePort(entry->getPublicPort());

	net::log::packet(net::log::Level::OUT, packet);
}


void NatRouter::handleInbound(Packet &packet) const {
	table.removeExpired();

	const NatEntry *const entry = table.findByPublicPort(packet.getDestinationPort());
	if (entry == nullptr) {
		net::log::message(net::log::Level::DROP, "NO_MAPPING");
		net::log::packet(net::log::Level::DROP, packet);
		return;
	}

	packet.setDestinationIp(entry->getPrivateIp());
	packet.setDestinationPort(entry->getPrivatePort());

	net::log::packet(net::log::Level::IN, packet);
}
