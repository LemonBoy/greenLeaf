#ifndef _MEMORY_H
#define _MEMORY_H

#include "types.h"
#include "emulator.h"

/* Defines */

#define FLAG_ROM 0x1
#define FLAG_RAM 0x2

/* Structs */

typedef struct _mipsMappedMemory {
	u32 addrStart;
	u32 addrEnd;
	u32 size;
	u8  flags;
	u64* memory;
	struct _mipsMappedMemory *next;
} mipsMappedMemory;

/* Prototypes */

int mapMemory    (mipsCpu* cpu, u32 start, u32 size, u8 flags);
void changeFlag  (mipsCpu* cpu, u32 address, u8 newFlag);
void unmapMemory (mipsCpu* cpu);

void memcopy     (mipsCpu* cpu, void *src, u32 address, int size);
void memoryset   (mipsCpu* cpu, u32 address, u8 fill, int size);

u8  readByteLE   (mipsCpu* cpu, u32 address);
u16 readHwordLE  (mipsCpu* cpu, u32 address);
u32 readWordLE   (mipsCpu* cpu, u32 address);
u64 readDwordLE  (mipsCpu* cpu, u32 address);

u8  readByteBE   (mipsCpu* cpu, u32 address);
u16 readHwordBE  (mipsCpu* cpu, u32 address);
u32 readWordBE   (mipsCpu* cpu, u32 address);
u64 readDwordBE  (mipsCpu* cpu, u32 address);

void writeByteLE (mipsCpu* cpu, u32 address, u8  value);
void writeHwordLE(mipsCpu* cpu, u32 address, u16 value);
void writeWordLE (mipsCpu* cpu, u32 address, u32 value);
void writeDwordLE(mipsCpu* cpu, u32 address, u64 value);

void writeByteBE (mipsCpu* cpu, u32 address, u8  value);
void writeHwordBE(mipsCpu* cpu, u32 address, u16 value);
void writeWordBE (mipsCpu* cpu, u32 address, u32 value);
void writeDwordBE(mipsCpu* cpu, u32 address, u64 value);

#endif
