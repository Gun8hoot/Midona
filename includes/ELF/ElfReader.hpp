
#pragma once

#include "includes/types.hpp"
#include <linux/elf.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <exception>
#include <new>
#include <cstring>
#include <string>
#include <vector>

enum class		Elf_error { None, AlreadyOpen, FailedOpen, NotELF, ReadFailed, Unsupported, FailedAlloc };

#define ERR_ELF_READ "Failed to read %s"
#define ERR_ELF_INVAL "%s is not an ELF"

class ElfReader
{
	private:
		/* --- Private attributes --- */
		std::string				_filepath;
		std::ifstream			*_stream;
		bool					_validElf;
		bool					_opened;
		Elf_error				_error;

		Elf64_Ehdr				_header;
		std::vector<Elf64_Shdr>	_sectionHeader;
		std::vector<Elf64_Phdr>	_programHeader;
		std::vector<u8_t>		*_sectionContent;

		/* === ELF FILE INFORMATION PARSING === */
		bool					_parseElf(void);
		void					_parseElfSectionHeader(void);
		void					_parseElfHeader(void);
		void					_parseElfProgramHeader(void);

	public:
		/* === CONSTRUCTOR === */
		ElfReader(void);
		~ElfReader(void);
		ElfReader(ElfReader &cpy);
		ElfReader(std::string &filepath);
		ElfReader				&operator=(ElfReader &cpy);

		/* === I/O === */
		bool					open(std::string &filepath);
		void					close(void);
		bool					isOpen(void) const;
		bool					isELF(void) const;
		Elf_error				error(void) const;

		/* === Getter === */
		std::string				getFilepath(void) const;
		Elf64_Ehdr				getElfHeader(void) const;
		std::vector<Elf64_Shdr>	getSectionHeader(void) const;
		std::vector<Elf64_Phdr>	getProgramHeader(void) const;
		std::vector<u8_t>		*getSectionData(Elf64_Shdr hdr);
};
