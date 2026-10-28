
#include "includes/ELF/ElfBuilder.hpp"
#include "includes/error.hpp"
#include "includes/midona.hpp"
#include <fstream>
#include <istream>
#include <linux/elf.h>
#include <sys/stat.h>

void	ElfBuilder::_open(void)
{
	this->_stream = new (std::nothrow) std::ofstream();
	if (!this->_stream)
		throw (MidonaRuntimeError(EALLOC_ERROR, "ElfBuilder file stream"));
	this->_stream->open(this->_filepath, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!this->_stream->is_open())
		throw (MidonaRuntimeError(EOPEN_FILE, this->_filepath));
	_opened = true;
}

bool	ElfBuilder::writeAt(u64_t offset, void *buff, std::size_t size)
{
	if (!buff || !this->_opened)
		return (false);
	this->_stream->seekp(offset);
	this->_stream->write(reinterpret_cast<char*>(buff), size);
	if (!this->_stream)
		return (false);
	this->_filesize += size;
	return (true);
}

bool	ElfBuilder::createFile(std::string &filepath)
{
	this->_filepath = filepath;
	return (this->createFile());
}

std::size_t	ElfBuilder::getFilesize(void) const
{
	return (this->_filesize);
}

bool	ElfBuilder::createFile(void)
{
	this->_open();

	/* Write the ELF header at the top of the file */
	this->writeAt(this->_stream->tellp(), &this->_ElfHeader, sizeof(Elf64_Ehdr));

	auto it = this->_ProgramHeader.begin();
	auto ite = this->_ProgramHeader.end();

	while (it != ite)
	{
		writeAt(this->_stream->tellp(), &*it, sizeof(Elf64_Phdr));
		++it;
	}

	/* Change the output file permission to rwx:r-x:r-x */
	if (chmod(this->_filepath.c_str(), S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH) == -1)
		throw (MidonaRuntimeError("Failed to change %s permission", this->_filepath));
	return (true);
}
