#include <nlohmann/json.hpp>
#include <iostream>

int main(void) {
	nlohmann::json whatever;
	whatever["poop"] = "train";
	std::cout << "hello " << whatever << '\n';
}