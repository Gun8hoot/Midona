
#include "includes/class/cli.hpp"

#include <cstring>
#include <exception>
#include <iostream>

/*
 * ARGUMENTS AVAILABLE:
 *   -c/--compress: Compress the binary
 *   -e/--encrypt: Encrypt the binary
 *   -b/-both : Do both
 *   -h/--help : Display an help message to std::cerr
 *   -v/--version: Display the version of the program
 */
int main(int argc, char **argv)
{
	cli	c(argc, argv);

	try
	{
		c.checkArguments();
		c.run();
	}
	catch (cli::cliError &ex)
	{
		std::cerr << basename(argv[0]) << ": " <<  ex.what() << std::endl;
		c.help();
	}
	catch (std::exception &ex)
	{
		std::cerr << basename(argv[0]) << ": " <<  ex.what() << std::endl;
	}
}
