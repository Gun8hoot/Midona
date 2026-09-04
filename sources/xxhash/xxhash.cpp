
#include "includes/xxhash/xxhash.hpp"
#include "includes/helper.hpp"
#include <cstddef>
#include <climits>
#include <cstdint>
#include <iostream>

/*
 * @brief: This  function do a circular shift to the right
 * @src: https://en.wikipedia.org/wiki/Circular_shift
 */
static inline u8_t	circ_shiftl(u32_t value, u32_t shift) noexcept
{
	shift %= 32;
	return (value << shift) | (value >> (32 - shift));
}

static u32_t		create_lane(u8_t *&data)
{
	u32_t	lane = 0;

	for (uint8_t i = 0 ; i < 4; i++)
		lane |= *data++ << (8 * i);
	return (lane);
}

void	xxhash::_init_acc(const u32_t seed, u8_t *&data, u32_t &data_len, u32_t &f_acc) const noexcept
{
	u32_t	acc[4] = {0};

	acc[0] = seed + PRIME32_1 + PRIME32_2;
	acc[1] = seed + PRIME32_2;
	acc[2] = seed + 0;
	acc[3] = seed - PRIME32_1;

	for (std::size_t i = 0; i < 4; i++)
	{
		acc[i] = acc[i] + (create_lane(data) * PRIME32_2);
		acc[i] = circ_shiftl(acc[i], 13);
		acc[i] = acc[i] * PRIME32_1;
		data_len -= 4;
	}
	f_acc = circ_shiftl(acc[0], 1) + circ_shiftl(acc[1], 7) + circ_shiftl(acc[2], 12) + circ_shiftl(acc[3], 18);
}

u32_t	xxhash::xxhash32(const u32_t &seed, u8_t *data, const u32_t &data_len)
{
	u32_t	remaining_bytes = data_len, f_acc = {0}, lane;

	// ACCUMULATOR INITIALIZATION
	if (data_len >= 16)
		this->_init_acc(seed, data, remaining_bytes, f_acc);
	else
		f_acc = seed + PRIME32_5;
	f_acc += data_len;

	// CONSUME BYTES
	while (remaining_bytes >= 4)
	{
		lane = create_lane(data);
		f_acc += lane * PRIME32_3;
		f_acc = circ_shiftl(f_acc, 17) * PRIME32_4;
		remaining_bytes -= 4;
	}
	while (remaining_bytes >= 1)
	{
		lane = create_lane(data);
		f_acc += lane * PRIME32_5;
		f_acc = circ_shiftl(f_acc, 11) * PRIME32_1;
		remaining_bytes -= 1;
	}

	// AVALANCHE
	f_acc ^= f_acc >> 15;
	f_acc *= PRIME32_2;
	f_acc ^= f_acc >> 13;
	f_acc *= PRIME32_3;
	f_acc ^= f_acc >> 16;
	return (f_acc);
}

xxhash::xxhash(void) { ; }

xxhash::~xxhash(void) { ; }
