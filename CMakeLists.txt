cmake_minimum_required(VERSION 3.24)

project(experiments)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif ()
# https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#gcc--clang
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pedantic -Wshadow -Wnon-virtual-dtor \
										-Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion \
										-Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference \
										-Wuseless-cast -Wdouble-promotion -Wformat=2 -Wpadded")

# install deps via Conan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
								"${CMAKE_BINARY_DIR}/conan.cmake"
								TLS_VERIFY ON)
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)
configure_file(${CMAKE_SOURCE_DIR}/conanfile.txt ${CMAKE_BINARY_DIR}/conanfile.txt COPYONLY)
set(CONAN_HOST_PROFILE "default" CACHE STRING "Conan profile of target machine") # change this externally for cross compilation
set(CONAN_BUILD_PROFILE "default" CACHE STRING "Conan profile of target machine")
conan_cmake_install(PATH_OR_REFERENCE . BUILD missing PROFILE_HOST ${CONAN_HOST_PROFILE} PROFILE_BUILD ${CONAN_BUILD_PROFILE})

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
target_link_libraries(functestmain functest)

add_executable(sockpp-tcp-server ${SRC_DIR}/sockpp-tcp-server.cc)
target_link_libraries(sockpp-tcp-server CONAN_PKG::sockpp CONAN_PKG::fmt)

add_executable(sockpp-tcp-client ${SRC_DIR}/sockpp-tcp-client.cc)
target_link_libraries(sockpp-tcp-client CONAN_PKG::sockpp)

add_executable(asio-tcp-server ${SRC_DIR}/asio-tcp-server.cc)
target_link_libraries(asio-tcp-server CONAN_PKG::asio)

add_executable(asio-tcp-client ${SRC_DIR}/asio-tcp-client.cc)
target_link_libraries(asio-tcp-client CONAN_PKG::asio)

include(Catch)

add_executable(jsonlog-test ${TEST_DIR}/jsonlog-test.cc)
target_link_libraries(jsonlog-test PRIVATE logging CONAN_PKG::catch2)
catch_discover_tests(jsonlog-test)
