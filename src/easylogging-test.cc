#include "easylogging-wrapper.h"

INITIALIZE_EASYLOGGINGPP

void foo(int hello) {
	doLog(el::Level::Trace, 2, 3, {{"hello", {hello}}});
	LOG(TRACE) << makeLog(el::Level::Trace, 2, 3, {{"hello", {hello}}});
}

int main(void) {
	InitLogging();
	doLog(
		el::Level::Debug, 2, 3,
		{{"cvx", {1}},
	   {"cvy", {1.5f}},
	   {"ispass", {true}},
	   {"int", {5}},
	   {"float", {4.65f}},
	   {"bool", {false}},
	   {"string", {"messagfe"}}});
	doLog(el::Level::Error, 2, 3, {{"string", {"khf"}}});
	doLog(el::Level::Info, 2, 3, {{"float", {1.5f}}});
	foo(1244);
	// doLog(el::Level::Fatal, 2, 3, {{"integerrr", {1}}});
}
