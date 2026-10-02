#include <elf.h>

class elf64_io
{
	public:
		elf64_io(void);
		elf64_io(const elf64_io &cpy);
		elf64_io &operator=(const elf64_io &cpy);
		~elf64_io(void);
}
