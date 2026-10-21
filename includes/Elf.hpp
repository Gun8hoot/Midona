
#pragma once

#include <linux/elf.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <exception>
#include <new>
#include <cstring>
#include <string>

enum class		Elf_error { None, AlreadyOpen, FailedOpen, NotELF, ReadFailed, Unsupported, FailedAlloc };

class Elf
{
	private:
		/* --- Private attributes --- */
		std::fstream	*_stream;
		bool			_validElf;
		bool			_opened;
		Elf64_Ehdr		_header;
		Elf_error		_error;

		/* --- Private methodes --- */
		void			_parseElfHeader(void);

	public:
		/* === CONSTRUCTOR === */
		Elf(void);
		Elf(Elf &cpy);
		Elf(std::string &filename);
		Elf	&operator=(Elf &cpy);
		~Elf(void);

		/* I/O */
		bool		open(std::string &filename);
		void		close(void);
		bool		isOpen(void) const;
		bool		isELF(void) const;
		Elf_error	error(void) const;

		/* Getter */
		Elf64_Ehdr	getHeader(void) const;
};
