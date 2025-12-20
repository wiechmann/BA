#include <iostream>
#include <memory>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cmath>
#include <cstring>

#include "compressor.h"
#include "benchmark.h"
#include "io.h"
#include "utils.h"


const std::vector<std::string> datasets = {
    "msg_bt",
    "msg_lu",
    "msg_sp",
    "msg_sppm",
    "msg_sweep3d",
    "num_brain",
    "num_comet",
    "num_control",
    "num_plasma",
    "obs_error",
    "obs_info",
    "obs_spitzer",
    "obs_temp",
};


int main(int argc, char const* argv[]) {

    // single
    std::vector<std::unique_ptr<Compressor>> gpCompressors;
    gpCompressors.push_back(std::make_unique<Lz4Compressor>());
    gpCompressors.push_back(std::make_unique<SnappyCompressor>());
    gpCompressors.push_back(std::make_unique<ZstdCompressor>());
    gpCompressors.push_back(std::make_unique<BrotliCompressor>());

    std::vector<std::unique_ptr<Compressor>> fpCompressors;
    fpCompressors.push_back(std::make_unique<SpdpCompressor>());
    fpCompressors.push_back(std::make_unique<FpzipCompressor<float>>());
    fpCompressors.push_back(std::make_unique<NdzipCompressor<float, uint32_t>>());

    std::string prefix = "../data/single/";
    std::string suffix = ".sp";
    std::string outputFile = "../results/single.csv";

    size_t repeat = 10;

    for (const auto& dataset : datasets) {

        std::unique_ptr<char[]> buffer;
        size_t size = readData(prefix + dataset + suffix, buffer);

        for (const auto& compressor : gpCompressors) {
            for (int level : compressor->getLevels()) {

                Result result = benchmark<float>(compressor, buffer.get(), size, level, false, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
                result = benchmark<float>(compressor, buffer.get(), size, level, true, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
            }
        }

        for (const auto& compressor : fpCompressors) {
            for (int level : compressor->getLevels()) {

                Result result = benchmark<float>(compressor, buffer.get(), size, level, false, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
            }
        }
    }

    // double
    fpCompressors[fpCompressors.size() - 2] = std::make_unique <FpzipCompressor<double>>();
    fpCompressors[fpCompressors.size() - 1] = std::make_unique <NdzipCompressor<double, uint64_t>>();

    prefix = "../data/double/";
    suffix = ".dp";
    outputFile = "../results/double.csv";

    for (const auto& dataset : datasets) {

        std::unique_ptr<char[]> buffer;
        size_t size = readData(prefix + dataset + suffix, buffer);

        for (const auto& compressor : gpCompressors) {
            for (int level : compressor->getLevels()) {

                Result result = benchmark<double>(compressor, buffer.get(), size, level, false, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
                result = benchmark<double>(compressor, buffer.get(), size, level, true, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
            }
        }
        for (const auto& compressor : fpCompressors) {
            for (int level : compressor->getLevels()) {

                Result result = benchmark<double>(compressor, buffer.get(), size, level, false, repeat);
                result.dataset = dataset;
                writeResult2csv(outputFile, result);
            }
        }
    }
    
    return EXIT_SUCCESS;
}