
#pragma once
#include "includes/types.hpp"
#include <fstream>
#include <linux/elf.h>
#include <vector>
#include <string>

class ElfBuilder
{
	private:
		/* Attributes */
		std::ofstream	_stream;
		bool			_fileState;

		/* Private methodes */
	public:
		/* Constructor */
		ElfBuilder(void);
		~ElfBuilder(void);
		ElfBuilder(ElfBuilder &cpy);
		ElfBuilder	&operator=(ElfBuilder &cpy);

		/* Public methodes */
		void		createSection(const Elf64_Shdr &section_header, std::vector<u8_t> &section_data);
		bool		createFile(std::string &filename);
};
