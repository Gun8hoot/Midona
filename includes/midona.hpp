
#pragma once
#include <string>
#include <exception>

/* The magic bytes located on the section data to know if this current section is a key fragment */
#define MAGIC_KEY_SECTION { '\xAF', '\x76', '\xBE', '\x13', '\x3C', '\x88', '\xFF', '\x03' }
/* The total size in bytes of the key (32 bytes = 256bit) */
#define KEY_SECTION_SIZE 32


/* The exception class who can do string formating and custom message on the what message */
class MidonaRuntimeError : public std::exception
{
	private:
		std::string _msg;

	public:
		MidonaRuntimeError(std::string msg) : _msg(msg) { ; };
		MidonaRuntimeError(std::string msg, std::string format) : _msg(msg)
		{
			std::size_t	form = msg.find("%s");
			if (form != std::string::npos)
				this->_msg.replace(form, 2, format);
		};
		~MidonaRuntimeError(void) {;};

		virtual const char *what(void) const noexcept
		{ return (_msg.c_str()); }
};
