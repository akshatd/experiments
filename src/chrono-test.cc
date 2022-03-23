#include <iostream>
#include "chrono-wrapper.h"

int main(void) {
	std::cout << "hello\n";
	chronowrapper::sleep_for(chronowrapper::seconds(1));
	std::cout << "hello after 1 sec\n";
}