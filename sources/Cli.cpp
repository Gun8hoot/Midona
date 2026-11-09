
#include "includes/Cli.hpp"
#include "includes/ELF/ElfBuilder.hpp"
#include "includes/ELF/ElfReader.hpp"
#include "includes/ELF/MidonaStruct.hpp"
#include "includes/error.hpp"
#include "includes/helper.hpp"
#include "includes/midona.hpp"
#include "includes/types.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <csignal>
#include <vector>

#include <vector>
#include <cstdint>
#include <cstring>

std::vector<uint8_t>    make_hello_stub(void)
{
    static const uint8_t bytes[] = {
        0x52,                                            // push rdx
        0x57,                                            // push rdi
        0x56,                                            // push rsi
        0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,        // mov rax, 1  (SYS_write)
        0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00,        // mov rdi, 1  (stdout)
        0x48, 0x8D, 0x35, 0x11, 0x00, 0x00, 0x00,        // lea rsi, [rip+0x11] -> msg @ +41
        0x48, 0xC7, 0xC2, 0x17, 0x00, 0x00, 0x00,        // mov rdx, 23
        0x0F, 0x05,                                      // syscall
        0x5E,                                            // pop rsi
        0x5F,                                            // pop rdi
        0x5A,                                            // pop rdx
        0xE9, 0x00, 0x00, 0x00, 0x00,                    // jmp rel32  (patched)
        0x68,0x65,0x6C,0x6C,0x6F,0x20,                   // "hello "
        0x66,0x72,0x6F,0x6D,0x20,                        // "from "
        0x65,0x6E,0x74,0x72,0x79,0x20,                   // "entry "
        0x70,0x6F,0x69,0x6E,0x74,0x0A                    // "point\n"
    };
    return std::vector<uint8_t>(bytes, bytes + sizeof(bytes));
}

constexpr std::size_t JMP_REL32_OFFSET = 36;   // offset of the E9 inside the stub

/*
 * ==========================
 * = CONSTRUCTOR/DESTRUCTOR =
 * ==========================
 */

Cli::Cli(int &argc, char **&argv) : _argc(argc), _argv(argv), _flags(0), _infile(""), _outfile(""), _elf_reader(nullptr), _elf_builder(nullptr) {;}
Cli::~Cli(void)
{
	if (this->_elf_reader != nullptr)
		delete this->_elf_reader;
	if (this->_elf_builder != nullptr)
		delete this->_elf_builder;
}

/*
 * =========
 * = FLAGS =
 * =========
 */

/*
 * @brief: This function is show the help message who explain what arguments should
 * the user provided to use the CLI
 */
void Cli::help(void) const noexcept
{
	/* https://bettercli.org/design/cli-help-page/ */
	std::cout	<< "Description:" << std::endl
				<< "    Midona " << VERSION << ", a runtime compression and encryption packer." << std::endl
				<< "Usage: " << basename(this->_argv[0]) << " [FLAGS]... [FILE]" << std::endl
				<< "    -c/--compress : Compress the binary" << std::endl
				<< "    -e/--encrypt : Encrypt the binary" << std::endl
				<< "    -b/--both : Compress and encrypt the binary" << std::endl
				<< "    -o/--out : The location of the compressed/encrypted binary" << std::endl
				<< "    -s/--silent : Dont display any information during the program execution" << std::endl
				<< "    -h/--help : Display this message" << std::endl
				<< "    -v/--version : Display the version of the software" << std::endl;
}
/*
 * @brief: This function display the version of the program
 */
void Cli::version(void) const noexcept
{
	std::cout << "Midona version: " << VERSION << std::endl;
}
/*
 * @brief: Wrapper who do the compression of some section
 */
void	Cli::compress(void) const
{
	std::cout << "Compress binary" << std::endl;
}
/*
 * @brief: Wrapper who do the encryption of some section
 */
void	Cli::encrypt(void) const
{
	std::cout << "Encrypt binary" << std::endl;
}
/*
 * @brief: This function check if a flag is enable
 * @param flagToCheck: The flag we want check if he is active
 */
inline bool		Cli::checkActiveFlag(int flagToCheck) const
{
	return (this->_flags & flagToCheck ? true : false);
}

