#include "../include/NatRouter.hpp"
#include "../include/NatTable.hpp"
int main() {

	NatTable table("203.0.113.5");
	NatRouter router(table);

	Packet p1("192.168.1.10", 5000, "8.8.8.8", 53, Protocol::UDP);
	router.handleOutbound(p1);

	Packet p2("192.168.1.10", 6002, "1.1.1.1", 80, Protocol::TCP);
	router.handleOutbound(p2);

	Packet p3("192.168.1.11", 6000, "8.8.4.4", 443, Protocol::TCP);
	router.handleOutbound(p3);

	table.printTable();

	Packet inbound1("8.8.8.8", 53, "203.0.113.5", 40001, Protocol::UDP);
	router.handleInbound(inbound1);

	Packet inbound2("9.9.9.9", 1234, "203.0.113.5", 49999, Protocol::TCP);
	router.handleInbound(inbound2);

	return 0;
}
