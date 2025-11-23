#pragma once

#include <thread>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <iostream>


void printTime() {
    const std::chrono::seconds offset{ 2 * 60 * 60 };
    auto now = std::chrono::system_clock::now();
    auto local_time = now + offset;
    std::time_t t_local = std::chrono::system_clock::to_time_t(local_time);
    std::tm tm_local = *std::localtime(&t_local);
    std::cout << std::put_time(&tm_local, "%Y-%m-%d %H:%M:%S") << std::endl;
}

void printDuration(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    std::chrono::duration<double, std::ratio<60>> elapsed_minutes = end - start;
    std::cout << "Dauer: " << elapsed_minutes.count() << " Minuten" << std::endl;
}

void printThreads() {
    size_t numThreads = std::thread::hardware_concurrency();
    std::cout << "Max threads: " << numThreads << std::endl;
}