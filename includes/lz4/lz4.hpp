
#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <inttypes.h>

# define LZ4_MAGICNUMBER 0x184D2204

typedef uint32_t u32_t;
typedef uint8_t u8_t;

const u32_t PRIME32_1 = 0x9E3779B1U;  // 0b10011110001101110111100110110001
const u32_t PRIME32_2 = 0x85EBCA77U;  // 0b10000101111010111100101001110111
const u32_t PRIME32_3 = 0xC2B2AE3DU;  // 0b11000010101100101010111000111101
const u32_t PRIME32_4 = 0x27D4EB2FU;  // 0b00100111110101001110101100101111
const u32_t PRIME32_5 = 0x165667B1U;  // 0b00010110010101100110011110110001

# define SEED 0


class	lz4
{
	private:
		const	std::string		&_filepath;
		const	std::fstream	&_stream;
	public:
		lz4(void);
		lz4(const std::string &filepath, const std::fstream &stream);
		~lz4(void);

		const std::string	&getFilepath(void);
		const std::fstream	&getStream(void);


		void	compress(void);
		void	decompress(void);
		static u32_t xxh32(u32_t toHash);
};
