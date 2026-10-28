
#pragma once

#include "includes/types.hpp"
#include "includes/midona.hpp"
#include <cstddef>
#include <linux/elf.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <exception>
#include <new>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <vector>

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
		std::size_t				_filesize;
		bool					_silent;

		/* === ELF DATA === */
		Elf64_Ehdr				_header;
		std::vector<Elf64_Shdr>	_sectionHeader;
		std::vector<Elf64_Phdr>	_programHeader;
		std::vector<Elf64_Nhdr>	_noteHeader;

		/* === sources/ELF/parsing.cpp === */
		bool					_parseElf(void);
		void					_parseElfHeader(void);
		void					_parseElfSectionHeader(void);
		void					_parseElfProgramHeader(void);
		void					_parseElfNoteHeader(void);

	public:
		/* === sources/ELF/constructor.cpp === */
		ElfReader(void);
		~ElfReader(void);
		ElfReader(ElfReader &cpy);
		ElfReader(bool shouldSilent);
		ElfReader(std::string &filepath);
		ElfReader(std::string &filepath, bool shouldVerbose);
		ElfReader				&operator=(ElfReader &cpy);

		/* === sources/ELF/io.cpp === */
		void					open(std::string &filepath);
		void					close(void);
		bool					isOpen(void) const;
		bool					isELF(void) const;
		bool					readAt(u64_t offset, void *buf, size_t len);
		std::size_t				getFilesize(void) const;

		/* === sources/ELF/getter.cpp === */
		std::string				getFilepath(void) const;
		Elf64_Ehdr				getElfHeader(void) const;
		std::vector<Elf64_Shdr>	getSectionHeader(void) const;
		std::vector<Elf64_Phdr>	getProgramHeader(void) const;
		std::vector<Elf64_Nhdr>	getNoteHeader(void) const;
};
