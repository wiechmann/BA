#pragma once

#include <string>
#include <filesystem>  // c++17
#include <fstream>
#include <omp.h>
#include <cstddef>

template<typename T>
void shuffleBytes(const char* src, char* dest, size_t size);

template<typename T>
void unShuffleBytes(const char* src, char* dest, size_t size);


