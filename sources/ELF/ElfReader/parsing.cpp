#include "includes/ELF/ElfReader.hpp"
#include "includes/midona.hpp"
#include "includes/types.hpp"
#include <cstddef>
#include <cstring>
#include <linux/elf.h>

/*
 * @brief: Orchestrates the full ELF parsing.
 * Called by ElfReader::open().
 */
bool    ElfReader::_parseElf(void)
{
	this->_parseElfHeader();
	this->_parseElfSectionHeader();
	this->_parseElfProgramHeader();
	this->_parseElfNoteHeader();
	this->_validElf = true;
	return (true);
}

/*
 * @brief: Reads + validates the ELF header.
 */
void    ElfReader::_parseElfHeader(void)
{
	this->_validElf = false;

	if (!this->readAt(0, &this->_header, sizeof(this->_header)))
		throw (MidonaRuntimeError("Failed to read ELF header"));

	if (this->_header.e_ident[0] != ELFMAG0 ||
		this->_header.e_ident[1] != ELFMAG1 ||
		this->_header.e_ident[2] != ELFMAG2 ||
		this->_header.e_ident[3] != ELFMAG3 ||
		this->_header.e_ident[4] != ELFCLASS64)
		throw (MidonaRuntimeError("Invalid ELF header"));

	if (this->_header.e_shnum > 0 && this->_header.e_shoff == 0)
		throw (MidonaRuntimeError("Invalid ELF: e_shoff is 0 but e_shnum > 0"));

	if (!this->_silent)
		std::cout << "\x1b[32m[+] ELF header perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: Reads every section header AND every section's raw data.
 *         Data is stored index-aligned with _sectionHeader so that
 *         ElfBuilder can rebuild the file byte-for-byte.
 */
void    ElfReader::_parseElfSectionHeader(void)
{
	this->_sectionHeader.clear();
	this->_sectionData.clear();

	if (this->_header.e_shnum == 0)
		return;

	Elf64_Off off = this->_header.e_shoff;
	this->_sectionHeader.reserve(this->_header.e_shnum);
	this->_sectionData.reserve(this->_header.e_shnum);

	for (Elf64_Half i = 0; i < this->_header.e_shnum; i++)
	{
		Elf64_Shdr current;
		if (!this->readAt(off, &current, sizeof(Elf64_Shdr)))
			throw (MidonaRuntimeError("Failed to read ELF section header"));
		off += sizeof(Elf64_Shdr);
		this->_sectionHeader.push_back(current);

		/* SHT_NULL, SHT_NOBITS and zero-sized sections carry no file data */
		const bool has_data = (current.sh_type != SHT_NULL)
						   && (current.sh_type != SHT_NOBITS)
						   && (current.sh_size  != 0);

		if (!has_data)
		{
			this->_sectionData.push_back(std::vector<uint8_t>());
			continue;
		}

		std::vector<uint8_t> data(current.sh_size);
		if (!this->readAt(current.sh_offset, data.data(), current.sh_size))
			throw (MidonaRuntimeError("Failed to read ELF section data"));
		this->_sectionData.push_back(std::move(data));
	}

	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every section header has been perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: Reads every program header.
 */
void    ElfReader::_parseElfProgramHeader(void)
{
	this->_programHeader.clear();

	if (this->_header.e_phnum == 0)
		return;

	Elf64_Off off = this->_header.e_phoff;
	this->_programHeader.reserve(this->_header.e_phnum);

	for (Elf64_Half i = 0; i < this->_header.e_phnum; i++)
	{
		Elf64_Phdr current;
		if (!this->readAt(off, &current, sizeof(Elf64_Phdr)))
			throw (MidonaRuntimeError("Failed to read ELF program header"));
		off += sizeof(Elf64_Phdr);
		this->_programHeader.push_back(current);
	}

	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every program header has been perfectly parse!\x1b[0m" << std::endl;
}

/*
 * @brief: Reads every note entry from every SHT_NOTE section.
 */
void    ElfReader::_parseElfNoteHeader(void)
{
	this->_noteHeader.clear();

	for (const Elf64_Shdr &shdr : this->_sectionHeader)
	{
		if (shdr.sh_type != SHT_NOTE || shdr.sh_size == 0)
			continue;

		std::vector<uint8_t> buf(shdr.sh_size);
		if (!this->readAt(shdr.sh_offset, buf.data(), shdr.sh_size))
			throw (MidonaRuntimeError("Failed to read ELF note section"));

		std::size_t off = 0;
		while (off + sizeof(Elf64_Nhdr) <= buf.size())
		{
			Elf64_Nhdr nhdr;
			std::memcpy(&nhdr, buf.data() + off, sizeof(Elf64_Nhdr));
			this->_noteHeader.push_back(nhdr);

			const std::size_t namesz = (nhdr.n_namesz + 3u) & ~3u;
			const std::size_t descsz = (nhdr.n_descsz + 3u) & ~3u;
			const std::size_t step   = sizeof(Elf64_Nhdr) + namesz + descsz;
			if (step == 0) break;
			off += step;
		}
	}

	if (!this->_silent)
		std::cout << "\x1b[32m[+] Every note header has been perfectly parse!\x1b[0m" << std::endl;
}
