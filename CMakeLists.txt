cmake_minimum_required(VERSION 3.17)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE CACHE STRING "CMake message log level (default: VERBOSE)")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMakeJSON_FOUND)
    find_package(CMakeJSON REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project("testing-grounds.json")
qt_import_qml_plugins(tg-testing-qml)
#qt_add_resources(tg-testing-qml src/qml.qrc PREFIX "/")
get_target_property(src tg-testing-qml SOURCES)
get_target_property(link tg-testing-qml LINK_LIBRARIES)
cmake_print_variables(src link)
