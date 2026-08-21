
#include "includes/cli.hpp"
#include "includes/error.hpp"
#include "includes/helper.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <csignal>

/*
 * -- CONSTRUCTOR --
 */

cli::cli(int &argc, char **&argv) : _argc(argc), _argv(argv), _flags(0), _infile(""), _outfile("") {;}
cli::~cli(void) { ; }

/*
 * -- FLAGS --
 */

/*
 * @brief: This function is show every parameter available for the program
 */
void cli::help(void) const noexcept
{
	/* https://bettercli.org/design/cli-help-page/ */
	std::cout << "Description:" << std::endl;
	std::cout << "    Midona " << VERSION << ", a runtime compression and encryption packer." << std::endl;
	std::cout << "Usage: " << basename(this->_argv[0]) << " [FLAGS]... [FILE]" << std::endl;
	std::cout << "    -c/--compress : Compress the binary" << std::endl;
	std::cout << "    -e/--encrypt : Encrypt the binary" << std::endl;
	std::cout << "    -b/--both : Compress and encrypt the binary" << std::endl;
	std::cout << "    -o/--out : The location of the compressed/encrypted binary" << std::endl;
	std::cout << "    -p/--progress : A progress bar will be display" << std::endl;
	std::cout << "    -h/--help : Display this message" << std::endl;
	std::cout << "    -v/--version : Display the version of the software" << std::endl;
}
/*
 * @brief: This function display the actual version of the program
 */
void cli::version(void) const noexcept
{
	std::cout << "Midona version: " << VERSION << std::endl; /* NEED TO FIND AN ANOTHER WAY TO CHECK VERSION */
}
/*
 * @brief: This function will called other function to compress the binary using lz4 compression algorithm
 */
void	cli::compress(void) const
{
	std::cout << "Compress binary" << std::endl;
}
/*
 * @brief: This function will called other function to encrypt the binary using AES256 encryption algorithm
 */
void	cli::encrypt(void) const
{
	std::cout << "Encrypt binary" << std::endl;
}
/*
 * @brief: This function check if a flag is enable
 * @param:
 *   1. flagToCheck: The flag we want check (see includes/cli.hpp to have every flags available)
 */
inline bool		cli::isFlagged(int flagToCheck) const
{
	return (this->_flags & flagToCheck ? true : false);
}

/*
 * -- OTHER METHODES --
 */

/*
 * @brief: This function will look every arguments put in argv and
 *         activate their respective flags
 */
void	cli::assignFlag(void)
{
	std::string currentArguments;
	std::string	nextArguments;

	for (int i = 1; i < _argc ; i++)
	{
		currentArguments = helper::trim(_argv[i]);
		if (currentArguments == "-h" || currentArguments == "--help")
		{
			this->_flags |= MIDONA_HELP;
		}
		else if (currentArguments == "-c" || currentArguments == "--compress")
		{
			this->_flags |= MIDONA_COMPRESS;
		}
		else if (currentArguments == "-e" || currentArguments == "--encrypt")
		{
			this->_flags |= MIDONA_ENCRYPT;
		}
		else if (currentArguments == "-b" || currentArguments == "--both")
		{
			this->_flags |= MIDONA_COMPRESS;
			this->_flags |= MIDONA_ENCRYPT;
		}
		else if (currentArguments == "-v" || currentArguments == "--version")
		{
			this->_flags |= MIDONA_VERSION;
		}
		else if (currentArguments == "-p" || currentArguments == "--progress")
		{
			this->_flags |= MIDONA_PROGRESS;
		}
		else if (currentArguments == "-o" || currentArguments == "--out")
		{
			nextArguments = _argv[i + 1];
			if (nextArguments.rfind("-", 0) == 0)
				throw (std::runtime_error(EINVALID_OUTFILE + nextArguments));
			this->_flags |= MIDONA_OUTFILE;
			this->_outfile = helper::expand(helper::trim(nextArguments));
			i++;
		}
		else if (this->_infile == "" && currentArguments.rfind("-", 0) != 0) // If the argument is not a parameter
		{
			helper::expand(currentArguments);
			this->_infile = currentArguments;
		}
		else
			throw (cli::cliError(EUNKNOWN_FLAG + std::string(this->_argv[i])));
	}
	if (this->_outfile.empty())
		this->_outfile = this->_infile
						+ (isFlagged(MIDONA_COMPRESS) ? "_compressed" : "")
						+ (isFlagged(MIDONA_ENCRYPT) ? "_encrypt" : "");
}
/*
 * @brief: This function will check if we have gattered enough data to compress/encrypt data
 */
void	cli::checkArguments(void)
{
	#ifdef DEBUG
	std::cout << "--- DEBUG ---" << std::endl;
	if (!this->_infile.empty()) {std::cout << "INFILE : " << this->_infile << std::endl;}
	if (!this->_outfile.empty()) {std::cout << "OUTFILE : " << this->_outfile << std::endl;}
	if (this->isFlagged(MIDONA_COMPRESS)) {std::cout << "COMPRESSION ACTIVATED" << std::endl;}
	if (this->isFlagged(MIDONA_ENCRYPT)) {std::cout << "ENCRYPTION ACTIVATED" << std::endl;}
	if (this->isFlagged(MIDONA_HELP)) {std::cout << "HELP ACTIVATED" << std::endl;}
	if (this->isFlagged(MIDONA_VERSION)) {std::cout << "VERSION ACTIVATED" << std::endl;}
	if (this->isFlagged(MIDONA_PROGRESS)) {std::cout << "PROGRESS ACTIVATED" << std::endl;}
	std::cout << "--- ---- ---" << std::endl;
	#endif
	if (this->_infile.empty() // Throw a missing infile error if _infile is empty and -h/-v flag are not set
		&& (!this->isFlagged(MIDONA_HELP)
			|| !this->isFlagged(MIDONA_VERSION)))
		throw (cli::cliError(EMISSING_FILEPATH));
	IO::checkFile(this->_infile);
	if (!this->isFlagged(MIDONA_COMPRESS) // Check that we have at least one flag
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
	this->_io.closeFile();
}
