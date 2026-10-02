
#include "includes/io/io.hpp"
#include "includes/error.hpp"

#include <fstream>
#include <sys/stat.h>
#include <stdexcept>
#include <unistd.h>

IO::IO(std::string &filepath, std::ios::openmode mode)
{
	this->_stream.open(filepath, mode);
	if (!this->_stream.is_open())
		throw (std::runtime_error(EOPEN_FILE + filepath));
}

IO::IO(void) noexcept {;}

IO::~IO(void)
{
	if (this->_stream.is_open())
	{
		this->_stream.close();
		this->_filepath = "";
	}
}

/*
 * @brief: This function return a constant reference of the stream who is currently openned
 */
inline std::fstream	&IO::getStream(void) const
{
	return (this->_stream);
}
/*
 * @brief: This function return a constant reference of the file path who is currently openned
 */
inline const std::string	&IO::getFilepath(void) const
{
	return (this->_filepath);
}
/*
 * @brief: This function open a new file and return a reference to his stream
 * @param:
 *   filepath: The path of the file we want open
 *   mode: std::ios flags who define how we should open the file
 */
const std::fstream	&IO::openFile(std::string &filepath, std::ios::openmode mode)
{
	if (this->_stream.is_open())
		this->closeFile();
	this->_stream.open(filepath, mode);
	if (!this->_stream.is_open())
		throw (std::runtime_error(EOPEN_FILE + filepath));
	this->_modes = mode;
	return (_stream);
}
/*
 * @brief: This function close the currently openned file
 */
void	IO::closeFile(void) noexcept
{
	if (this->_stream.is_open())
	{
		this->_stream.close();
		this->_filepath = "";
	}
}
/*
 * @brief: This function check if the file exist, he is a regular file and if we have read permission on it
 * @param:
 *   str = The path of the file we want check
 */
bool	IO::checkFile(const std::string &str)
{
	struct stat	st;

	if (access(str.c_str(), F_OK) != 0)
		throw (std::runtime_error(EDONT_EXIST));
	if (lstat(str.c_str(), &st) != 0)
		throw (std::runtime_error(ELSTAT_FAILED + str));
	if ((st.st_mode & S_IFMT) != S_IFREG)
		throw (std::runtime_error(EREG_FILE));
	if (!(st.st_mode & S_IREAD))
		throw (std::runtime_error(EREAD_ERROR));
	return (true);
}
