#include <rpc/client.h>
#include <spdlog/spdlog.h>

int main() {
	rpc::client c("127.0.0.1", 8080);

	int a = c.call("add", 2, 3).as<int>();
	spdlog::info("int received {}\n", a);

	c.call("foo");

	float f = c.call("divide", 5.1, 3.9).as<float>();
	spdlog::info("float received {}\n", f);

	return 0;
}