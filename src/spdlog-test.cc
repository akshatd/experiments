#include <iostream>
#include <string>
#include <variant>
#include <cstdint>
#include <chrono>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "jsonlog.h"

// nlohman json adds curly braces, so remove those
#define JSON_STRING(json) (json.dump().substr(1, json.dump().length() - 2))

inline std::string jsonToLogString(nlohmann::json j) {
	if (!j.is_object()) throw std::invalid_argument("argument must be an object");
	std::string s = j.dump();
	return std::string(std::next(s.begin(), 1), std::prev(s.end(), 1));
}

int main(void) {
	int            hmm       = 34;
	nlohmann::json some_json = {
		{  "pi", 3.141},
    {"poop",   hmm}
  };

	JsonLog some_jsonlog({
		{  "pi", 3.141},
    {"poop",   hmm}
  });

	std::cout << "raw jsons: " << some_json << ", " << some_jsonlog << '\n';

	spdlog::info("Welcome to spdlog!");
	// change log pattern
	spdlog::set_pattern(JSONLOG_SPDLOG_PATTERN);
	spdlog::error("Some error message with arg: {}", 1);
	spdlog::warn("Easy padding in numbers like {:08d}", 12);
	spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
	spdlog::info("Support for floats {:03.2f}", 1.23456);
	spdlog::info("Positional args are {1} {0}..", "too", "supported");
	spdlog::info("{:<30}", "left aligned");
	spdlog::warn(jsonToLogString({
		{"fuck",   69},
    { "you", true}
  }));
	// spdlog::warn(JsonLog({
	// 	{"fuck",   69},
	//   { "you", true}
	// }));
	int iter = 100000;
	spdlog::info("json");
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iter; ++i) {
		// spdlog::info(JSON_STRING(some_json));
		spdlog::info(jsonToLogString(some_json));
	}
	auto json_spent =
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

	spdlog::info("jsonlog");
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iter; ++i) {
		// spdlog::info(some_jsonlog);
	}
	auto jsonlog_spent =
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

	std::cout << "formatted jsons: " << jsonToLogString(some_json) << ", " << some_jsonlog << '\n';
	spdlog::info("json: {}, jsonlog: {}", json_spent.count(), jsonlog_spent.count());

	spdlog::set_level(spdlog::level::debug); // Set global log level to debug
	spdlog::debug("This message should be displayed..");

	// Compile time log levels
	// define SPDLOG_ACTIVE_LEVEL to desired level
	SPDLOG_TRACE("Some trace message with param {}", 42);
	SPDLOG_DEBUG("Some debug message");

	return 0;
}
