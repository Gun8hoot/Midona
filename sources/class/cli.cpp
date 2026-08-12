
#include "includes/class/cli.hpp"

cli::cli(int &argc, char **&argv) : _argc(argc), _argv(argv)  {;}

cli::~cli(void) {;}

void cli::help(void) const noexcept
{
	/* https://bettercli.org/design/cli-help-page/ */
	std::cerr << "Usage: " << basename(this->_argv[0]) << " [FLAGS]... [FILE]" << std::endl;
}

void cli::version(void) const noexcept
{
	std::cout << "Version: 1.0.0" << std::endl; /* NEED TO FIND AN ANOTHER WAY TO CHECK VERSION */
}

void cli::checkArguments(void) const
{
	if (this->_argc == 1)
		this->help();
}
