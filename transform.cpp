#include <string>
#include <filesystem>
#include <fstream>
#include <omp.h>
#include "transform.h"


template<typename T>
void shuffleBytes(const char* src, char* dest, size_t size) {
    size_t bytes = sizeof(T);
    size_t n = size / bytes;
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) { 
        for (size_t j = 0; j < bytes; ++j) { 
            dest[j * n + i] = src[i * bytes + j];
        }
    }
}

template<typename T>
void unShuffleBytes(const char* src, char* dest, size_t size) {
    size_t bytes = sizeof(T);
    size_t n = size / bytes;
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < bytes; ++j) { 
            dest[i * bytes + j] = src[j * n + i];
        }
    }
}


template void shuffleBytes<float>(const char*, char*, size_t);
template void shuffleBytes<double>(const char*, char*, size_t);

template void unShuffleBytes<float>(const char*, char*, size_t);
template void unShuffleBytes<double>(const char*, char*, size_t);
