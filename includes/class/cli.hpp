
#pragma once

#include <exception>
#include <iostream>
#include <cstring>

class	cli
{
	private:
		int		&_argc;
		char	**&_argv;

		void	help(void) const noexcept;
		void	version(void) const noexcept;
	public:
		cli(int &argc, char **&argv);
		~cli(void);

		void	checkArguments(void) const;
};
