cmake_minimum_required(VERSION 3.28)

project("1B-row-challenge-data")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(data-generator "data-generator.cpp")