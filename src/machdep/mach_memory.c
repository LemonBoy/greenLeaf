#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "memory.h"
#include "emulator.h"

#include "machdep/mach_memory.h"

/*
 * Endian-independant functions.
 */

u8 readByte(mipsCpu* cpu, u32 address)
{
	mipsMappedMemory *bank = getBank(cpu, address, sizeof(u8), 1);
	if (bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(cpu, EXCEPTION_BADADDR_LOAD, 0);
		exit(1);
	}
	int addr = (address ^ bank->addrStart);
#ifdef DEBUG
#ifdef HYPERDEBUG
	printf("Address: 0x%08X\n", addr);
#endif
#endif
	return bank->memory[addr];
}

void writeByte(mipsCpu* cpu, u32 address, u8 value)
{
	mipsMappedMemory *bank = getBank(cpu, address, sizeof(u8), 2);
	if(bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(cpu, EXCEPTION_BADADDR_STORE, 0);
		exit(1);
	}
	int addr = (address ^ bank->addrStart);
#ifdef DEBUG
#ifdef HYPERDEBUG
	printf("Address: 0x%08X\n", addr);
#endif
#endif
	bank->memory[addr] = value;
}

/*
 * Read access.
 */

u8 readByteLE(mipsCpu* cpu, u32 address)
{
	return readByte(cpu, address);
}

u16 readHwordLE(mipsCpu* cpu, u32 address)
{
	return	(readByte(cpu, address + 0) <<  8) |
		(readByte(cpu, address + 1) <<  0) ;
}

u32 readWordLE(mipsCpu* cpu, u32 address)
{
	return	(readByte(cpu, address + 0) << 24) |
		(readByte(cpu, address + 1) << 16) |
		(readByte(cpu, address + 2) <<  8) |
		(readByte(cpu, address + 3) <<  0) ;
}

u64 readDwordLE(mipsCpu* cpu, u32 address)
{
	return	((u64)readByte(cpu, address + 0) << 56) |
		((u64)readByte(cpu, address + 1) << 48) |
		((u64)readByte(cpu, address + 2) << 40) |
		((u64)readByte(cpu, address + 3) << 32) |
		((u64)readByte(cpu, address + 4) << 24) |
		((u64)readByte(cpu, address + 5) << 16) |
		((u64)readByte(cpu, address + 6) <<  8) |
		((u64)readByte(cpu, address + 7) <<  0) ;	
}

u8 readByteBE(mipsCpu* cpu, u32 address)
{
	return readByte(cpu, address);
}

u16 readHwordBE(mipsCpu* cpu, u32 address)
{
	return	(readByte(cpu, address + 1) <<  8) |
		(readByte(cpu, address + 0) <<  0);
}

u32 readWordBE(mipsCpu* cpu, u32 address)
{
	return	(readByte(cpu, address + 3) << 24) |
		(readByte(cpu, address + 2) << 16) |
		(readByte(cpu, address + 1) <<  8) |
		(readByte(cpu, address + 0) <<  0) ;
}

u64 readDwordBE(mipsCpu* cpu, u32 address)
{
	return	((u64)readByte(cpu, address + 7) << 56) |
		((u64)readByte(cpu, address + 6) << 48) |
		((u64)readByte(cpu, address + 5) << 40) |
		((u64)readByte(cpu, address + 4) << 32) |
		((u64)readByte(cpu, address + 3) << 24) |
		((u64)readByte(cpu, address + 2) << 16) |
		((u64)readByte(cpu, address + 1) <<  8) |
		((u64)readByte(cpu, address + 0) <<  0) ;	
}

u32 readOpcode(mipsCpu* cpu, u32 address)
{
	return readWordBE(cpu, address);
}

/*
 * Write access.
 */

void writeByteLE(mipsCpu* cpu, u32 address, u8 value)
{
	writeByte(cpu, address, value);
}

void writeHwordLE(mipsCpu* cpu, u32 address, u16 value)
{
	writeByte(cpu, address + 0, (u8)(value >> 8));
	writeByte(cpu, address + 1, (u8)(value >> 0));
}

void writeWordLE(mipsCpu* cpu, u32 address, u32 value)
{
	writeByte(cpu, address + 0, (u8)(value >> 24));
	writeByte(cpu, address + 1, (u8)(value >> 16));
	writeByte(cpu, address + 2, (u8)(value >> 8 ));
	writeByte(cpu, address + 3, (u8)(value >> 0 ));
}

void writeDwordLE(mipsCpu* cpu, u32 address, u64 value)
{
	writeByte(cpu, address + 0, (u8)(value >> 56));
	writeByte(cpu, address + 1, (u8)(value >> 48));
	writeByte(cpu, address + 2, (u8)(value >> 40));
	writeByte(cpu, address + 3, (u8)(value >> 32));
	writeByte(cpu, address + 4, (u8)(value >> 24));
	writeByte(cpu, address + 5, (u8)(value >> 16));
	writeByte(cpu, address + 6, (u8)(value >> 8 ));
	writeByte(cpu, address + 7, (u8)(value >> 0 ));	
}

void writeByteBE(mipsCpu* cpu, u32 address, u8 value)
{
	writeByte(cpu, address, value);
}

void writeHwordBE(mipsCpu* cpu, u32 address, u16 value)
{
	writeByte(cpu, address + 1, (u8)(value >> 8));
	writeByte(cpu, address + 0, (u8)(value >> 0));
}

void writeWordBE(mipsCpu* cpu, u32 address, u32 value)
{
	writeByte(cpu, address + 3, (u8)(value >> 24));
	writeByte(cpu, address + 2, (u8)(value >> 16));
	writeByte(cpu, address + 1, (u8)(value >> 8 ));
	writeByte(cpu, address + 0, (u8)(value >> 0 ));
}

void writeDwordBE(mipsCpu* cpu, u32 address, u64 value)
{
	writeByte(cpu, address + 7, (u8)(value >> 56));
	writeByte(cpu, address + 6, (u8)(value >> 48));
	writeByte(cpu, address + 5, (u8)(value >> 40));
	writeByte(cpu, address + 4, (u8)(value >> 32));
	writeByte(cpu, address + 3, (u8)(value >> 24));
	writeByte(cpu, address + 2, (u8)(value >> 16));
	writeByte(cpu, address + 1, (u8)(value >> 8 ));
	writeByte(cpu, address + 0, (u8)(value >> 0 ));	
}

/*
 * Memory functions
 */

void memcopy(mipsCpu* cpu, void *src, u32 address, int size)
{
	u32 x;
	u8* s = src;
	for (x = 0; x < size; x++, s++)
		writeByte(cpu, address + x, *s);
}

void memoryset(mipsCpu* cpu, u32 address, u8 fill, int size)
{
	int x;
	
	for (x = 0; x < size; x++, address++)
		writeByte(cpu, address, fill);
}
