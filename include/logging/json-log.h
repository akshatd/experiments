#ifndef JSON_LOG_H
#define JSON_LOG_H

#include <iomanip> // quoted
#include <iostream>
#include <string>
#include <variant>
#include <map>
// https://github.com/gabime/spdlog#user-defined-types
// must be included so spdlog can print this out
#include <spdlog/fmt/ostr.h>

// helper type for the variant visitor
// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... {
		using Ts::operator()...;
};

struct JsonLog {
		using json_val = std::variant<long, double, bool, std::string>;
		std::map<std::string, json_val> json;
		// this is to prevent the extra brace while constructing
		// https://docs.microsoft.com/en-us/cpp/cpp/initializing-classes-and-structs-without-constructors-cpp?view=msvc-170
		JsonLog(std::map<std::string, json_val> json_in) : json{json_in} {}

		// for spdlog https://github.com/gabime/spdlog#user-defined-types
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const JsonLog& json_log) {
			return os << json_log;
		}
};

std::ostream& operator<<(std::ostream& os, const JsonLog& json) {
	for (const auto& kv : json.json) {
		os << ", " << std::quoted(kv.first) << ":";
		std::visit(
			overloaded{
				[&os](long arg) { os << arg; },                      //
				[&os](double arg) { os << std::fixed << arg; },      // fixed to 6, milli
				[&os](bool arg) { os << (arg ? "true" : "false"); }, //
				[&os](const std::string& arg) { os << std::quoted(arg); }},
			kv.second);
	}
	return os;
}

#endif