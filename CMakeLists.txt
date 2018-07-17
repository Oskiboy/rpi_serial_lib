cmake_minimum_required(VERSION 3.0.0)
project(SERIAL_LIB)

add_compile_options(-g -Wall -std=c++17)

include_directories(include)

file(GLOB SERIAL_SRC "src/*.cpp")

add_library(serial_lib ${SERIAL_SRC})

