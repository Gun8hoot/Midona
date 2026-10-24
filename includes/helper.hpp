
#pragma once

#include "includes/types.hpp"
#include <string>

namespace helper
{
	std::string	&trim(std::string &str);
	std::string	trim(const char *c_str);
	std::string	&expand(std::string &str);
	char	*genPassword(std::size_t size);
}
