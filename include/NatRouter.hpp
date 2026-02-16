#pragma once
#include "NatTable.hpp"
#include "Packet.hpp"

class NatRouter {
private:
	NatTable &table;

public:
	explicit NatRouter(NatTable &table);
	void handleOutbound(Packet &packet);
	void handleInbound(Packet &packet);
};
