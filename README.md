<div align="center">
	<h1>Midona</h1>
	<a href="https://github.com/Gun8hoot/Midona"><img src="https://github.com/Gun8hoot/Midona/blob/923228249fff62a15e3cafdb5298342fa4103c70/assets/ascii_art.png"></a>
</div>
<br>
<div align="center">
	<h2>Warning : </h2>
	<p>I (Nathan "Gun8hoot" CLAVEL) am not in charge of the bad/good usage or the modification of my program.</p>
	<p>I've created this program for education purposes only and it should not be used to evade any protection.</p>
</div>
<div align="center">
	<h1>Description : </h1>
</div>

Midona is a [runtime packer](https://en.wikipedia.org/wiki/Executable_compression) that can both compress and/or encrypt an linux [ELF executable](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format). The decompression and decryption will be completely done at the runtime in memory without exposing the clear code of the binary on the disk. The name __Midona__ come from a game that i played when i was a child who is called [The Legend of Zelda: Twilight Princess](https://en.wikipedia.org/wiki/The_Legend_of_Zelda%3A_Twilight_Princess).  

<div align="center">
	<h1>Usage : </h1>
</div>

1. Clone the repository on your computer and go inside
```sh
git clone git@github.com:Gun8hoot/Midona.git && cd Midona
```
2. Compile the program using [GNU Make](https://www.gnu.org/software/make/)
```sh
make
```
3. Use the program
```sh
# Example:
./midona -b compiled_file
```
- When compiling the program, some [object file](https://stackoverflow.com/questions/7718299/whats-an-object-file-in-c) are generated in the **.objects** directory.<br>You can remove it easly by running :
```sh
make clean
```

<div align="center">
	<h1>Known issues : </h1>
</div>

- None for now

<div align="center">
	<h1>Roadmap : </h1>
</div>

Done at : ~3%

- [ ] : Create the best program i can do with the maximum of quality
- [x] : Parse arguments with their corresponding flags
	- [x] : (optional) Add -o flag to know where the compressed/encrypted executable should be placed/named
- [ ] : Being able to compress data
	- [x] : Choose a good compression algorithm
	- [ ] : Implement the algorithm
- [ ] : Being able to encrypt data
	- [ ] : Implement AES256
- [ ] : Being able to decompress and decrypt the binary from the [cli](https://en.wikipedia.org/wiki/Command-line_interface)
	- [ ] Decompression ? 
	- [ ] Decryption ? 
- [ ] : Being able to decompress and decrypt data at the runtime 
	- [ ] Decompression ? 
	- [ ] Decryption ? 
- [ ] : Being able to execute the decompressed/decrypted stub on the memory
- [ ] : (optional) Change the signature every time the compressed binary is executed
- [ ] : (optional) Adapt the stub to support both 64 and 32 bits
- [ ] : (optional) Add more CLI flags to add more control of how the program is compress/encrypted

<div align="center">
	<h1>Technical description : </h1>
</div>

The program is written in **C++11** using Linux, without including any external library other than C++ standard library and glibc. To compress data, i have chosen **LZ4** because it is pretty well documented and i find that this compression algorithm has a good ratio between compression and speed. For the encryption i will probably use **AES256-GCM** to get both confidentiality and authenticity. The compression and encryption is done on a single thread so it can take a lot of time depending of size of the content to encrypt/compress.

<div align="center">
	<h1>Ressources used : </h1>
</div>

- [My notes](https://github.com/Gun8hoot/notes)
- [How ELF work part.1](https://aleeamini.com/elfs-story-part1/) <sup>[[mirror]](https://web.archive.org/web/20260721164905/https://aleeamini.com/elfs-story-part1/)</sup>
- [How ELF work part.2](https://aleeamini.com/elfs-story-part2/) <sup>[[mirror]](https://web.archive.org/web/20260721165025/https://aleeamini.com/elfs-story-part2-elf-structure-elf-header/)</sup>
- [How ELF work part.3](https://aleeamini.com/elfs-story-part3/) <sup>[[mirror]](https://web.archive.org/web/20260721165326/https://aleeamini.com/elfs-story-part3-elfs-structure-elf-section-headers/)</sup>
- [Executing ELF in memory](https://towardsdev.com/memfd-create-fileless-execution-linux-elf-in-memory-28422d6bcbef)
- [LZ4 Algorithm](https://deepwiki.com/lz4/lz4/6.2-lz4-frame-format) <sup>[[mirror]](https://web.archive.org/web/20260324072508/https://deepwiki.com/lz4/lz4/6.2-lz4-frame-format)</sup>
