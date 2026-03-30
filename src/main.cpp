#include "../include/NatRouter.hpp"
#include "../include/NatTable.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <string>

static Protocol readProtocol() {
	while (true) {
		std::cout << "Protocol (tcp/udp): ";
		std::string line;
		if (!std::getline(std::cin, line)) {
			return Protocol::UDP;
		}
		std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });
		if (line == "tcp") {
			return Protocol::TCP;
		}
		if (line == "udp") {
			return Protocol::UDP;
		}
		std::cout << "Invalid protocol; please enter 'tcp' or 'udp'.\n";
	}
}

static uint16_t readPort(const std::string &prompt) {
	while (true) {
		std::cout << prompt;
		unsigned int port;
		if (!(std::cin >> port)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid number; try again.\n";
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (port > 0 && port <= 65535) {
			return static_cast<uint16_t>(port);
		}
		std::cout << "Port must be in range 1-65535.\n";
	}
}

static std::string readLine(const std::string &prompt) {
	std::cout << prompt;
	std::string line;
	std::getline(std::cin, line);
	return line;
}

static void runPredefinedScenario(NatRouter &router, NatTable &table, const std::string &publicIp) {
	Packet p1("192.168.1.10", 5000, "8.8.8.8", 53, Protocol::UDP);
	router.handleOutbound(p1);

	Packet p2("192.168.1.10", 6002, "1.1.1.1", 80, Protocol::TCP);
	router.handleOutbound(p2);

	Packet p3("192.168.1.11", 6000, "8.8.4.4", 443, Protocol::TCP);
	router.handleOutbound(p3);

	table.printTable();

	Packet inbound1("8.8.8.8", 53, publicIp, 40001, Protocol::UDP);
	router.handleInbound(inbound1);

	Packet inbound2("9.9.9.9", 1234, publicIp, 49999, Protocol::TCP);
	router.handleInbound(inbound2);
}

static void simulateAttack(NatRouter &router) {
	std::mt19937 rng(12345);
	std::uniform_int_distribution<int> portDist(1024, 60000);

	for (int i = 0; i < 200; ++i) {
		std::string privateIp = "192.168.1." + std::to_string((i % 254) + 1);
		uint16_t privatePort = static_cast<uint16_t>(portDist(rng));
		uint16_t destPort = static_cast<uint16_t>(portDist(rng));
		Packet p(privateIp, privatePort, "8.8.8.8", destPort, Protocol::UDP);
		router.handleOutbound(p);
	}
}

static void runInteractive(NatRouter &router, NatTable &table, const std::string &publicIp) {
	while (true) {
		std::cout << "\n--- NAT Simulator Menu ---\n";
		std::cout << "1) Send outbound packet\n";
		std::cout << "2) Send inbound packet\n";
		std::cout << "3) Print NAT table\n";
		std::cout << "4) Simulate attack (port exhaustion)\n";
		std::cout << "5) Exit\n";
		std::cout << "Select an option: ";

		int choice = 0;
		if (!(std::cin >> choice)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid selection.\n";
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice) {
			case 1: {
				std::string srcIp = readLine("Source IP: ");
				uint16_t srcPort = readPort("Source port: ");
				std::string destIp = readLine("Destination IP: ");
				uint16_t destPort = readPort("Destination port: ");
				Protocol proto = readProtocol();
				Packet packet(srcIp, srcPort, destIp, destPort, proto);
				router.handleOutbound(packet);
				break;
			}
			case 2: {
				std::string srcIp = readLine("Remote source IP: ");
				uint16_t srcPort = readPort("Remote source port: ");
				uint16_t destPort = readPort("Destination public port: ");
				Protocol proto = readProtocol();
				Packet packet(srcIp, srcPort, publicIp, destPort, proto);
				router.handleInbound(packet);
				break;
			}
			case 3:
				table.printTable();
				break;
			case 4:
				simulateAttack(router);
				break;
			case 5:
				return;
			default:
				std::cout << "Unknown option.\n";
				break;
		}
	}
}

int main(int argc, char **argv) {
	const std::string publicIp = "203.0.113.5";

	bool interactive = false;
	bool enableCleanup = false;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--interactive" || arg == "-i") {
			interactive = true;
		} else if (arg == "--background" || arg == "-b") {
			enableCleanup = true;
		}
	}

	NatTable table(publicIp, std::chrono::seconds(60), enableCleanup);
	NatRouter router(table);

	if (interactive) {
		runInteractive(router, table, publicIp);
	} else {
		runPredefinedScenario(router, table, publicIp);
	}

	return 0;
}
