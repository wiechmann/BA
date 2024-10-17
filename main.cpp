#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "compress.h"
#include "helper.h"
#include "stats.h"

int read_data(const char *infile, char **buffer, size_t &size) {
    std::ifstream in(infile, std::ifstream::binary);
    if (in.is_open()) {
        size = std::filesystem::file_size(infile);
        *buffer = (char *)malloc(size);
        in.read(*buffer, size);
        in.close();
    } else
        return -1;
    return 0;
}

int write_header(std::ofstream &out, size_t bytes) {
    if (out.is_open()) {
        std::string data_type = (bytes == 4) ? "singles" : "doubles";
        out
            << ",size (megabytes),"
            << data_type
            << " (millions),unique values (percent),1st order entropy (bits),randomness (percent)";
        for (size_t i = 0; i < bytes; ++i) {
            out
                << ",entropy byte "
                << i;
            if (i == 0) {
                out << " / lsb";
            }
            if (i == (bytes - 1)) {
                out << " / msb";
            }
            out << " (bits)";
        }
        out << std::endl;
    } else
        return -1;
    return 0;
}

int write_statistics(std::ofstream &out, const char *infile, size_t size, size_t bytes, double unique, double random, double entropy, double byte_entropies[]) {
    size_t n{size / bytes};
    std::string filename{infile};
    filename = filename.substr(filename.find_last_of("/\\") + 1);
    if (out.is_open()) {
        out
            // infile without path and extension
            << filename.substr(0, filename.find_last_of('.'))
            << ","
            << std::fixed << std::setprecision(1)
            // size of file in megabytes
            << floor((((double)(size) / (1 << 20)) * 10.0 + 0.5)) / 10.0  // round to tenth
            << ","
            << std::setprecision(2)
            // number of values in millions
            << floor(n / pow(10, 4) + 0.5) / 100  // round to hundredth
            << ","
            << std::setprecision(1)
            // unique values in percent
            << floor((double)unique / n * 1000 + 0.5) / 10  // round to tenth
            << ","
            << std::setprecision(2)
            // first order entropy in bits
            << floor(entropy * 100 + 0.5) / 100  // round to hundredth
            << ","
            << std::setprecision(1)
            // randomness in percent
            << floor(random * 10 + 0.5) / 10;  // round to tenth;
        for (size_t i = 0; i < bytes; ++i) {
            out
                << ","
                << std::setprecision(2)
                // first order entropy of byte number i in bits
                << floor(byte_entropies[i] * 100 + 0.5) / 100;  // round to hundredth
        }
        out << std::endl;
    } else
        return -1;
    return 0;
}

int main(int argc, char const *argv[]) {
    // usage: ./main 4|8 outfile infile...
    if (argc > 2) {
        const size_t bytes{atoi(argv[1])};
        const char *outfile{argv[2]};
        char *buffer;
        uint8_t *reordered;
        size_t size, n, unique, unique_bytes;
        double random, entropy;
        double byte_entropies[bytes];
        std::unordered_map<uint8_t, size_t> count_bytes;

        std::ofstream out(outfile, std::ios::app);
        if (!out.is_open()) {
            std::cout << "Error writing file " << outfile << std::endl;
            return EXIT_FAILURE;
        }
        write_header(out, bytes);

        for (size_t i = 3; i < argc; ++i) {
            const char *infile{argv[i]};
            if (read_data(infile, &buffer, size)) {
                std::cout << "Error reading file " << infile << std::endl;
                continue;
            }
            n = size / bytes;
            reordered = (uint8_t *)malloc(size);
            if (bytes == 4) {
                std::unordered_map<uint32_t, size_t> count;
                unique = unique_values((uint32_t *)buffer, count, n);
                entropy = first_order_entropy(count, n);
                // rotate((uint32_t *)buffer, size, true);
                reorder((uint32_t *)buffer, reordered, size);
            } else {
                std::unordered_map<uint64_t, size_t> count;
                unique = unique_values((uint64_t *)buffer, count, n);
                entropy = first_order_entropy(count, n);
                // rotate((uint64_t *)buffer, size, true);
                reorder((uint64_t *)buffer, reordered, size);
            }
            random = randomness(unique, entropy);
            for (size_t i = 0; i < bytes; ++i) {
                count_bytes.clear();
                unique_bytes = unique_values(reordered + i * n, count_bytes, n);
                byte_entropies[i] = first_order_entropy(count_bytes, n);
            }

            write_statistics(out, infile, size, bytes, unique, random, entropy, byte_entropies);
            free(reordered);
        }
        out.close();
        free(buffer);
    }
    return EXIT_SUCCESS;
}
