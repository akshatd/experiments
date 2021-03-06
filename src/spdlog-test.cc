#include <iostream>
#include <string>
#include <variant>
#include <cstdint>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "jsonlog.h"

// nlohman json adds curly braces, so remove those
#define JSON_STRING(json) (", " + json.dump().substr(1, json.dump().length() - 2))

int main(void) {
	int            hmm      = 34;
	nlohmann::json whatever = {
		{"pi", 3.141},
	};
	whatever["poop"] = "train";
	JsonLog whatever2({
		{  "pi", 3.141},
    {"poop",   hmm}
  });

	std::cout << "raw jsons: " << whatever << ", " << whatever2 << '\n';

	spdlog::info("Welcome to spdlog!");
	// change log pattern
	spdlog::set_pattern("%^{\"datetime\":\"%Y-%m-%dT%H:%M:%S.%e\" %v}%$");
	spdlog::error("Some error message with arg: {}", 1);
	spdlog::warn("Easy padding in numbers like {:08d}", 12);
	spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
	spdlog::info("Support for floats {:03.2f}", 1.23456);
	spdlog::info("Positional args are {1} {0}..", "too", "supported");
	spdlog::info("{:<30}", "left aligned");
	spdlog::info(JSON_STRING(whatever));
	spdlog::info(whatever2);
	spdlog::warn(JsonLog({
		{"fuck",   69},
    { "you", true}
  }));

	spdlog::set_level(spdlog::level::debug); // Set global log level to debug
	spdlog::debug("This message should be displayed..");

	// Compile time log levels
	// define SPDLOG_ACTIVE_LEVEL to desired level
	SPDLOG_TRACE("Some trace message with param {}", 42);
	SPDLOG_DEBUG("Some debug message");

	return 0;
}
