
#pragma once

#include "includes/types.hpp"

typedef struct s_midona_key
{
	s_midona_key(void) : magic{0}, number_fragments(0), reserved(0), hash(0) {};

	/* An array with magic number content */
	u8_t	magic[8];
	/* The number of the key fragments */
	u32_t	number_fragments;
	u32_t	reserved; // HARD CODED PADDING, IS ALWAYS SET TO 0
	/* The hash of the decrypted key */
	u64_t	hash;
} MidonaKey;
