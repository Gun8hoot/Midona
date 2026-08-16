
#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <inttypes.h>

# define LZ4_MAGICNUMBER 0x184D2204

class	lz4
{
	private:
		const	std::string		&_filepath;
		const	std::fstream	&_stream;
	public:
		lz4(const std::string &filepath, const std::fstream &stream);
		~lz4(void);

		const std::string	&getFilepath(void);
		const std::fstream	&getStream(void);

		template<typename T>
		class	hashmap
		{
			private :
				std::vector<T> _vector;
				uint16_t		computeHash(const uint8_t	*ptr);
			public:
				hashmap(void);
				~hashmap(void);

				add()

		};
		void	compress(void);
		void	decompress(void);
}
