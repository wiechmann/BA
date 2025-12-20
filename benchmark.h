#pragma once

#include <ctime>
#include <string>
#include <iomanip>
#include <sstream> 
#include <iostream>
#include <vector>
#include "compressor.h"
#include "transform.h"


struct Result {
    std::string dataset;
    std::string compressorName;
    int compressionLevel;
    size_t uncompressedSize;
    size_t compressedSize;
    double shuffleTimeComp;
    double shuffleTimeDe;
    double compressionTime;
    double decompressionTime;

    std::string toCSV() const;
    static std::string csvHeader();
};

double timespecMilliseconds(std::timespec start, std::timespec end);

template<typename T>
Result benchmark(const std::unique_ptr<Compressor>& compressor,
    char* uncompressed,
    size_t uncompressedSize,
    int level,
    bool shuffle,
    size_t repeat = 10);

