#include "includes/ELF/ElfBuilder.hpp"
#include <cstring>

/* Copy headers AND section data from the reader. */
void    ElfBuilder::copyReaderData(void)
{
	this->_ElfHeader      = this->_elfReader->getElfHeader();
	this->_SectionHeader  = this->_elfReader->getSectionHeader();
	this->_ProgramHeader  = this->_elfReader->getProgramHeader();
	this->_NoteHeader     = this->_elfReader->getNoteHeader();
	this->_SectionData    = this->_elfReader->getSectionData();
}

void    ElfBuilder::copyReaderData(ElfReader &reader)
{
	this->_elfReader = &reader;
	this->copyReaderData();
}

void    ElfBuilder::setElfHeader(const Elf64_Ehdr &elf_header)
{
	this->_ElfHeader = elf_header;
}

/* Header-only add (data stays empty until setSectionData is used). */
void    ElfBuilder::addSectionHeader(const Elf64_Shdr &section_header)
{
	this->_SectionHeader.push_back(section_header);
	this->_SectionData.push_back(std::vector<uint8_t>());
}

/* Header + payload in one shot. This is the API you want to use when
   injecting a new .packed section into an ELF. */
void    ElfBuilder::addSection(const Elf64_Shdr &section_header,
							   const std::vector<uint8_t> &data)
{
	this->_SectionHeader.push_back(section_header);
	this->_SectionData.push_back(data);
}

void    ElfBuilder::setSectionData(std::size_t index,
								   const std::vector<uint8_t> &data)
{
	if (index >= this->_SectionData.size())
		this->_SectionData.resize(index + 1);
	this->_SectionData[index] = data;
}

void    ElfBuilder::addProgramHeader(const Elf64_Phdr &program_header)
{
	this->_ProgramHeader.push_back(program_header);
}

void    ElfBuilder::addNoteHeader(const Elf64_Nhdr &note_header)
{
	this->_NoteHeader.push_back(note_header);
}

void    ElfBuilder::removeSectionHeader(const Elf64_Shdr &section_header)
{
	for (std::size_t i = 0; i < this->_SectionHeader.size(); ++i)
	{
		if (std::memcmp(&this->_SectionHeader[i], &section_header,
						sizeof(Elf64_Shdr)) == 0)
		{
			this->_SectionHeader.erase(this->_SectionHeader.begin() + i);
			this->_SectionData.erase(this->_SectionData.begin() + i);
			return;
		}
	}
}

void    ElfBuilder::removeProgramHeader(const Elf64_Phdr &program_header)
{
	for (std::size_t i = 0; i < this->_ProgramHeader.size(); ++i)
	{
		if (std::memcmp(&this->_ProgramHeader[i], &program_header,
						sizeof(Elf64_Phdr)) == 0)
		{
			this->_ProgramHeader.erase(this->_ProgramHeader.begin() + i);
			return;
		}
	}
}

void    ElfBuilder::removeNoteHeader(const Elf64_Nhdr &note_header)
{
	for (std::size_t i = 0; i < this->_NoteHeader.size(); ++i)
	{
		if (std::memcmp(&this->_NoteHeader[i], &note_header,
						sizeof(Elf64_Nhdr)) == 0)
		{
			this->_NoteHeader.erase(this->_NoteHeader.begin() + i);
			return;
		}
	}
}

std::vector<std::vector<uint8_t>> ElfBuilder::getSectionData(void) const
{ return (this->_SectionData); }

std::vector<uint8_t> ElfBuilder::getSectionData(std::size_t index) const
{
	if (index >= this->_SectionData.size())
		return (std::vector<uint8_t>());
	return (this->_SectionData[index]);
}
