
#include "includes/class/cli.hpp"

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

	c.checkArguments();
}