/*
 * @brief: This function will set correct flag bits using given arguments in argv
 */
void	Cli::assignFlag(void)
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
		else if (currentArguments == "-s" || currentArguments == "--silent")
		{
			this->_flags |= MIDONA_SILENT;
		}
		else if (currentArguments == "-o" || currentArguments == "--out")
		{
			if (!_argv[i + 1])
				throw (MidonaRuntimeError("Missing output file"));
			nextArguments = _argv[i + 1];
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
			throw (Cli::cliError(EUNKNOWN_FLAG + std::string(this->_argv[i])));
	}
	if (this->_outfile.empty())
		this->_outfile =
			this->_infile + (checkActiveFlag(MIDONA_COMPRESS) || checkActiveFlag(MIDONA_ENCRYPT) ? "_packed" : "");
}
/*
 * @brief: This function will check if we have gattered enough data to compress/encrypt data
 */
void	Cli::checkArguments(void)
{
	if (this->_infile.empty() // Throw a missing infile error if _infile is empty and -h/-v flag are not set
			&& !this->checkActiveFlag(MIDONA_HELP)
			&& !this->checkActiveFlag(MIDONA_VERSION))
		throw (Cli::cliError(EMISSING_FILEPATH));
	else if (!this->_infile.empty() && (this->_infile == this->_outfile) && !this->checkActiveFlag(MIDONA_SILENT))
	{
		std::string usr;
		std::cout << "\x1b[33m[?] The input and output file are the same, are you sure to continue ? (Y/n)\x1b[0m : ";
		std::cin >> usr;
		if (usr == "N" || usr == "n")
			throw (MidonaRuntimeError("Abording ..."));
	}
	if (!this->_infile.empty())
	{
		if (!this->checkActiveFlag(MIDONA_COMPRESS) // Check that we have at least one flag
				&& !this->checkActiveFlag(MIDONA_ENCRYPT)
				&& !this->checkActiveFlag(MIDONA_VERSION)
				&& !this->checkActiveFlag(MIDONA_HELP))
			throw (Cli::cliError(EMISSING_ACTION));
	}
}

/*
 * @brief: This function is called when we want run the program with flag gettered before
 */
void	Cli::run(void)
{
	if (this->checkActiveFlag(MIDONA_VERSION))
		return (Cli::version());
	if (this->checkActiveFlag(MIDONA_HELP) || (!this->checkActiveFlag(MIDONA_COMPRESS) && !this->checkActiveFlag(MIDONA_ENCRYPT)))
		return (Cli::help());

	this->_elf_reader = new (std::nothrow) ElfReader(this->_infile, this->checkActiveFlag(MIDONA_SILENT));
	if (this->_elf_reader == nullptr)
		throw (MidonaRuntimeError(EALLOC_ERROR, "ElfReader class"));

	this->_elf_builder = new (std::nothrow) ElfBuilder(*this->_elf_reader);
	if (this->_elf_builder == nullptr)
		throw (MidonaRuntimeError(EALLOC_ERROR, "ElfBuilder class"));

	Elf64_Shdr packed = {};
	packed.sh_type      = SHT_PROGBITS;
	packed.sh_flags     = SHF_ALLOC | SHF_EXECINSTR;
	packed.sh_addralign = 16;
	packed.sh_name      = 0;

	std::vector<u8_t> data = make_hello_stub();
	std::size_t idx = this->_elf_builder->getSectionData().size();  // = 0 if builder is fresh
	this->_elf_builder->addSection(packed, data);

	this->_elf_builder->setEntryPointToSection(idx, /*offset_in_section=*/0,
                                   /*jmp_rel32_offset=*/JMP_REL32_OFFSET);

	this->_elf_builder->createFile(this->_outfile);

	if (this->checkActiveFlag(MIDONA_COMPRESS))
		this->compress();
	if (this->checkActiveFlag(MIDONA_ENCRYPT))
		this->encrypt();
	if (!this->checkActiveFlag(MIDONA_SILENT))
		std::cout << "\x1b[32m[!] Successfully packed " << this->_outfile << " (" << this->_elf_reader->getFilesize() << "/" << this->_elf_builder->getFilesize() <<  " bytes)\x1b[0m" << std::endl;
}
