
<div align="center">
	<h1>ELF</h1>
</div>

Since Midona is a ELF crypter, the program need to read the original file to crypt and write the new binary file by himself.

<div align="center">
	<h2>Keys section</h2>
</div>

When encrypt the content of the ELF binary, AES-256 need to use a key to decrypt the original content. This key is splitted on different non loadable section of the binary. Each key fragments section is 32 bytes wide. They follow this:
```
Magic number¹ (8 bytes)
     |
     v
Identifier² (4 bytes + 4 bytes of padding)
     |
     v
Decrypted fragments hash³ (8 bytes)
     |
     v
Encrypted fragments (4 bytes max)
```
1. The magic number is used to know if the section is a key framgents section
2. The identifier is use to know each part of the key the fragments is
3. The hash is used to check if the key fragments was correctly decrypted

A C struct is on `include/ELF/MidonaStruct.hpp` with the section layout to not have to parse by hand. Just give the structure to a .read() function 
```
typedef struct s_midona_key
{
	s_midona_key(void) : magic{0}, number_fragments(0), reserved(0), hash(0) {};

	/* An array with magic number content */
	u8_t	magic[8];
	/* The number of the key fragments */
	u32_t	number_fragments;
	u32_t	reserved; // HARD CODED PADDING, IS ALWAYS SET TO 0
	/* The hash of the decrypted key fragments */
	u64_t	hash;
} MidonaKey;
```

During the execution of the crypted program, the key will be search on his own binary by reading `/proc/self/exe` and parsing his own ELF data

<div align="center">
	<h2>Keys section</h2>
</div>
