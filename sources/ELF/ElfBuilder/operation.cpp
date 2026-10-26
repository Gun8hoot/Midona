
#include "includes/ELF/ElfBuilder.hpp"
#include <cstring>

void	ElfBuilder::copyReaderData(void)
{
	this->_ElfHeader = this->_elfReader->getElfHeader();
	this->_SectionHeader = this->_elfReader->getSectionHeader();
	this->_ProgramHeader = this->_elfReader->getProgramHeader();
	// this->_NoteHeader = this->_elfReader->get();
}

void	ElfBuilder::copyReaderData(ElfReader &reader)
{
	this->_elfReader = &reader;
	this->copyReaderData();
}

void	ElfBuilder::setElfHeader(const Elf64_Ehdr &elf_header)
{
	this->_ElfHeader = elf_header;
}
void	ElfBuilder::addSectionHeader(const Elf64_Shdr &section_header)
{
	this->_SectionHeader.push_back(section_header);
}
void	ElfBuilder::addProgramHeader(const Elf64_Phdr &program_header)
{
	this->_ProgramHeader.push_back(program_header);

}
void	ElfBuilder::addNoteHeader(const Elf64_Nhdr &note_header)
{
	this->_NoteHeader.push_back(note_header);
}

void	ElfBuilder::removeSectionHeader(const Elf64_Shdr &section_header)
{
	auto it = this->_SectionHeader.begin();
	auto ite = this->_SectionHeader.end();

	while (it != ite)
	{
		if (std::memcmp(&*it, &section_header, sizeof(Elf64_Shdr)) == 0)
		{
			this->_SectionHeader.erase(it);
			break;
		}
		++it;
	}
}
void	ElfBuilder::removeProgramHeader(const Elf64_Phdr &section_header)
{

}
void	ElfBuilder::removeNoteHeader(const Elf64_Nhdr &section_header)
{

}

bool	ElfBuilder::createFile(std::string &filename)
{
	return (true);
}
