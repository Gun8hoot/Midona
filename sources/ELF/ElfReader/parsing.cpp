
#include "includes/ELF/ElfReader.hpp"
#include "includes/types.hpp"
#include <cstddef>
#include <linux/elf.h>

/*
 * @brief: This function parse the opened ELF file, take information in the ELF header
 * and check that the header is an valid ELF file
 * @throw: Can throw ElfReader::io_error if he failed to read the header or if the header is invalid
 */
void	ElfReader::_parseElfHeader(void)
{
	/* Read the header */
	if (!this->readAt(0, &this->_header, sizeof(this->_header)))
		throw (ElfReader::io_error("Failed to read ELF header", this->_error, Elf_error::ReadFailed));

	/* Check ELF magic number */
	if (this->_header.e_ident[0] != ELFMAG0 ||
			this->_header.e_ident[1] != ELFMAG1 ||
			this->_header.e_ident[2] != ELFMAG2 ||
			this->_header.e_ident[3] != ELFMAG3 ||
			this->_header.e_ident[4] != ELFCLASS64)
		throw (ElfReader::io_error("Invalid ELF header", this->_error, Elf_error::InvalidElf));
	if (!this->_silent)
		std::cout << "\x1b[32m[+] ELF header perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: This function will get every program section header present in the binary
 * @throw: ElfReader::io_error with the corresponding error messages
 */
void	ElfReader::_parseElfSectionHeader(void)
{
	Elf64_Shdr	current_header;
	Elf64_Off	off = this->_header.e_shoff;

	this->_sectionHeader.resize(this->_header.e_shnum);
	for (Elf64_Half section_readed = 0 ; section_readed < this->_header.e_shnum - 1; section_readed++)
	{
		if (!this->readAt(off, &current_header, sizeof(Elf64_Shdr)))
			throw (ElfReader::io_error("Failed to read ELF section header", this->_error, Elf_error::ReadFailed));
		off += sizeof(Elf64_Shdr);
		if (section_readed != 0)
			this->_sectionHeader.push_back(current_header);
	}
	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every section header has been perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: This function will get every program section header present in the binary
 * @throw: ElfReader::io_error with the corresponding error messages
 */
void	ElfReader::_parseElfProgramHeader(void)
{
	Elf64_Phdr	current_header;
	Elf64_Off	off = this->_header.e_phoff;

	this->_programHeader.resize(this->_header.e_phnum);
	for (Elf64_Half section_readed = 0 ; section_readed < this->_header.e_phnum - 1; section_readed++)
	{
		if (!this->readAt(off, &current_header, sizeof(Elf64_Phdr)))
			throw (ElfReader::io_error("Failed to read ELF program header", this->_error, Elf_error::ReadFailed));
		off += sizeof(Elf64_Phdr);
		this->_programHeader.push_back(current_header);
	}
	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every program header has been perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: This function will get every note section header present in the binary
 * @throw: ElfReader::io_error with the corresponding error messages
 */
void	ElfReader::_parseElfNoteHeader(void)
{
	auto		it = this->_sectionHeader.begin();
	auto		ite = this->_sectionHeader.end();
	Elf64_Nhdr	current_header;

	while (it != ite)
	{
		if ((*it).sh_type == SHT_NOTE)
		{
			if (!readAt((*it).sh_offset, &current_header, sizeof(Elf64_Nhdr)))
				throw (ElfReader::io_error("Failed to read ELF note header", this->_error, Elf_error::ReadFailed));
			this->_noteHeader.push_back(current_header);
		}
		++it;
	}
	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every note header has been perfectly parse!\x1b[0m" << std::endl;
}
