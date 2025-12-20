#include <ctime>
#include <string>
#include <iomanip>
#include <sstream> 
#include <iostream>
#include <vector>
#include "compressor.h"
#include "transform.h"
#include "benchmark.h"


std::string Result::toCSV() const {
    std::ostringstream out;
    out << dataset << ","
        << compressorName << ","
        << compressionLevel << ","
        << uncompressedSize << ","
        << compressedSize << ","
        << std::fixed
        << std::setprecision(3)
        << static_cast<double>(compressedSize) / uncompressedSize << ","
        << std::setprecision(0)
        << shuffleTimeComp << ","
        << shuffleTimeDe << ","
        << compressionTime << ","
        << decompressionTime << ","
        << (shuffleTimeComp + shuffleTimeDe + compressionTime + decompressionTime);
    return out.str();
}

std::string Result::csvHeader() {
    return "dataset,compressor,level,size,compressed size,ratio,shuffle time comp,shuffle time de,compression time,decompression time,time\n";
}

std::timespec timespecAdd(std::timespec a, std::timespec b) {
    std::timespec result;
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec >= 1000000000) {
        result.tv_sec++;
        result.tv_nsec -= 1000000000;
    }
    return result;
}

double timespecMilliseconds(std::timespec start, std::timespec end) {
    double ms;
    ms = double(end.tv_sec - start.tv_sec) * 1000.0;
    ms += double(end.tv_nsec - start.tv_nsec) / 1000000.0;
    return ms;
}

template<typename T>
Result benchmark(const std::unique_ptr<Compressor>& compressor,
    char* uncompressed,
    size_t uncompressedSize,
    int level,
    bool shuffle,
    size_t repeat) {

    std::timespec start, end;
    std::timespec totalCompressionTime = {0, 0};
    std::timespec totalDecompressionTime = {0, 0};
    std::timespec totalShuffleTimeComp = {0, 0};
    std::timespec totalShuffleTimeDe = {0, 0};

    size_t compressedSize = 0;
    char* compressed = new char[compressor->compressBounds(uncompressedSize)];
    char* buffer = shuffle ? new char[uncompressedSize] : uncompressed;

    for (size_t i = 0; i < repeat; ++i) {

        // compress
        if (shuffle) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            shuffleBytes<T>(uncompressed, buffer, uncompressedSize);
            clock_gettime(CLOCK_MONOTONIC, &end);
            totalShuffleTimeComp = timespecAdd(totalShuffleTimeComp, {end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec});
        }
        clock_gettime(CLOCK_MONOTONIC, &start);
        compressedSize = compressor->compress(buffer, compressed, uncompressedSize, level);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalCompressionTime = timespecAdd(totalCompressionTime, {end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec});

        // decompress
        clock_gettime(CLOCK_MONOTONIC, &start);
        compressor->decompress(compressed, buffer, compressedSize, uncompressedSize, level);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalDecompressionTime = timespecAdd(totalDecompressionTime, {end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec});
        
        if (shuffle) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            unShuffleBytes<T>(buffer, uncompressed, uncompressedSize);
            clock_gettime(CLOCK_MONOTONIC, &end);
            totalShuffleTimeDe = timespecAdd(totalShuffleTimeDe, {end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec});
        }
    }
    
    if (shuffle) {
        delete[] buffer;
    }
    delete[] compressed;
    
    return { "",
        compressor->getName(),
        level,
        uncompressedSize,
        compressedSize,
        timespecMilliseconds({0, 0}, totalShuffleTimeComp) / repeat,
        timespecMilliseconds({0, 0}, totalShuffleTimeDe) / repeat,
        timespecMilliseconds({0, 0}, totalCompressionTime) / repeat,
        timespecMilliseconds({0, 0}, totalDecompressionTime) / repeat };
}

template Result benchmark<float>(const std::unique_ptr<Compressor>&, char*, size_t, int, bool, size_t);
template Result benchmark<double>(const std::unique_ptr<Compressor>&, char*, size_t, int, bool, size_t);