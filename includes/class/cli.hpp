
#pragma once

#include "includes/class/io.hpp"


#include <exception>
#include <iostream>
#include <cstring>

#define VERSION "1.0.0"

enum	cliFlags
{
	MIDONA_VERSION = 1 << 0,
	MIDONA_HELP = 1 << 1,
	MIDONA_COMPRESS = 1 << 2,
	MIDONA_ENCRYPT = 1 << 3,
};

class	cli
{
	private:
		int			&_argc;
		char		**&_argv;
		int			_flags;
		std::string	_filepath;
		IO			_io;

		void		version(void) const noexcept;
		void		compress(void) const;
		void		encrypt(void) const;
		bool		isFlagged(int flagToCheck) const;

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
		void		sortArguments(void);
		void		run(void);
};
