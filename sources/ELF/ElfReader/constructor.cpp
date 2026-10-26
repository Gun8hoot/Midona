
#include "includes/ELF/ElfReader.hpp"

ElfReader::ElfReader(void) :
		_filepath(),
		_stream(nullptr),
		_validElf(false),
		_opened(false),
		_silent(false),
		_error(Elf_error::None),
		_header{},
		_sectionHeader(),
		_programHeader()
{ ; }

ElfReader::ElfReader(ElfReader &cpy) :
		_filepath(cpy._filepath),
		_stream(nullptr),
		_validElf(false),
		_opened(false),
		_silent(cpy._silent),
		_error(Elf_error::None),
		_header{},
		_sectionHeader(),
		_programHeader()
{
	this->open(cpy._filepath);
}

ElfReader::ElfReader(std::string &filepath) :
		_filepath(filepath),
		_stream(nullptr),
		_validElf(false),
		_opened(false),
		_silent(false),
		_error(Elf_error::None),
		_header{},
		_sectionHeader(),
		_programHeader()
{
	this->open(filepath);
}
ElfReader::ElfReader(bool shouldSilent) :
		_filepath(),
		_stream(nullptr),
		_validElf(false),
		_opened(false),
		_silent(shouldSilent),
		_error(Elf_error::None),
		_header{},
		_sectionHeader(),
		_programHeader()
{ ; }
ElfReader::ElfReader(std::string &filepath, bool shouldSilent) :
		_filepath(filepath),
		_stream(nullptr),
		_validElf(false),
		_opened(false),
		_silent(shouldSilent),
		_error(Elf_error::None),
		_header{},
		_sectionHeader(),
		_programHeader()
{
	this->open(filepath);
}
ElfReader	&ElfReader::operator=(ElfReader &cpy)
{
	if (&cpy != this)
	{
		if (this->isOpen())
			this->close();
		this->_filepath = cpy._filepath;
		this->open(this->_filepath);
	}
	return (*this);
}

ElfReader::~ElfReader(void)
{
	this->close();
}
