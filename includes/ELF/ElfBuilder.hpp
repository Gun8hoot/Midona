#pragma once
#include "includes/ELF/ElfReader.hpp"
#include "includes/types.hpp"
#include <fstream>
#include <linux/elf.h>
#include <vector>
#include <string>
#include <cstdint>

class ElfBuilder
{
	private:
		/* === File related == */
		std::string                 _filepath;
		std::ofstream               *_stream;
		std::size_t                 _filesize;
		bool                        _opened;
		ElfReader                   *_elfReader;

		/* === ELF information == */
		Elf64_Ehdr                  _ElfHeader;
		std::vector<Elf64_Shdr>     _SectionHeader;
		std::vector<Elf64_Phdr>     _ProgramHeader;
		std::vector<Elf64_Nhdr>     _NoteHeader;

		/* Index-aligned with _SectionHeader. Empty for SHT_NOBITS/SHT_NULL. */
		std::vector<std::vector<uint8_t>> _SectionData;

		void                        _open(void);
		std::size_t                 _computeLayout(void);   // private

	public:
		/* === sources/ELF/ElfBuilder/constructor.cpp === */
		ElfBuilder(void);
		~ElfBuilder(void);
		ElfBuilder(ElfReader &elfReader);
		ElfBuilder(ElfBuilder &cpy);
		ElfBuilder                  &operator=(ElfBuilder &cpy);

		/* === sources/ELF/ElfBuilder/entry.cpp === */
		void                        setEntryPoint(uint64_t vaddr);
		bool						setEntryPointToSection(std::size_t section_index,
														   std::size_t offset_in_section,
														   std::size_t jmp_rel32_offset = SIZE_MAX);

		/* === sources/ELF/ElfBuilder/operation.cpp === */
		void                        copyReaderData(void);
		void                        copyReaderData(ElfReader &reader);

		void                        setElfHeader(const Elf64_Ehdr &elf_header);
		void                        addSectionHeader(const Elf64_Shdr &section_header);
		void                        addProgramHeader(const Elf64_Phdr &program_header);
		void                        addNoteHeader(const Elf64_Nhdr &note_header);

		/* Adds a new section together with its payload in one call. */
		void                        addSection(const Elf64_Shdr &section_header,
											   const std::vector<uint8_t> &data);
		void                        setSectionData(std::size_t index,
												  const std::vector<uint8_t> &data);

		void                        removeSectionHeader(const Elf64_Shdr &section_header);
		void                        removeProgramHeader(const Elf64_Phdr &section_header);
		void                        removeNoteHeader(const Elf64_Nhdr &section_header);

		/* === sources/ELF/ElfBuilder/io.cpp === */
		bool                        writeAt(u64_t offset, void *buff, std::size_t size);
		bool                        createFile(std::string &filename);
		bool                        createFile(void);
		std::size_t                 getFilesize(void) const;

		/* Accessors */
		std::vector<std::vector<uint8_t>>  getSectionData(void) const;
		std::vector<uint8_t>               getSectionData(std::size_t index) const;
};
