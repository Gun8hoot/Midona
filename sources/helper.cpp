
#include "includes/helper.hpp"
#include "includes/error.hpp"
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

/*
 * @brief: This function remove white space at the beginning and the end of the string
 * @param:
 *   str = Reference to the string we want trim
 */
std::string	&helper::trim(std::string &str)
{
	std::size_t	size = str.size();
	std::size_t first = 0;
	std::size_t last = size;

	while (first < last && std::isspace(static_cast<unsigned char>(str[first])))
		++first;
	while (last > first && std::isspace(static_cast<unsigned char>(str[last - 1])))
		--last;

	if (last != size)
		str.erase(last, size);
	if (first != 0)
		str.erase(0, first);
	return (str);
}
/*
 * @brief: This function remove white space at the beginning and the end of the string
 * @param:
 *   str = The string we want trim
 */
std::string	helper::trim(const char *c_str)
{
	std::string str;
	std::size_t	size;
	std::size_t first = 0;
	std::size_t last;

	if (!c_str)
		return ("");

	str = c_str;
	size = str.size();
	last = size;

	while (first < last && std::isspace(static_cast<unsigned char>(str[first])))
		++first;
	while (last > first && std::isspace(static_cast<unsigned char>(str[last - 1])))
		--last;

	if (last != size)
		str.erase(last, size);
	if (first != 0)
		str.erase(0, first);
	return (str);
}
/*
 * @brief: This function give the username of the user by using his uid
 */
static	const std::string	get_username(void)
{
	uid_t	uid = getuid();
	passwd	*data = getpwuid(uid);

	if (!data || !data->pw_name)
		throw (std::runtime_error(EUSERNAME));
	return (std::string(data->pw_name));
}
/*
 * @brief: This function expand the tilde (~) operator on a file path
 */
std::string	&helper::expand(std::string &str)
{
	const std::string username = get_username();

	if (str.rfind("~/", 0) == 0)
	{
		str.erase(0, 1);
		str = "/home/" + username + str;
	}
	return (str);
}
