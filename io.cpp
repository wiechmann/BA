#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "benchmark.h"
#include "io.h"

size_t readData(const std::string& path, std::unique_ptr<char[]>& buffer) {
    std::ifstream in(path, std::ifstream::binary);
    if (!in.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return 0;
    }
    size_t size = std::filesystem::file_size(path);
    buffer = std::make_unique<char[]>(size);
    in.read(buffer.get(), size);
    in.close();
    return size;
}

void writeResult2csv(const std::string& path, const Result& result, bool writeHeader) {
    std::ofstream out(path, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }
    if (writeHeader && std::filesystem::file_size(path) == 0) {
        out << Result::csvHeader();
    }
    out << result.toCSV() << std::endl;
    out.close();
}