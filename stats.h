#ifndef STATS_H
#define STATS_H

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

double randomness(size_t unique, double entropy);

template <typename T>
size_t unique_values(T *buffer, std::unordered_map<T, size_t> &count, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        T value{buffer[i]};
        if (auto search = count.find(value); search != count.end()) {
            (search->second)++;
        } else {
            count.insert({value, 1});
        }
    }
    return count.size();
}

template <typename T>
double first_order_entropy(std::unordered_map<T, size_t> &count, size_t n) {
    double result{0};
    for (const auto &value : count) {
        size_t frequency{value.second};
        double ratio{(double)frequency / n};
        result += ratio * std::log2(ratio);
    }
    return -result;
}

#endif