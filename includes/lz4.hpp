#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;

/* ---- LZ4 block-format constants ---- */
static const int  LZ4_MINMATCH      = 4;
static const int  LZ4_LASTLITERALS  = 5;
static const int  LZ4_MFLIMIT       = 12;
static const int  LZ4_ML_BITS       = 4;
static const int  LZ4_ML_MASK       = (1 << LZ4_ML_BITS) - 1;       /* 15  */
static const int  LZ4_RUN_MASK      = (1 << (8 - LZ4_ML_BITS)) - 1; /* 15  */
static const int  LZ4_HASHLOG       = 12;
static const int  LZ4_HASHTABLESIZE = 1 << LZ4_HASHLOG;
static const int  LZ4_MAX_DISTANCE  = 65535;

/* Frame magic — only needed if you later wrap this in a .lz4 frame */
static const u32_t LZ4_MAGICNUMBER  = 0x184D2204U;

class lz4
{
public:
    lz4()  = default;
    ~lz4() = default;

    /* Compress srcSize bytes from src into dst.
       Returns bytes written, or -1 if dstCapacity is too small. */
    static int compressBlock(const u8_t* src, int srcSize,
                             u8_t* dst, int dstCapacity);

    /* Decompress srcSize bytes from src into dst.
       Returns bytes written, or -1 on malformed input. */
    static int decompressBlock(const u8_t* src, int srcSize,
                               u8_t* dst, int dstCapacity);

    /* Convenience wrappers around std::vector. */
    static std::vector<u8_t> compress(const std::vector<u8_t>& in);

    /* You must know the decompressed size ahead of time — the block format
       does not store it (that is what the frame format is for). */
    static std::vector<u8_t> decompress(const std::vector<u8_t>& in,
                                        std::size_t uncompressedSize);
};
