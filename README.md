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

Midona is a [runtime packer](https://en.wikipedia.org/wiki/Executable_compression) that can both compress and/or encrypt an linux [ELF executable](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format). The decompression and decryption will be completely done at the runtime in memory without leaving any persistant (???) on the disk.

<div align="center">
	<h1>Usage : </h1>
</div>

1. Clone the repository on your computer and go inside
```sh
git clone git@github.com:Gun8hoot/Midona.git && cd Midona
```
2. Compile the program using GNU Make
```sh
make
```
3. Use the program
```sh
# Example:
./midona -b compiled_file
```

<div align="center">
	<h1>Roadmap : </h1>
</div>

Done at : ~3%

- [ ] : Create the best program i can do with the maximum of quality
- [x] : Parse arguments with their corresponding flags
	- [ ] : (optional) Add -o flag to know where the compressed/encrypted executable should be placed/named
- [ ] : Being able to compress data
	- [ ] : Choose a good compression algorithm
	- [ ] : Implement the algorithm
- [ ] : Being able to encrypt data
	- [ ] : Implement AES256
- [ ] : Being able to decompress and decrypt data at the runtime 
- [ ] : Being able to execute the decompressed/decrypted stub on the memory
- [ ] : (optional) Change the signature every time the compressed binary is executed


<div align="center">
	<h1>Ressources used : </h1>
</div>

- [How ELF work part.1](https://aleeamini.com/elfs-story-part1/) <sup>[[mirror]](https://web.archive.org/web/20260721164905/https://aleeamini.com/elfs-story-part1/)</sup>
- [How ELF work part.2](https://aleeamini.com/elfs-story-part2/) <sup>[[mirror]](https://web.archive.org/web/20260721165025/https://aleeamini.com/elfs-story-part2-elf-structure-elf-header/)</sup>
- [How ELF work part.3](https://aleeamini.com/elfs-story-part3/) <sup>[[mirror]](https://web.archive.org/web/20260721165326/https://aleeamini.com/elfs-story-part3-elfs-structure-elf-section-headers/)</sup>
- [Executing ELF in memory](https://towardsdev.com/memfd-create-fileless-execution-linux-elf-in-memory-28422d6bcbef)
