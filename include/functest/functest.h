#ifndef FUNCTEST_H
#define FUNCTEST_H

#include <bitset>
#include <string>
#include <functional>
#include <vector>

constexpr int kIsFT  = 0;
constexpr int kIsRFT = 1;

struct FunctionalTest;

static std::vector<FunctionalTest> all_tests;

struct FunctionalTest {
		std::string               name;
		std::string               description;
		std::function<bool(void)> run;
		FunctionalTest(std::string name, std::string description, std::function<bool(void)> run)
				: name{name}, description{description}, run{run} {
			all_tests.push_back(*this);
		}
};

FunctionalTest test1("hello", "world", []() { return true; });
FunctionalTest test3("helslo", "world", []() { return true; });

#endif
