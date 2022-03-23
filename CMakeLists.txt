cmake_minimum_required(VERSION 3.22)

project(experiments)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

# set up deps vis conan. CMAKE_BINARY_DIR=the build folder
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# setup some vars
set(SRC_DIR src)
set(INCLUDE_DIR include)

#interface libraries
# https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries
add_library(chrono-wrapper INTERFACE)
target_include_directories(chrono-wrapper INTERFACE ${INCLUDE_DIR}/chrono-wrapper)

add_library(logging INTERFACE ${INCLUDE_DIR})
target_include_directories(logging INTERFACE ${INCLUDE_DIR}/logging)

# executables
add_executable(destructor-test ${SRC_DIR}/destructor-test.cc)

add_executable(chrono-test ${SRC_DIR}/chrono-test.cc)
target_link_libraries(chrono-test chrono-wrapper)

add_executable(easylogging-test ${SRC_DIR}/easylogging-test.cc)
target_link_libraries(easylogging-test logging CONAN_PKG::easyloggingpp)

add_executable(json-test ${SRC_DIR}/json-test.cc)
target_link_libraries(json-test CONAN_PKG::nlohmann_json)

add_executable(spdlog-test ${SRC_DIR}/spdlog-test.cc)
target_link_libraries(spdlog-test CONAN_PKG::spdlog)
