#pragma once

#include <cassert>
#include <string>       
#include <iostream>    
#include <sstream> 
#include <thread>

#include "ndzip.hh"             // https://github.com/celerity/ndzip
#include "offload.hh"       
#include "fpzip.h"              // https://github.com/llnl/fpzip
#include "spdp.h"               // https://userweb.cs.txstate.edu/~burtscher/research/SPDPcompressor/
#include "snappy.h"             // https://github.com/google/snappy
#include "zstd.h"               // https://github.com/facebook/zstd
#include "lz4.h"                // https://github.com/lz4
#include "brotli/decode.h"      // https://github.com/google/brotli
#include "brotli/encode.h"
#include "brotli/port.h"
#include "brotli/shared_dictionary.h"
#include "brotli/types.h"

class Compressor {
protected:
    std::string name;
    std::vector<int> levels;

public:
    Compressor(const std::string& name, const std::vector<int>& levels)
        : name(name), levels(levels) {
    }

    virtual ~Compressor() = default;

    virtual size_t compressBounds(size_t size) = 0;
    virtual size_t compress(const char* in, char*& out, size_t inSize, int level) = 0;
    virtual void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) = 0;

    std::string getName() const { return name; }
    std::vector<int> getLevels() const { return levels; }
};

class SnappyCompressor : public Compressor {
public:
    SnappyCompressor()
        : Compressor("snappy", { 1, 2 }) {
    }

    size_t compressBounds(size_t size) {
        return snappy::MaxCompressedLength(size);
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        size_t outSize = 0;
        snappy::RawCompress(in, inSize, out, &outSize, level);
        return outSize;
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        snappy::RawUncompress(in, inSize, out);
    }

    ~SnappyCompressor() override = default;
};

class Lz4Compressor : public Compressor {
public:
    Lz4Compressor()
        : Compressor("lz4", { -1 }) {
    }

    size_t compressBounds(size_t size) {
        return LZ4_compressBound(size);
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        size_t maxSize = LZ4_compressBound(inSize);
        return LZ4_compress_default(in, out, inSize, maxSize);
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        LZ4_decompress_safe(in, out, inSize, outSize);
    }

    ~Lz4Compressor() override = default;
};

class ZstdCompressor : public Compressor {
public:
    ZstdCompressor()
        : Compressor("zstd", { 1, 9, 18 }) { 
    }

    size_t compressBounds(size_t size) {
        return ZSTD_compressBound(size);
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        size_t numThreads = std::thread::hardware_concurrency();
        size_t maxSize = ZSTD_compressBound(inSize);
        size_t compressedSize = 0;
        if (numThreads > 1) {
            ZSTD_CCtx* cctx = ZSTD_createCCtx();
            ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);
            ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, numThreads);
            compressedSize = ZSTD_compress2(cctx, out, maxSize, in, inSize);
            ZSTD_freeCCtx(cctx);
        }
        else { compressedSize = ZSTD_compress(out, maxSize, in, inSize, level); }

        return compressedSize;
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        ZSTD_decompress(out, outSize, in, inSize);
    }

    ~ZstdCompressor() override = default;
};

class BrotliCompressor : public Compressor {
public:
    BrotliCompressor()
        : Compressor("brotli", { 1, 5, 9 }) { 
    }

    size_t compressBounds(size_t size) {
        return BrotliEncoderMaxCompressedSize(size);
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        size_t outSize = BrotliEncoderMaxCompressedSize(inSize);
        BrotliEncoderCompress(level, BROTLI_DEFAULT_WINDOW, BROTLI_MODE_GENERIC, inSize, (uint8_t*)in, &outSize, (uint8_t*)out);
        return outSize;
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        BrotliDecoderDecompress(inSize, (uint8_t*)in, &outSize, (uint8_t*)out);
    }

    ~BrotliCompressor() override = default;
};

template<typename T, typename U>
class NdzipCompressor : public Compressor {
public:
    NdzipCompressor()
        : Compressor("ndzip", { -1 }) {
    }

    size_t compressBounds(size_t size) {
        ndzip::extent extent(1);        // auslagern
        size_t n = size / sizeof(T);
        extent[0] = n;
        return (ndzip::compressed_length_bound<T>(extent) * sizeof(T));
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        ndzip::extent extent(1);
        size_t n = inSize / sizeof(T);
        extent[0] = n;
        auto offloader = ndzip::make_cpu_offloader<T>(1);
        return offloader->compress((T*)in, extent, (U*)(out)) * sizeof(T);
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        ndzip::extent extent(1);
        size_t n = outSize / sizeof(T);
        extent[0] = n;
        auto offloader = ndzip::make_cpu_offloader<T>(1);
        offloader->decompress((U*)(in), n, (T*)out, extent);
    }

    ~NdzipCompressor() override = default;
};

template<typename T>
class FpzipCompressor : public Compressor {
public:
    FpzipCompressor()
        : Compressor("fpzip", { -1 }) {
    }

    size_t compressBounds(size_t size) {
        return 2 * size;    
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        FPZ* fpz = fpzip_write_to_buffer(out, compressBounds(inSize));
        fpz->type = sizeof(T) == 8 ? 1 : 0;
        fpz->prec = 0;
        fpz->nx = inSize / sizeof(T);
        fpz->ny = 1;
        fpz->nz = 1;
        fpz->nf = 1;
        size_t outSize = fpzip_write(fpz, in);
        fpzip_write_close(fpz);
        return outSize;
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        FPZ* fpz = fpzip_read_from_buffer(in);
        fpz->type = sizeof(T) == 8 ? 1 : 0;
        fpz->prec = 0;
        fpz->nx = inSize / sizeof(T);
        fpz->ny = 1;
        fpz->nz = 1;
        fpz->nf = 1;
        fpzip_read(fpz, out);
        fpzip_read_close(fpz);
    }

    ~FpzipCompressor() override = default;
};

class SpdpCompressor : public Compressor {
public:
    SpdpCompressor()
        : Compressor("spdp", { 0, 9 }) {
    }

    size_t compressBounds(size_t size) {
        return compressed_size_bound(size);
    }

    size_t compress(const char* in, char*& out, size_t inSize, int level) override {
        return spdp_compress(level, inSize, (byte_t*)in, (byte_t*)out);
    }

    void decompress(const char* in, char*& out, size_t inSize, size_t outSize, int level) override {
        spdp_decompress(level, inSize, (byte_t*)in, (byte_t*)out);
    }

    ~SpdpCompressor() override = default;
};