#ifndef CHRONO_WRAPPER_H
#define CHRONO_WRAPPER_H

#include <chrono>
#include <thread>
#include <iostream>

// #define TESTING

namespace chronowrapper {
#ifdef TESTING
using seconds = int;
void sleep_for(seconds s) { std::cout << "pretending to sleep for " << s << " seconds\n"; }

#else
using namespace std::chrono;
using namespace std::this_thread;
#endif
} // namespace chronowrapper

#endif
