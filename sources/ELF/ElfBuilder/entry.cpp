#include "includes/ELF/ElfBuilder.hpp"
#include "includes/midona.hpp"
#include <linux/elf.h>
#include <climits>
#include <cstdint>
#include <string>

namespace {
	inline uint64_t pageAlignUp(uint64_t v, uint64_t a = 0x1000) {
		return ((v + a - 1) & ~(a - 1));
	}

	/* PT_GNU_SFRAME may not be defined in older <linux/elf.h>. So if not i define it */
	#ifndef PT_GNU_SFRAME
	# define PT_GNU_SFRAME 0x6474e554
	#endif
}

/*
 * Redirect e_entry to section_index + offset_in_section.
 *
 * Strategy:
 *   1. Repurpose an existing phdr the kernel ignores at exec time
 *      (PT_NOTE -> PT_GNU_PROPERTY -> PT_GNU_EH_FRAME -> PT_GNU_SFRAME).
 *      This keeps e_phnum constant, so nothing downstream shifts.
 *   2. Page-align the section, mark it SHF_ALLOC | SHF_EXECINSTR.
 *   3. Run _computeLayout() so sh_offset is assigned.
 *   4. Fill in the repurposed phdr as PT_LOAD (PF_R | PF_X).
 *   5. Pick p_vaddr above every other PT_LOAD (mod 0x1000 invariant
 *      preserved because both p_offset and p_vaddr are page-aligned).
 *   6. If jmp_rel32_offset is set, patch that E9 in the section data
 *      to jump back to the ORIGINAL e_entry (link-time vaddr space, so
 *      it works identically for PIE and non-PIE).
 */
bool    ElfBuilder::setEntryPointToSection(std::size_t section_index,
										   std::size_t offset_in_section,
										   std::size_t jmp_rel32_offset)
{
	const std::size_t NONE = static_cast<std::size_t>(-1);

	if (section_index >= this->_SectionHeader.size())
		throw (MidonaRuntimeError("ElfBuilder: section index out of range"));
	if (this->_SectionData.size() != this->_SectionHeader.size())
		throw (MidonaRuntimeError("ElfBuilder: section data out of sync"));

	Elf64_Shdr &sec = this->_SectionHeader[section_index];

	if (sec.sh_type == SHT_NULL || sec.sh_type == SHT_NOBITS)
		throw (MidonaRuntimeError("ElfBuilder: cannot use SHT_NULL/SHT_NOBITS as entry"));
	if (this->_SectionData[section_index].empty())
		throw (MidonaRuntimeError("ElfBuilder: entry section has no data"));
	if (offset_in_section >= this->_SectionData[section_index].size())
		throw (MidonaRuntimeError("ElfBuilder: entry offset out of bounds"));

	/* Snapshot the original entry BEFORE overwriting it. */
	const uint64_t orig_entry = this->_ElfHeader.e_entry;

	/* Page-align and mark executable. */
	sec.sh_addralign  = 0x1000;
	sec.sh_flags     |= (SHF_ALLOC | SHF_EXECINSTR);

	static const Elf64_Word candidates[] = {
		PT_NOTE,
		PT_GNU_PROPERTY,
		PT_GNU_EH_FRAME,
		static_cast<Elf64_Word>(PT_GNU_SFRAME),
	};

	std::size_t load_index = NONE;
	for (std::size_t c = 0; c < sizeof(candidates)/sizeof(candidates[0]); ++c)
	{
		for (std::size_t i = 0; i < this->_ProgramHeader.size(); ++i)
		{
			if (this->_ProgramHeader[i].p_type == candidates[c])
			{
				load_index = i;
				break;
			}
		}
		if (load_index != NONE)
			break;
	}

	if (load_index == NONE)
		throw (MidonaRuntimeError(
			"ElfBuilder: no repurposable phdr "
			"(need one of PT_NOTE, PT_GNU_PROPERTY, PT_GNU_EH_FRAME, PT_GNU_SFRAME)"));

	this->_computeLayout();

	const std::size_t p_offset = sec.sh_offset;
	const std::size_t p_size   = sec.sh_size;
	if (p_offset == 0 || p_size == 0)
		throw (MidonaRuntimeError("ElfBuilder: layout did not assign an offset"));

	uint64_t max_vend = 0;
	for (std::size_t i = 0; i < this->_ProgramHeader.size(); ++i)
	{
		if (i == load_index)
			continue;
		const Elf64_Phdr &ph = this->_ProgramHeader[i];
		if (ph.p_type != PT_LOAD || ph.p_memsz == 0)
			continue;
		const uint64_t end = ph.p_vaddr + ph.p_memsz;
		if (end > max_vend)
			max_vend = end;
	}
	if (max_vend == 0)
		max_vend = 0x400000;  /* fallback base for exotic binaries */

	const uint64_t p_vaddr = pageAlignUp(max_vend);

	Elf64_Phdr &ph = this->_ProgramHeader[load_index];
	ph.p_type   = PT_LOAD;
	ph.p_flags  = PF_R | PF_X;
	ph.p_offset = p_offset;
	ph.p_vaddr  = p_vaddr;
	ph.p_paddr  = p_vaddr;
	ph.p_filesz = p_size;
	ph.p_memsz  = p_size;
	ph.p_align  = 0x1000;

	this->_ElfHeader.e_entry = p_vaddr + offset_in_section;

	/* Patch the trailing jmp rel32 in the stub if requested. */
	if (jmp_rel32_offset != NONE)
	{
		std::vector<uint8_t> &data = this->_SectionData[section_index];

		if (jmp_rel32_offset + 5 > data.size())
			throw (MidonaRuntimeError("ElfBuilder: jmp_rel32_offset out of bounds"));
		if (data[jmp_rel32_offset] != 0xE9)
			throw (MidonaRuntimeError("ElfBuilder: expected E9 opcode in stub"));

		/* Runtime RIP after the jmp equals (p_vaddr + jmp_rel32_offset + 5).
		   Target is (base + orig_entry). Base cancels out for PIE and
		   non-PIE alike, so this is a pure link-time subtraction. */
		const int64_t rel64 = (int64_t)orig_entry
							- (int64_t)(p_vaddr + jmp_rel32_offset + 5);

		if (rel64 < INT32_MIN || rel64 > INT32_MAX)
			throw (MidonaRuntimeError("ElfBuilder: jmp rel32 out of range"));

		const int32_t rel32 = (int32_t)rel64;
		data[jmp_rel32_offset + 1] = (uint8_t)(rel32      );
		data[jmp_rel32_offset + 2] = (uint8_t)(rel32 >>  8);
		data[jmp_rel32_offset + 3] = (uint8_t)(rel32 >> 16);
		data[jmp_rel32_offset + 4] = (uint8_t)(rel32 >> 24);
	}

	return (true);
}

void    ElfBuilder::setEntryPoint(uint64_t vaddr)
{
	this->_ElfHeader.e_entry = vaddr;
}
