#include "includes/ELF/ElfBuilder.hpp"
#include "includes/ELF/ElfReader.hpp"

ElfBuilder::ElfBuilder(void) :
	_filepath(),
	_stream(nullptr),
	_filesize(0),
	_opened(false),
	_elfReader(nullptr),
	_ElfHeader(),
	_SectionHeader(),
	_ProgramHeader(),
	_NoteHeader(),
	_SectionData()
{ ; }

ElfBuilder::ElfBuilder(ElfReader &elfReader) :
	_filepath(elfReader.getFilepath()),
	_stream(nullptr),
	_filesize(0),
	_opened(false),
	_elfReader(&elfReader),
	_ElfHeader(),
	_SectionHeader(),
	_ProgramHeader(),
	_NoteHeader(),
	_SectionData()
{
	this->copyReaderData();
}

ElfBuilder::ElfBuilder(ElfBuilder &cpy) :
	_filepath(cpy._filepath),
	_stream(nullptr),
	_filesize(cpy._filesize),
	_opened(false),
	_elfReader(cpy._elfReader),
	_ElfHeader(cpy._ElfHeader),
	_SectionHeader(cpy._SectionHeader),
	_ProgramHeader(cpy._ProgramHeader),
	_NoteHeader(cpy._NoteHeader),
	_SectionData(cpy._SectionData)
{ ; }

ElfBuilder::~ElfBuilder(void)
{
	if (this->_opened && this->_stream)
	{
		this->_stream->flush();
		this->_stream->close();
	}
	if (this->_stream)
	{
		delete this->_stream;
		this->_stream = nullptr;
	}
}

ElfBuilder  &ElfBuilder::operator=(ElfBuilder &cpy)
{
	if (this != &cpy)
	{
		this->_filepath      = cpy._filepath;
		this->_elfReader     = cpy._elfReader;
		this->_filesize      = cpy._filesize;

		this->_ElfHeader     = cpy._ElfHeader;
		this->_SectionHeader = cpy._SectionHeader;
		this->_ProgramHeader = cpy._ProgramHeader;
		this->_NoteHeader    = cpy._NoteHeader;
		this->_SectionData   = cpy._SectionData;
	}
	return (*this);
}
