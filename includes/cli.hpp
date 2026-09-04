
#pragma once

#include "includes/io/io.hpp"

#include <exception>
#include <iostream>
#include <cstring>
#include <inttypes.h>

#define VERSION "1.0.0"

enum	cliFlags
{
	MIDONA_VERSION = 1 << 0,
	MIDONA_HELP = 1 << 1,
	MIDONA_COMPRESS = 1 << 2,
	MIDONA_ENCRYPT = 1 << 3,
	MIDONA_PROGRESS = 1 << 4,
	MIDONA_OUTFILE = 1 << 5,
	MIDONA_DECOMPRESS = 1 << 6,
	MIDONA_DECRYPT = 1 << 7,
};

class	cli
{
	private:
		int			&_argc;
		char		**&_argv;
		uint8_t		_flags;
		std::string	_infile;
		std::string	_outfile;
		IO			_io;

		void		version(void) const noexcept;
		void		compress(void) const;
		void		encrypt(void) const;
		bool		checkActiveFlag(int flagToCheck) const;
		bool		checkValidAction(void);
		bool		checkValidInfile(void);

	public:
		cli(int &argc, char **&argv);
		~cli(void);
		class cliError : public std::exception
		{
			private:
				std::string _msg;
			public:
				cliError(std::string msg) : _msg(msg) {;}
				const char	*what(void) const noexcept
				{
					return (_msg.c_str());
				}
		};

		void		help(void) const noexcept;
		void		checkArguments(void);
		void		assignFlag(void);
		void		run(void);
};
