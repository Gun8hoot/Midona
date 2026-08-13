
#include "includes/class/io.hpp"
#include "includes/error.hpp"

#include <fstream>
#include <stdexcept>

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

inline const std::fstream	&IO::getStream(void) const
{
	return (this->_stream);
}

inline const std::string	&IO::getFilepath(void) const
{
	return (this->_filepath);
}

const std::fstream	&IO::openFile(std::string &filepath, std::ios::openmode mode)
{
	if (this->_stream.is_open())
		this->closeFile();
	this->_stream.open(filepath, mode);
	if (!this->_stream.is_open())
		throw (std::runtime_error(EOPEN_FILE + filepath));
	return (_stream);
}

void	IO::closeFile(void) noexcept
{
	if (this->_stream.is_open())
	{
		this->_stream.close();
		this->_filepath = "";
	}
}
