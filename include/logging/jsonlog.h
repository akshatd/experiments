#ifndef JSONLOG_H
#define JSONLOG_H

#include <iomanip> // quoted
#include <iostream>
#include <string>
#include <variant>
#include <map>
// https://github.com/gabime/spdlog#user-defined-types
// must be included so spdlog can print this out
// #include <spdlog/fmt/ostr.h>, but this internall just includes the fmt header
#include <fmt/ostream.h>

#define JSONLOG_SPDLOG_PATTERN "%^{\"datetime\":\"%Y-%m-%dT%H:%M:%S.%e\" %v}%$"

// helper type for the variant visitor
// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... {
		using Ts::operator()...;
};

struct JsonLog {
		using json_val = std::variant<long, double, bool, std::string>;
		std::map<std::string, json_val> json;

		// this constructor overload is to prevent the extra brace while constructing
		// https://docs.microsoft.com/en-us/cpp/cpp/initializing-classes-and-structs-without-constructors-cpp?view=msvc-170

		// using std::initializer_list<std::pair> instead of std::map directly because if you initialise this with one key
		// value pair there will be ambiguity between a brace-enclosed list and the copy/move constructor (const JsonLog& or
		// JsonLog&&) this is because there is only one member variable in here, <json> which has the same constructor
		// signature as JsonLog so it will get confused between initialising that or the whole struct
		JsonLog(std::initializer_list<std::pair<const std::string, json_val>> json_in) : json{json_in} {}

		// for spdlog https://github.com/gabime/spdlog#user-defined-types
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const JsonLog& json_log) {
			return os << json_log;
		}
};

inline std::ostream& operator<<(std::ostream& os, const JsonLog& json) {
	for (const auto& kv : json.json) {
		os << ", " << std::quoted(kv.first) << ":";
		std::visit(
			overloaded{
				[&os](auto arg) { os << arg; },                      //
				[&os](double arg) { os << std::fixed << arg; },      // fixed to 6, milli
				[&os](bool arg) { os << (arg ? "true" : "false"); }, //
				[&os](const std::string& arg) { os << std::quoted(arg); }},
			kv.second);
	}
	return os;
}

#endif