
#include "includes/class/cli.hpp"
#include "includes/class/packer.hpp"
#include "includes/error.hpp"
#include "includes/helper.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

/* -- CONSTRUCTOR -- */
cli::cli(int &argc, char **&argv) : _argc(argc), _argv(argv), _flags(0), _filepath("") { ; }
cli::~cli(void) { ; }

/* -- FLAGS -- */

/*
 * @brief: This function is called when we want to show how to use the program
 */
void cli::help(void) const noexcept
{
	/* https://bettercli.org/design/cli-help-page/ */
	std::cerr << "Description:" << std::endl;
	std::cerr << "    Midona, a runtime compression and encryption packer." << std::endl;
	std::cerr << "Usage: " << basename(this->_argv[0]) << " [FLAGS]... [FILE]" << std::endl;
	std::cerr << "    -c/--compress : Compress the binary" << std::endl;
	std::cerr << "    -e/--encrypt : Encrypt the binary" << std::endl;
	std::cerr << "    -b/--both : Compress and encrypt the binary" << std::endl;
	std::cerr << "    -h/--help : Display this message" << std::endl;
	std::cerr << "    -v/--version : Display the version of the software" << std::endl;
}
/*
 * @brief: This function is called when we want to show the version of the program
 */
void cli::version(void) const noexcept
{
	std::cout << "Midona version: 1.0.0" << std::endl; /* NEED TO FIND AN ANOTHER WAY TO CHECK VERSION */
}
/*
 * @brief: This function is called when we want to compress a binary
 */
void	cli::compress(void) const
{
	std::cout << "Compress binary" << std::endl;
}
/*
 * @brief: This function is called when we want encrypt a binary
 */
void	cli::encrypt(void) const
{
	std::cout << "Encrypt binary" << std::endl;
}
inline bool		cli::isFlagged(int flagToCheck) const
{
	return (this->_flags & flagToCheck ? true : false);
}

/* -- OTHER METHODES -- */

void	cli::sortArguments(void)
{
	std::string tmp;

	for (int i = 1; i < _argc ; i++)
	{
		tmp = helper::trim(_argv[i]);
		if (tmp == "-h" || tmp == "--help")
		{
			this->_flags |= MIDONA_HELP;
		}
		else if (tmp == "-c" || tmp == "--compress")
		{
			this->_flags |= MIDONA_COMPRESS;
		}
		else if (tmp == "-e" || tmp == "--encrypt")
		{
			this->_flags |= MIDONA_ENCRYPT;
		}
		else if (tmp == "-b" || tmp == "--both")
		{
			this->_flags |= MIDONA_COMPRESS;
			this->_flags |= MIDONA_ENCRYPT;
		}
		else if (tmp == "-v" || tmp == "--version")
		{
			this->_flags |= MIDONA_VERSION;
		}
		else if (this->_filepath == "" && tmp.rfind("-") != 0)
		{
			helper::expand(tmp);
			this->_filepath = tmp;
		}
		else
			throw (cli::cliError(EUNKNOWN_FLAG + std::string(this->_argv[i])));
	}
}
/*
 * @brief: This function is used to
 */
void	cli::checkArguments(void)
{
	this->sortArguments();
	if (this->_filepath == "")
		throw (cli::cliError(EMISSING_FILEPATH));
	else if (access(this->_filepath.c_str(), F_OK) == -1) // TODO : BETTER FILE CHECK
		throw (std::runtime_error("The file \"" + this->_filepath + "\" doesnt exist."));
	else if (!this->isFlagged(MIDONA_COMPRESS)
			&& !this->isFlagged(MIDONA_ENCRYPT)
			&& !this->isFlagged(MIDONA_VERSION)
			&& !this->isFlagged(MIDONA_HELP))
		throw (cli::cliError(EMISSING_ACTION));
}
/*
 * @brief: This function is called when we want run the program with flag gettered before
 */
void	cli::run(void)
{
	if (this->isFlagged(MIDONA_VERSION))
		return (cli::version());
	if (this->isFlagged(MIDONA_HELP) || (!this->isFlagged(MIDONA_COMPRESS) && !this->isFlagged(MIDONA_ENCRYPT)))
		return (cli::help());
	if (this->isFlagged(MIDONA_COMPRESS))
		cli::compress();
	if (this->isFlagged(MIDONA_ENCRYPT))
		cli::encrypt();
}
