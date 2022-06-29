#include <iostream>
#include <thread>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/version.h>
#include <chrono>
#include <thread>
#include <fmt/format.h>

// --------------------------------------------------------------------------
// The thread function. This is run in a separate thread for each socket.
// Ownership of the socket object is transferred to the thread, so when this
// function exits, the socket is automatically closed.

#pragma pack(push, 1)
struct PacketHeader {
		uint32_t sync;
		uint16_t len;
		int16_t  id;
};

struct Command {
		float   value;
		uint8_t boolean;
};

struct Telemetry {
		float   value;
		uint8_t boolean;
};

template<typename PacketData>
struct Packet {
		PacketHeader hdr;
		PacketData   data;
};
#pragma pack(pop)

void run_echo(sockpp::tcp_socket sock) {
	ssize_t      n;
	PacketHeader hdr;
	Command      cmd;
	Telemetry    tlm;
	size_t       p = 0;

	while ((n = sock.read(&hdr, sizeof(PacketHeader))) > 0) {
		std::cout << "received " << n << ": " << hdr.id << ',' << hdr.len << '\n';
		p = 0;
		while (p < sizeof(hdr)) {
			std::cout << fmt::format("{0:#x}\n", (int)*((uint8_t*)(&hdr) + p++));
		}
		n = sock.read(&cmd, hdr.len - sizeof(PacketHeader));
		if (n <= 0) {
			std::cerr << "couldnt read the packet data\n";
			continue;
		}
		std::cout << "received " << n << ": " << cmd.value << ',' << (int)cmd.boolean << '\n';
		while (p < sizeof(cmd)) {
			std::cout << fmt::format("{0:#x}\n", (int)*((uint8_t*)(&cmd) + p++));
		}
		// tlm.hdr.sync = 0xBA5EBA11;
		hdr.sync = 0x11BA5EBA; // byte order is swapped as compared to cosmos
		hdr.len  = sizeof(Telemetry) + sizeof(PacketHeader);
		hdr.id   = hdr.id;
		int i    = 30;
		while (i > 0) {
			tlm.value   = cmd.value + i;
			tlm.boolean = (uint8_t)tlm.value % 2;
			Packet<Telemetry> pkt{
				.hdr  = hdr,
				.data = tlm,
			};
			if (sock.write(&pkt, sizeof(pkt)) != sizeof(pkt)) {
				std::cerr << "Error writing to the TCP stream: " << sock.last_error_str() << '\n';
				break;
			}

			std::cout << "sent: " << tlm.value << ',' << (int)tlm.boolean << '\n';

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			--i;
		}
	}

	std::cout << "Connection closed from " << sock.peer_address() << '\n';
}

// --------------------------------------------------------------------------
// The main thread runs the TCP port acceptor. Each time a connection is
// made, a new thread is spawned to handle it, leaving this main thread to
// immediately wait for the next connection.

int main(int argc, char* argv[]) {
	std::cout << "Sample TCP echo server for 'sockpp' " << sockpp::SOCKPP_VERSION << '\n' << '\n';

	in_port_t port = (argc > 1) ? atoi(argv[1]) : 12345;

	sockpp::socket_initializer sockInit;

	sockpp::tcp_acceptor acc(port);

	if (!acc) {
		std::cerr << "Error creating the acceptor: " << acc.last_error_str() << '\n';
		return 1;
	}
	std::cout << "Acceptor bound to address: " << acc.address() << '\n';
	std::cout << "Awaiting connections on port " << port << "..." << '\n';

	while (true) {
		sockpp::inet_address peer;

		// Accept a new client connection
		sockpp::tcp_socket sock = acc.accept(&peer);
		std::cout << "Received a connection request from " << peer << '\n';

		if (!sock) {
			std::cerr << "Error accepting incoming connection: " << acc.last_error_str() << '\n';
		} else {
			// Create a std::thread and transfer the new stream to it.
			std::thread thr(run_echo, std::move(sock));
			thr.detach();
		}
	}

	return 0;
}
