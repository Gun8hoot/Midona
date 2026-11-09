#include "includes/ELF/ElfReader.hpp"
#include <linux/elf.h>

std::string ElfReader::getFilepath(void) const
{ return (this->_filepath); }

Elf64_Ehdr  ElfReader::getElfHeader(void) const
{ return (this->_header); }

std::vector<Elf64_Shdr> ElfReader::getSectionHeader(void) const
{ return (this->_sectionHeader); }

std::vector<Elf64_Phdr> ElfReader::getProgramHeader(void) const
{ return (this->_programHeader); }

std::vector<Elf64_Nhdr> ElfReader::getNoteHeader(void) const
{ return (this->_noteHeader); }

std::vector<std::vector<uint8_t>> ElfReader::getSectionData(void) const
{ return (this->_sectionData); }

std::vector<uint8_t> ElfReader::getSectionData(std::size_t index) const
{
	if (index >= this->_sectionData.size())
		return (std::vector<uint8_t>());
	return (this->_sectionData[index]);
}
