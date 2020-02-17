# cmake_minimum_required(VERSION <specify CMake version here>)
project(cpptest)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpptest main.cpp import/import_resolver.cpp import/import_resolver.hpp import/import.hpp)