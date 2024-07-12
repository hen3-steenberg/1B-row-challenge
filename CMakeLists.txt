cmake_minimum_required(VERSION 3.28)

project("1B-row-challenge-data")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


include_directories("./")
add_subdirectory(data)
add_subdirectory(simple)