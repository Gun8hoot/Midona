
#include "includes/Elf.hpp"
#include <linux/elf.h>
#include <new>
#include <stdexcept>

Elf::Elf(void) : _stream(NULL), _validElf(false), _opened(false), _header{}, _error(Elf_error::None) { ; }
Elf::Elf(Elf &cpy) : _stream(cpy._stream), _validElf(cpy._validElf), _opened(cpy._opened), _header(cpy._header), _error(cpy._error) { ; }
Elf::Elf(std::string &filename) : _stream(NULL), _validElf(false), _opened(false)
{
	this->open(filename);
}
Elf::~Elf(void)
{
	this->close();
}

void	Elf::_parseElfHeader(void)
{
	try
	{
		/* Read the header */
		this->_stream->read(reinterpret_cast<char *>(&this->_header), sizeof(this->_header));
		if (this->_stream->gcount() != sizeof(this->_header))
			throw std::runtime_error("ReadFailed");
		/* Check ELF magic number */
		if (this->_header.e_ident[0] != ELFMAG0 ||
				this->_header.e_ident[1] != ELFMAG1 ||
				this->_header.e_ident[2] != ELFMAG2 ||
				this->_header.e_ident[3] != ELFMAG3)
			throw std::runtime_error("NotELF");
		/* Check if the file is 64bits */
		if (this->_header.e_ident[4] != ELFCLASS64)
			throw std::runtime_error("Unsupported");
		this->_validElf = true;

	}
	catch (std::runtime_error &ex)
	{
		if (std::strcmp(ex.what(), "ReadFailed") == 0)
			this->_error = Elf_error::ReadFailed;
		else if (std::strcmp(ex.what(), "NotELF") == 0)
			this->_error = Elf_error::NotELF;
		else if (std::strcmp(ex.what(), "Unsupported") == 0)
			this->_error = Elf_error::Unsupported;
	}
	this->_stream->seekg(0, std::ios::beg);
	this->_stream->flush();
}

bool	Elf::open(std::string &filename)
{
	this->_error = Elf_error::None;

	if (isOpen())
		this->_error = Elf_error::AlreadyOpen;
	else
	{
		try
		{
			this->_stream = new std::fstream(filename, std::ios::in | std::ios::in | std::ios::binary);
			if (!this->_stream->is_open())
				throw (std::runtime_error("FailedOpen"));
			this->_parseElfHeader();
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

void	Elf::close(void)
{
	if (this->_opened)
	{
		this->_stream->close();
		delete this->_stream;
	}
}

bool		Elf::isOpen(void) const
{
	return (this->_opened);
}

Elf64_Ehdr	Elf::getHeader(void) const
{
	return (this->_header);
}
