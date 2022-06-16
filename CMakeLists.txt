cmake_minimum_required(VERSION 3.22)

project(experiments)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-psabi")

# set up deps vis conan. CMAKE_BINARY_DIR=the build folder
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# setup some vars
set(SRC_DIR src)
set(INCLUDE_DIR include)
set(TEST_DIR test)

#interface libraries
# https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries
add_library(chrono-wrapper INTERFACE)
target_include_directories(chrono-wrapper INTERFACE ${INCLUDE_DIR}/chrono-wrapper)

add_library(logging INTERFACE ${INCLUDE_DIR})
target_include_directories(logging INTERFACE ${INCLUDE_DIR}/logging)
target_link_libraries(logging INTERFACE CONAN_PKG::fmt)

add_library(functest INTERFACE)
target_include_directories(functest INTERFACE ${INCLUDE_DIR}/functest)

# executables
add_executable(destructor-test ${SRC_DIR}/destructor-test.cc)

add_executable(chrono-test ${SRC_DIR}/chrono-test.cc)
target_link_libraries(chrono-test chrono-wrapper)

add_executable(easylogging-test ${SRC_DIR}/easylogging-test.cc)
target_link_libraries(easylogging-test logging CONAN_PKG::easyloggingpp)

add_executable(spdlog-test ${SRC_DIR}/spdlog-test.cc)
target_link_libraries(spdlog-test logging CONAN_PKG::spdlog CONAN_PKG::nlohmann_json)

add_executable(invertigo ${SRC_DIR}/invertigo.cc)

add_executable(rpclib-server ${SRC_DIR}/rpclib-server.cc)
target_link_libraries(rpclib-server logging CONAN_PKG::spdlog CONAN_PKG::rpclib)

add_executable(rpclib-client ${SRC_DIR}/rpclib-client.cc)
target_link_libraries(rpclib-client logging CONAN_PKG::spdlog CONAN_PKG::rpclib)

add_executable(fmt-progress ${SRC_DIR}/fmt-progress.cc)
target_link_libraries(fmt-progress CONAN_PKG::fmt)

add_executable(functestmain ${SRC_DIR}/functestmain.cc)
target_link_libraries(functestmain PRIVATE functest)

include(Catch)

add_executable(jsonlog-test ${TEST_DIR}/jsonlog-test.cc)
target_link_libraries(jsonlog-test PRIVATE logging CONAN_PKG::catch2)
catch_discover_tests(jsonlog-test)
