#ifndef MACH_MEMORY_H
#define MACH_MEMORY_H

#include "types.h"
#include "emulator.h"

void memcopy       (mipsCpu* cpu, void *src, u32 address, int size);
void memcopy_load  (mipsCpu* cpu, void *src, u32 address, int size);
void memoryset     (mipsCpu* cpu, u32 address, u8 fill, int size);
void memoryset_load(mipsCpu* cpu, u32 address, u8 fill, int size);

u8  readByteLE     (mipsCpu* cpu, u32 address);
u16 readHwordLE    (mipsCpu* cpu, u32 address);
u32 readWordLE     (mipsCpu* cpu, u32 address);
u64 readDwordLE    (mipsCpu* cpu, u32 address);

u8  readByteBE     (mipsCpu* cpu, u32 address);
u16 readHwordBE    (mipsCpu* cpu, u32 address);
u32 readWordBE     (mipsCpu* cpu, u32 address);
u64 readDwordBE    (mipsCpu* cpu, u32 address);

u32 readOpcode     (mipsCpu* cpu, u32 address);

void writeByteLE   (mipsCpu* cpu, u32 address, u8  value);
void writeHwordLE  (mipsCpu* cpu, u32 address, u16 value);
void writeWordLE   (mipsCpu* cpu, u32 address, u32 value);
void writeDwordLE  (mipsCpu* cpu, u32 address, u64 value);

void writeByteBE   (mipsCpu* cpu, u32 address, u8  value);
void writeHwordBE  (mipsCpu* cpu, u32 address, u16 value);
void writeWordBE   (mipsCpu* cpu, u32 address, u32 value);
void writeDwordBE  (mipsCpu* cpu, u32 address, u64 value);

#endif
