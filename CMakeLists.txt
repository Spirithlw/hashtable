cmake_minimum_required(VERSION 3.15)
project(hashtable)

set(CMAKE_CXX_STANDARD 14)

add_executable(hashtable hashtable.cpp hashes.h list.h files.h)