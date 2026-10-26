
#include "includes/Cli.hpp"

#include <cstring>
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	Cli	cli(argc, argv);

	try
	{
		cli.assignFlag();
		cli.checkArguments();
		cli.run();
	}
	catch (Cli::cliError &ex)
	{
		std::cerr << basename(argv[0]) << " : " <<  ex.what() << std::endl
				  << "Use --help to get more information." << std::endl;
		return (1);
	}
	catch (std::exception &ex)
	{
		std::cerr << "\x1b[31m[!] " << basename(argv[0]) << " : " <<  ex.what() << "\x1b[0m" <<  std::endl;
		return (1);
	}
	return (0);
}
