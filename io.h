#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include "benchmark.h"


size_t readData(const std::string& dataset, std::unique_ptr<char[]>& buffer);

void writeResult2csv(const std::string& path, const Result& result, bool writeHeader = true);