#include <chrono>
#include <thread>
#include <iostream>

#include <fmt/format.h>

constexpr int kProgressWidth = 70;
constexpr int kTotal         = 100;

void PrintProgress(int current, int total, int width) {
	int progress = width * current / total;
	std::cout << fmt::format("\r[{:|>{}}{:>{}} {}/{}", "", progress, "]", width - progress, current, total) << std::flush;
}

int main() {
	for (int current = 0; current < kTotal; ++current) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		PrintProgress(current + 1, kTotal, kProgressWidth);
	}
	std::cout << std::endl;
	return EXIT_SUCCESS;
}