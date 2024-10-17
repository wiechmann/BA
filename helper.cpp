#include "helper.h"

#include <cstdint>
#include <iostream>

void print_binary(float value) {
    uint32_t *float_as_int = (uint32_t *)&value;
    for (size_t i = 0; i < 32; ++i) {
        if ((i == 1) || (i == 9)) {
            std::cout << " ";
        }
        if ((*float_as_int >> (31 - i)) & 1) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

void print_binary(double value) {
    u_int64_t *double_as_int = (uint64_t *)&value;
    for (size_t i = 0; i <= 63; ++i) {
        if ((i == 1) || (i == 12)) {
            std::cout << " ";
        }
        if ((*double_as_int >> (63 - i)) & 1) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

void print_binary(char value) {
    for (int i = 0; i < 8; i++) {
        if ((value >> (7 - i)) & 1) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << " ";
}

void print_binary(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        if ((value >> (7 - i)) & 1) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << " ";
}

void print_binary(uint32_t value) {
    for (size_t i = 0; i < 32; ++i) {
        if ((i == 8) || (i == 16) || (i == 24)) {
            std::cout << " ";
        }
        if ((value >> (31 - i)) & 1) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

void print_binary(uint64_t value) {
    for (int i = 0; i < 64; i++) {
        if ((i == 8) || (i == 16) || (i == 24) || (i == 32) || (i == 40) || (i == 48) || (i == 56)) {
            std::cout << " ";
        }
        if ((value >> (63 - i)) & 1)
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << std::endl;
}
