#include "includes/ELF/ElfBuilder.hpp"
#include "includes/error.hpp"
#include "includes/midona.hpp"
#include <fstream>
#include <linux/elf.h>
#include <sys/stat.h>

std::size_t ElfBuilder::_computeLayout(void)
{
    std::size_t file_end = sizeof(Elf64_Ehdr)
                         + this->_ProgramHeader.size() * sizeof(Elf64_Phdr);

    for (std::size_t i = 0; i < this->_SectionHeader.size(); ++i)
    {
        Elf64_Shdr &s = this->_SectionHeader[i];

        if (s.sh_type == SHT_NULL || s.sh_type == SHT_NOBITS)
            continue;

        if (this->_SectionData[i].empty())
        {
            s.sh_size = 0;
            continue;
        }

        const std::size_t data_size = this->_SectionData[i].size();
        const bool preserved = (s.sh_offset != 0) && (s.sh_size == data_size);

        if (preserved)
        {
            const std::size_t end = s.sh_offset + data_size;
            if (end > file_end) file_end = end;
        }
        else
        {
            std::size_t align = s.sh_addralign ? s.sh_addralign : 1;
            const std::size_t off = (file_end + align - 1) & ~(align - 1);
            s.sh_offset = off;
            s.sh_size   = data_size;
            file_end    = off + data_size;
        }
    }
    return (file_end);
}

void    ElfBuilder::_open(void)
{
    if (this->_stream)
    {
        if (this->_opened) { this->_stream->flush(); this->_stream->close(); }
        delete this->_stream;
        this->_stream = nullptr;
    }
    this->_opened   = false;
    this->_filesize = 0;

    this->_stream = new (std::nothrow) std::ofstream();
    if (!this->_stream)
        throw (MidonaRuntimeError(EALLOC_ERROR, "ElfBuilder file stream"));
    this->_stream->open(this->_filepath,
                        std::ios::out | std::ios::trunc | std::ios::binary);
    if (!this->_stream->is_open())
        throw (MidonaRuntimeError(EOPEN_FILE, this->_filepath));
    this->_opened = true;
}

bool    ElfBuilder::writeAt(u64_t offset, void *buff, std::size_t size)
{
    if (!buff || !this->_opened || !this->_stream)
        return (false);
    this->_stream->clear();
    this->_stream->seekp(offset, std::ios::beg);
    if (this->_stream->fail()) return (false);
    this->_stream->write(reinterpret_cast<char *>(buff), size);
    if (this->_stream->fail()) return (false);
    if (static_cast<std::size_t>(offset) + size > this->_filesize)
        this->_filesize = static_cast<std::size_t>(offset) + size;
    return (true);
}

bool    ElfBuilder::createFile(std::string &filepath)
{
    this->_filepath = filepath;
    return (this->createFile());
}

std::size_t ElfBuilder::getFilesize(void) const
{ return (this->_filesize); }

bool    ElfBuilder::createFile(void)
{
    this->_open();

    const std::size_t file_end = this->_computeLayout();

    std::size_t shdr_off;
    if (this->_ElfHeader.e_shoff != 0 && this->_ElfHeader.e_shoff >= file_end)
        shdr_off = this->_ElfHeader.e_shoff;
    else
        shdr_off = (file_end + 7) & ~7ULL;

    Elf64_Ehdr ehdr = this->_ElfHeader;
    if (!this->_ProgramHeader.empty() && ehdr.e_phoff == 0)
        ehdr.e_phoff = sizeof(Elf64_Ehdr);
    ehdr.e_phnum = static_cast<Elf64_Half>(this->_ProgramHeader.size());
    ehdr.e_shoff = shdr_off;
    ehdr.e_shnum = static_cast<Elf64_Half>(this->_SectionHeader.size());

    if (!this->writeAt(0, &ehdr, sizeof(ehdr)))
        throw (MidonaRuntimeError("ElfBuilder: failed to write ELF header"));

    for (std::size_t i = 0; i < this->_ProgramHeader.size(); ++i)
        if (!this->writeAt(ehdr.e_phoff + i * sizeof(Elf64_Phdr),
                           &this->_ProgramHeader[i], sizeof(Elf64_Phdr)))
            throw (MidonaRuntimeError("ElfBuilder: failed to write phdr"));

    for (std::size_t i = 0; i < this->_SectionHeader.size(); ++i)
    {
        const Elf64_Shdr &s = this->_SectionHeader[i];
        if (s.sh_type == SHT_NULL || s.sh_type == SHT_NOBITS) continue;
        if (this->_SectionData[i].empty()) continue;
        if (!this->writeAt(s.sh_offset,
                           this->_SectionData[i].data(),
                           this->_SectionData[i].size()))
            throw (MidonaRuntimeError("ElfBuilder: failed to write section data"));
    }

    for (std::size_t i = 0; i < this->_SectionHeader.size(); ++i)
        if (!this->writeAt(shdr_off + i * sizeof(Elf64_Shdr),
                           &this->_SectionHeader[i], sizeof(Elf64_Shdr)))
            throw (MidonaRuntimeError("ElfBuilder: failed to write shdr"));

    this->_ElfHeader = ehdr;
    this->_filesize  = shdr_off
                     + this->_SectionHeader.size() * sizeof(Elf64_Shdr);

    this->_stream->flush();
    this->_stream->close();
    this->_opened = false;

    if (chmod(this->_filepath.c_str(),
              S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH) == -1)
        throw (MidonaRuntimeError("Failed to change %s permission",
                                  this->_filepath.c_str()));
    return (true);
}
