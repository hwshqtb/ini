cmake_minimum_required(VERSION 3.10)

project(ini)

set(CMAKE_CXX_STANDARD 17)

add_library(hwshqtb_ini INTERFACE)
add_library(hwshqtb::ini ALIAS hwshqtb_ini)

target_include_directories(hwshqtb_ini INTERFACE include)

add_subdirectory(test)