#include "includes/ELF/ElfReader.hpp"
#include "includes/error.hpp"
#include "includes/midona.hpp"
#include <exception>

void    ElfReader::open(std::string &filepath)
{
	if (isOpen())
		this->close();

	this->_filepath = filepath;

	/* --- FILE PREPARATION --- */
	this->_stream = new (std::nothrow) std::ifstream(filepath, std::ios::in | std::ios::binary);
	if (this->_stream == nullptr)
		throw (MidonaRuntimeError(EALLOC_ERROR, "ElfRead file stream"));
	if (!this->_stream->is_open())
		throw (MidonaRuntimeError(EOPEN_FILE, this->_filepath));
	this->_opened = true;

	/* --- ELF PARSING --- */
	this->_parseElf();
	this->_filesize = this->_stream->tellg();
}

void    ElfReader::close(void)
{
	if (this->_opened && this->_stream)
		this->_stream->close();
	if (this->_stream)
	{
		delete this->_stream;
		this->_stream = nullptr;
	}
	this->_opened = false;
}

bool    ElfReader::isOpen(void) const
{ return (this->_opened); }

bool    ElfReader::isELF(void) const
{ return (this->_validElf); }

bool    ElfReader::readAt(u64_t offset, void *buf, size_t len)
{
	if (!buf || !this->_opened || !this->_stream)
		return (false);
	this->_stream->clear();
	this->_stream->seekg(offset, std::ios::beg);
	if (this->_stream->fail())
		return (false);
	this->_stream->read(reinterpret_cast<char *>(buf), len);
	if (this->_stream->gcount() != static_cast<std::streamsize>(len))
		return (false);
	return (true);
}

std::size_t ElfReader::getFilesize(void) const
{ return (this->_filesize); }
