
#pragma once

# include "includes/helper.hpp"
# include <cstdint>
# include <fstream>
# include <string>
# include <vector>

# define LZ4_MAGICNUMBER 0x184D2204

# define SEED 0

class	lz4
{
	private:
		const	std::string		&_filepath;
		const	std::fstream	&_stream;
	public:
		lz4(void);
		lz4(const std::string &filepath, const std::fstream &stream);
		~lz4(void);

		const std::string	&getFilepath(void);
		const std::fstream	&getStream(void);


		void	compress(void);
		void	decompress(void);
};
