
#include "includes/ELF/ElfReader.hpp"
#include "includes/error.hpp"
#include "includes/midona.hpp"
#include <exception>

/*
 * @brief: Will try to open the file and assign the correct error state
 * if the file cannot be properly open/read
 */
void	ElfReader::open(std::string &filepath)
{
	/* --- FILE PREPARATION --- */
	if (isOpen())
		this->close();
	this->_stream = new (std::nothrow) std::ifstream(filepath, std::ios::in | std::ios::binary);
	if (this->_stream == nullptr)
		throw (MidonaRuntimeError(EALLOC_ERROR, "ElfRead file stream"));
	if (!this->_stream->is_open())
		throw (MidonaRuntimeError(EOPEN_FILE, this->_filepath));
	this->_opened = true;

	/* --- ELF PARSING --- */
	this->_parseElfHeader();
	this->_parseElfSectionHeader();
	this->_parseElfProgramHeader();
	this->_parseElfNoteHeader();
	this->_filesize = this->_stream->tellg();
}

/*
 * @brief: Properly close the opened file and release the memory allocated for the stream
 */
void	ElfReader::close(void)
{
	if (this->_opened)
		this->_stream->close();
	if (this->_stream)
	{
		delete this->_stream;
		this->_stream = nullptr;
	}
}

/*
 * @brief: Check if the file is currently openned
 * @return: True if the file is still open, False if not
 */
bool	ElfReader::isOpen(void) const
{ return (this->_opened); }

/*
 * @brief: This methode read the file at an offset and put his result in buf
 * @param offset: The location where we should start to read
 * @param buf: The place where the readed data should be put
 * @param len: The number of bytes we should read
 * @return: True if the data was correclty readed, False if not
 * @info: Set ElfReader::_error to Elf_error::ReadFailed if the read have failed
 */
bool	ElfReader::readAt(u64_t offset, void *buf, size_t len)
{
	if (!buf || !this->_opened)
		return (false);
	this->_stream->seekg(offset, std::ios::beg);
	if (this->_stream->fail())
		return(false);
	this->_stream->read(reinterpret_cast<char *>(buf), len);
	if (this->_stream->gcount() != static_cast<std::streamsize>(len))
		return(false);
	return (true);
}

std::size_t	ElfReader::getFilesize(void) const
{
	return (this->_filesize);
}
