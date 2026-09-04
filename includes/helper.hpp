
#pragma once

#include <inttypes.h>
#include <string>

typedef uint32_t u32_t;
typedef uint8_t u8_t;

namespace helper
{
	std::string	&trim(std::string &str);
	std::string	trim(const char *c_str);
	std::string	&expand(std::string &str);
}
