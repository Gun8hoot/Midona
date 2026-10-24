
#pragma once

/* The magic bytes located on the section data to know if this current section is a key fragment */
#define MAGIC_KEY_SECTION { '\xAF', '\x76', '\xBE', '\x13', '\x3C', '\x88', '\xFF', '\x03' }
/* The size in bytes of the key (32 bytes = 256bit) */
#define KEY_SECTION_SIZE 32
