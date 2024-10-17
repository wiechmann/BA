#ifndef COMPRESS_H
#define COMPRESS_H

#include <iostream>

template <typename T>
void rotate(T *buffer, size_t size, bool left) {
    size_t n{size / sizeof(T)};
    size_t offset{sizeof(T) * 8 - 1};
    for (size_t i = 0; i < n; ++i) {
        T mask = left ? (buffer[i] & (1ULL << offset)) >> offset : (buffer[i] & 1ULL) << offset;
        buffer[i] = left ? (buffer[i] << 1) | mask : (buffer[i] >> 1) | mask;
    }
}

template <typename T>
void reorder(T *buffer, uint8_t *reordered, size_t size) {
    size_t offset{sizeof(T)};
    size_t n{size / offset};
    uint8_t *bytes{(uint8_t *)buffer};
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < offset; ++j) {
            reordered[j * n + i] = bytes[i * offset + j];
        }
    }
}

#endif