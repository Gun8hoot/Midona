
#include "includes/lz4/lz4.hpp"
#include <climits>

/*
 * @brief: This  function do a circular shift to the right
 * @src: https://en.wikipedia.org/wiki/Circular_shift
 */
static u8_t	circ_shiftl(u32_t value, u32_t count)
{
	u32_t mask = CHAR_BIT * sizeof(value) - 1;
	count &= mask;
	return (value << count) | (value << -count & mask);
}

u32_t lz4::xxh32(u32_t toHash)
{
	u32_t	hashed, acc1, acc2, acc3, acc4, acc;

	if (toHash < 0xFFFF)
	{
		acc1 = ((SEED + PRIME32_1 % 32) + PRIME32_2 % 32);
		acc2 = SEED + PRIME32_2 % 32;
		acc3 = SEED + 0 % 32;
		acc4 = SEED - PRIME32_1 % 32;
	}
	else
		acc = SEED + PRIME32_5;
}

// #include "includes/class/compression.hpp"
// #include "includes/error.hpp"
// #include <stdexcept>

// lz4::lz4(const std::string &filepath, const std::fstream &stream) : _filepath(filepath), _stream(stream) {;};

// lz4::~lz4(void) {;}

// inline const std::string	&lz4::getFilepath(void) { return (this->_filepath); }

// inline const std::fstream	&lz4::getStream(void) { return (this->_stream); }

// // https://shubham-tomar.github.io/myblog/posts/lz4/
// void	lz4::compress(void)
// {
// 	if (this->_stream.is_open())
// 		throw (std::runtime_error(EFILE_CLOSE_TOO_SOON));
// }
