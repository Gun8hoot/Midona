
#include "includes/cli.hpp"

#include <cstring>
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	cli	c(argc, argv);

	try
	{
		c.assignFlag();
		c.checkArguments();
		c.run();
	}
	catch (cli::cliError &ex)
	{
		std::cerr << basename(argv[0]) << ": " <<  ex.what() << std::endl
				  << argv[0] << " --help to get more information." << std::endl;
	}
	catch (std::exception &ex)
	{
		std::cerr << basename(argv[0]) << ": " <<  ex.what() << std::endl;
	}
}
