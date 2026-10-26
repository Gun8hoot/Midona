
#include "includes/ELF/ElfBuilder.hpp"

ElfBuilder::ElfBuilder(void) :
	_filepath(),
	_stream(nullptr),
	_fileState(false),
	_elfReader(nullptr),
	_ElfHeader(),
	_SectionHeader(),
	_ProgramHeader(),
	_NoteHeader()
{ ; }

ElfBuilder::ElfBuilder(ElfBuilder &cpy) :
	_filepath(),
	_stream(nullptr),
	_fileState(false),
	_elfReader(cpy._elfReader),
	_ElfHeader(cpy._ElfHeader),
	_SectionHeader(cpy._SectionHeader),
	_ProgramHeader(cpy._ProgramHeader),
	_NoteHeader(cpy._NoteHeader)
{ ; }

ElfBuilder::~ElfBuilder(void)
{ ; }

ElfBuilder	&ElfBuilder::operator=(ElfBuilder &cpy)
{
	(void)cpy;
	return (*this);
}
