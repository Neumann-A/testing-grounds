cmake_minimum_required(VERSION 3.17)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE CACHE STRING "CMake message log level (default: VERBOSE)")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMakeJSON_FOUND)
    find_package(CMakeJSON REQUIRED)
endif()

project("testing-grounds.json")


set(MY_MESSAGE "Reconfiguring!")
set(MY_MESSAGE "${MY_MESSAGE}" CACHE STRING "Message to print!" FORCE)
set(MY_MESSAGE "First configure!" CACHE STRING "Message to print!")
message(STATUS "MY_MESSAGE:${MY_MESSAGE}")