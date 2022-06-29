
#include <iostream>
#include <string>
#include <sockpp/tcp_connector.h>
#include <sockpp/version.h>
#include <cstdint>

using namespace std::chrono;

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

int main(int argc, char* argv[]) {
	std::cout << "Sample TCP echo client for 'sockpp' " << sockpp::SOCKPP_VERSION << '\n' << '\n';

	std::string host = (argc > 1) ? argv[1] : "localhost";
	in_port_t   port = (argc > 2) ? atoi(argv[2]) : 12345;

	sockpp::socket_initializer sockInit;

	// Implicitly creates an inet_address from {host,port}
	// and then tries the connection.

	sockpp::tcp_connector conn({host, port});
	if (!conn) {
		std::cerr << "Error connecting to server at " << sockpp::inet_address(host, port) << "\n\t" << conn.last_error_str()
							<< '\n';
		return 1;
	}

	std::cout << "Created a connection from " << conn.address() << '\n';

	// Set a timeout for the responses
	if (!conn.read_timeout(seconds(5))) {
		std::cerr << "Error setting timeout on TCP stream: " << conn.last_error_str() << '\n';
	}

	PacketHeader cmd_hdr{
		.sync = 0x11BA5EBA,
		.len  = sizeof(Command) + sizeof(PacketHeader),
		.id   = 1,
	};
	Command cmd{
		.value   = 25.5,
		.boolean = 1,
	};
	Packet<Command> cmd_pkt{
		.hdr  = cmd_hdr,
		.data = cmd,
	};

	PacketHeader tlm_hdr{
		.sync = 0x11BA5EBA,
		.len  = sizeof(Telemetry) + sizeof(PacketHeader),
		.id   = 1,
	};
	Telemetry         tlm;
	Packet<Telemetry> tlm_pkt{
		.hdr  = tlm_hdr,
		.data = tlm,
	};

	std::string s;
	int         count = 0;
	while (std::getline(std::cin, s) && !s.empty()) {
		cmd_pkt.data.value = count;
		if (conn.write(&cmd_pkt, sizeof(cmd_pkt)) != sizeof(cmd_pkt)) {
			std::cerr << "Error writing to the TCP stream: " << conn.last_error_str() << '\n';
			break;
		}

		ssize_t n = conn.read(&tlm_pkt, sizeof(tlm_pkt));

		if (n != sizeof(tlm_pkt)) {
			std::cerr << "Error reading from TCP stream: " << conn.last_error_str() << '\n';
			break;
		}

		std::cout << tlm_pkt.hdr.id << ',' << tlm_pkt.data.value << ',' << (int)tlm_pkt.data.boolean << '\n';
		++count;
	}

	return (!conn) ? 1 : 0;
}