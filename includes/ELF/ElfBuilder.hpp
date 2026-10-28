
#pragma once
#include "includes/ELF/ElfReader.hpp"
#include "includes/types.hpp"
#include <fstream>
#include <linux/elf.h>
#include <vector>
#include <string>

class ElfBuilder
{
	private:
		/* === File related == */
		std::string					_filepath;
		std::ofstream				*_stream;
		std::size_t					_filesize;
		bool						_opened;
		ElfReader					*_elfReader;

		/* === ELF information == */
		Elf64_Ehdr					_ElfHeader;
		std::vector<Elf64_Shdr>		_SectionHeader;
		std::vector<Elf64_Phdr>		_ProgramHeader;
		std::vector<Elf64_Nhdr>		_NoteHeader;

		void						_open(void);

	public:
		/* === sources/ELF/ElfBuilder/constructor.cpp === */
		ElfBuilder(void);
		~ElfBuilder(void);
		ElfBuilder(ElfReader &elfReader);
		ElfBuilder(ElfBuilder &cpy);
		ElfBuilder					&operator=(ElfBuilder &cpy);

		/* === sources/ELF/ElfBuilder/operation.cpp === */
		void						copyReaderData(void);
		void						copyReaderData(ElfReader &reader);

		void						setElfHeader(const Elf64_Ehdr &elf_header);
		void						addSectionHeader(const Elf64_Shdr &section_header);
		void						addProgramHeader(const Elf64_Phdr &program_header);
		void						addNoteHeader(const Elf64_Nhdr &note_header);

		void						removeSectionHeader(const Elf64_Shdr &section_header);
		void						removeProgramHeader(const Elf64_Phdr &section_header);
		void						removeNoteHeader(const Elf64_Nhdr &section_header);

		/* === sources/ELF/ElfBuilder/io.cpp === */
		bool						writeAt(u64_t offset, void *buff, std::size_t size);
		bool						createFile(std::string &filename);
		bool						createFile(void);
		std::size_t					getFilesize(void) const;
};
