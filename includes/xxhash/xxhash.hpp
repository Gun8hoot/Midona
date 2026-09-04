
# pragma once

# include "includes/helper.hpp"
# include <cstdint>
# include <vector>

/* -- PRIMES NUMBERS USED -- */
const u32_t PRIME32_1 = 0x9E3779B1U;  // 0b10011110001101110111100110110001
const u32_t PRIME32_2 = 0x85EBCA77U;  // 0b10000101111010111100101001110111
const u32_t PRIME32_3 = 0xC2B2AE3DU;  // 0b11000010101100101010111000111101
const u32_t PRIME32_4 = 0x27D4EB2FU;  // 0b00100111110101001110101100101111
const u32_t PRIME32_5 = 0x165667B1U;  // 0b00010110010101100110011110110001

class xxhash
{
	private:
		void				_init_acc(const u32_t seed, u8_t *&data, u32_t &data_len, u32_t &f_acc) const noexcept;
	public:
		xxhash(void);
		~xxhash(void);

		u32_t		xxhash32(const u32_t &seed, u8_t *data, const u32_t &data_len);
};
