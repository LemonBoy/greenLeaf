#ifndef _MEMORY_H
#define _MEMORY_H

#include "types.h"

/* Defines */

#define FLAG_ROM 0x1
#define FLAG_RAM 0x2

/* Structs */

typedef struct _mipsMappedMemory
{
	u32 addrStart;
	u32 addrEnd;
	u32 size;
	u8  flags;
	u64* memory;
	struct _mipsMappedMemory *next;
} mipsMappedMemory;

/* Prototypes */

int mapMemory(u32 start, u32 size, u8 flags);
void changeFlag(u32 address, u8 newFlag);
void unmapMemory();

void memcopy(void *src, u32 address, int size);
void memoryset(u32 address, u8 fill, int size);

u8 readByteLE(u32 address);
u16 readHwordLE(u32 address);
u32 readWordLE(u32 address);
u64 readDwordLE(u32 address);

u8 readByteBE(u32 address);
u16 readHwordBE(u32 address);
u32 readWordBE(u32 address);
u64 readDwordBE(u32 address);

void writeByteLE(u32 address, u8 value);
void writeHwordLE(u32 address, u16 value);
void writeWordLE(u32 address, u32 value);
void writeDwordLE(u32 address, u64 value);

void writeByteBE(u32 address, u8 value);
void writeHwordBE(u32 address, u16 value);
void writeWordBE(u32 address, u32 value);
void writeDwordBE(u32 address, u64 value);

#endif
