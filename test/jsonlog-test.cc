#include <sstream>
#include <string>
// #include <fmt/format.h>
#include <catch2/catch.hpp>

#include "jsonlog.h"

TEST_CASE("jsonlog: Test ", "whatever") {
	JsonLog test_log({
		{  "pi",   3.141},
		{"poop", "train"},
		{"hell",    true},
	});

	std::cout << test_log << '\n';
	// TODO: check properly with spdlog::sinks::ostream_sink_st
	CHECK(true);
}