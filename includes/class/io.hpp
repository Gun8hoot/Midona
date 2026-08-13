
#pragma once

#include <fstream>

class	IO
{
	private:
		std::fstream		_stream;
		std::string			_filepath;
		std::ios::openmode	_modes;
	public:
		IO(std::string &filepath, std::ios::openmode mode);
		IO(void) noexcept;
		~IO(void);

		const std::fstream	&getStream(void) const;
		const std::string	&getFilepath(void) const;

		const std::fstream	&openFile(std::string &filepath, std::ios::openmode mode);
		void				closeFile(void) noexcept;
};
