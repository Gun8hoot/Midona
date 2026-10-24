
#include "includes/ELF/ElfReader.hpp"
#include "includes/types.hpp"
#include <exception>
#include <linux/elf.h>
#include <new>
#include <stdexcept>
#include <vector>

/*
 * ==========================
 * = CONSTRUCTOR/DESTRUCTOR =
 * ==========================
 */

ElfReader::ElfReader(void) : _filepath(""), _stream(NULL), _validElf(false), _opened(false), _error(Elf_error::None), _header{}, _sectionHeader(), _programHeader(), _sectionContent(NULL) { ; }
ElfReader::ElfReader(ElfReader &cpy) : _filepath(cpy._filepath), _stream(NULL), _validElf(false), _opened(false), _error(Elf_error::None), _header{}, _sectionHeader(), _programHeader(), _sectionContent(NULL)
{
	this->open(cpy._filepath);
}
ElfReader::ElfReader(std::string &filepath) : _filepath(filepath), _stream(NULL), _validElf(false), _opened(false), _error(Elf_error::None), _header{}, _sectionHeader(), _programHeader(), _sectionContent(NULL)
{
	this->open(filepath);
}
ElfReader	&ElfReader::operator=(ElfReader &cpy)
{
	if (&cpy != this)
	{
		if (this->isOpen())
			this->close();
		this->_filepath = cpy._filepath;
		this->open(this->_filepath);
	}
	return (*this);
}
ElfReader::~ElfReader(void)
{
	this->close();
}

/*
 * =======
 * = I/O =
 * =======
 */

/*
 * @brief: Will try to open the file and assign the correct error state
 * if the file cannot be properly open/read
 */
bool	ElfReader::open(std::string &filepath)
{
	this->_error = Elf_error::None;

	if (isOpen())
		this->_error = Elf_error::AlreadyOpen;
	else
	{
		try
		{
			this->_stream = new std::ifstream(filepath, std::ios::in | std::ios::binary);
			if (!this->_stream->is_open())
				throw (std::runtime_error("FailedOpen"));
			this->_parseElf();
			this->_opened = true;
		}
		catch (std::bad_alloc &ex)
		{
			this->_error = Elf_error::FailedAlloc;
		}
		catch (std::runtime_error &ex)
		{
			this->_error = Elf_error::FailedOpen;
		}
	}
	return (this->_error == Elf_error::None);
}
/*
 * @brief: Properly close the opened file
 */
void	ElfReader::close(void)
{
	if (this->_opened)
	{
		this->_stream->close();
		delete this->_stream;
	}
}
/*
 * @brief: Check if the file is currently openned
 */
bool	ElfReader::isOpen(void) const
{
	return (this->_opened);
}

/*
 * ==========
 * = GETTER =
 * ==========
 */

/*
 * @brief: This function (getter) will return the path of the opened
 * file.
 */
std::string	ElfReader::getFilepath(void) const
{
	return (this->_filepath);
}
/*
 * @brief: This function (getter) will return a Elf64_Ehdr struct with
 * every information who contain the ELF header.
 */
Elf64_Ehdr	ElfReader::getElfHeader(void) const
{
	return (this->_header);
}
/*
 * @brief: This function (getter) will return a vector of Elf64_Shdr
 * struct with every information of every section who compose the ELF file
 */
std::vector<Elf64_Shdr>	ElfReader::getSectionHeader(void) const
{
	return (this->_sectionHeader);
}
/*
 * @brief: Th
 */
std::vector<Elf64_Phdr>	ElfReader::getProgramHeader(void) const
{
	return (this->_programHeader);
}
std::vector<u8_t>		*ElfReader::getSectionData(Elf64_Shdr hdr)
{
	std::vector<u8_t>	*data;

	try
	{
		data = new std::vector<u8_t>(hdr.sh_size);
	}
	catch (std::bad_alloc &ex)
	{
		return (nullptr);
	}
	this->_stream->seekg(hdr.sh_offset, std::ios::beg);
	this->_stream->read(reinterpret_cast<char *>(data->data()), hdr.sh_size);
	if (this->_stream->gcount() == 0)
	{
		delete data;
		return (nullptr);
	}
	return (data);
}

/*
 * ================================
 * = PARSE ELF HEADER INFORMATION =
 * ================================
 */

bool	ElfReader::_parseElf(void)
{
	try
	{
		/* ELF section parsing */
		this->_parseElfHeader();
		this->_parseElfSectionHeader();
		this->_parseElfProgramHeader();
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return (false);
	}
	return (true);
}

void	ElfReader::_parseElfHeader(void)
{
	/* Read the header */
	this->_stream->read(reinterpret_cast<char *>(&this->_header), sizeof(this->_header));
	if (this->_stream->gcount() != sizeof(this->_header))
		throw std::runtime_error(ERR_ELF_READ);
	/* Check ELF magic number */
	if (this->_header.e_ident[0] != ELFMAG0 ||
			this->_header.e_ident[1] != ELFMAG1 ||
			this->_header.e_ident[2] != ELFMAG2 ||
			this->_header.e_ident[3] != ELFMAG3 ||
			this->_header.e_ident[4] != ELFCLASS64)
		throw std::runtime_error(ERR_ELF_INVAL);
}

void	ElfReader::_parseElfSectionHeader(void)
{
	Elf64_Shdr	current_header;

	this->_stream->seekg(this->_header.e_shoff, std::ios::beg);

	for (std::size_t section_readed = 0 ; section_readed < this->_header.e_shnum - 1; section_readed++)
	{
		this->_stream->read(reinterpret_cast<char *>(&current_header), sizeof(Elf64_Shdr));
		if (this->_stream->gcount() == 0 || current_header.sh_entsize == sizeof(Elf64_Shdr))
			throw (std::runtime_error(ERR_ELF_READ));
		this->_sectionHeader.push_back(current_header);
	}
}

void	ElfReader::_parseElfProgramHeader(void)
{
	Elf64_Phdr	current_header;

	this->_stream->seekg(this->_header.e_phoff, std::ios::beg);
	for (std::size_t i = 0; i < this->_header.e_phnum - 1 ; i++)
	{
		this->_stream->read(reinterpret_cast<char *>(&current_header), sizeof(Elf64_Phdr));
		if (this->_stream->gcount() == 0)
			throw (std::runtime_error(ERR_ELF_READ));
		this->_programHeader.push_back(current_header);
	}
}
