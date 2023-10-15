cmake_minimum_required(VERSION 2.8.12)

project(mxt)

set(CMAKE_CXX_FLAGS  "-Wfatal-errors -g  -O3 -pedantic -std=c++14 ${CMAKE_CXX_FLAGS}")

add_executable(trie_queue tests/trie_queue.cc)
add_executable(radix_sort tests/radix_sort.cc)
add_executable(maxtree_trie tests/maxtree_trie.cc src/bit_array.cc)
add_executable(area_example area_example.cc src/bit_array.cc)