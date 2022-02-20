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
#get_target_property(src tg-testing-qml SOURCES)
#get_target_property(link tg-testing-qml LINK_LIBRARIES)
#cmake_print_variables(src link)

#cmake_minimum_required(VERSION 3.21)
#project(test_meta)
find_package(Qt6 COMPONENTS Widgets Gui)
add_library(qt_test_lib STATIC test_lib.cpp test_lib.hpp)
target_link_libraries(qt_test_lib PUBLIC Qt6::Gui Qt6::Widgets)
add_executable(qt_test test.cpp)
target_link_libraries(qt_test PRIVATE qt_test_lib)

feature_summary(WHAT ALL)