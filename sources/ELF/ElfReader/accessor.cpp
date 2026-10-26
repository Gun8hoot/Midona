
#include "includes/ELF/ElfReader.hpp"

/*
 * @brief: This function return the path of the path
 * @return: The path to the file
 */
std::string	ElfReader::getFilepath(void) const
{
	return (this->_filepath);
}
/*
 * @brief: This function return the struct with every information in the ELF header
 * @param: The Elf64_Ehdr structure with every information already filled
 */
Elf64_Ehdr	ElfReader::getElfHeader(void) const
{
	return (this->_header);
}
/*
 * @brief: This function return a vector of Elf64_Shdr struct with every section header in it
 * @param: The Elf64_Shdr vector with every section header filled
 */
std::vector<Elf64_Shdr>	ElfReader::getSectionHeader(void) const
{
	return (this->_sectionHeader);
}
/*
 * @brief: This function return a vector of Elf64_Phdr struct with every section header in it
 * @param: The Elf64_Phdr vector with every section header filled
 */
std::vector<Elf64_Phdr>	ElfReader::getProgramHeader(void) const
{
	return (this->_programHeader);
}
