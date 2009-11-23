#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "memory.h"
#include "emulator.h"

int checkBankOverlap(mipsCpu* cpu, u32 address, u32 size);

int mapMemory(mipsCpu* cpu, u32 start, u32 size, u8 flags)
{
	mipsMappedMemory *newMappedMem = malloc(sizeof(mipsMappedMemory));

	if(checkBankOverlap(cpu, start, size))
		return -1;
	
	newMappedMem->addrStart = start;
	newMappedMem->addrEnd   = start + size;
	newMappedMem->flags     = flags;
	newMappedMem->memory    = calloc(1, size);
	
	if(newMappedMem->memory == NULL) {
		printf("Out of memory\n");
		return -2;
	}
	
#ifdef DEBUG
	printf("Mapped bank from 0x%08X to 0x%08X which points to 0x%08X\n", \
		newMappedMem->addrStart, newMappedMem->addrEnd, (u32)newMappedMem->memory);
#endif		
	
	newMappedMem->next = NULL;
	if(cpu->mappedBanks == NULL) {
		cpu->mappedBanks = newMappedMem;
		cpu->mappedBanks->next = NULL;
		cpu->rootBank = cpu->mappedBanks;
	}else{
		cpu->mappedBanks->next = newMappedMem;
		cpu->mappedBanks = newMappedMem;
		cpu->mappedBanks->next = NULL;
	}
	
	return 1;
}

int checkBankOverlap(mipsCpu* cpu, u32 address, u32 size)
{
	mipsMappedMemory *ptr = cpu->rootBank;
	
	while(ptr != NULL) {
		if(address >= ptr->addrStart)
			if (address + size <= ptr->addrEnd)
				return 1;
		ptr = ptr->next;
	}
	
	return 0;
}

mipsMappedMemory *getBank(mipsCpu* cpu, u32 address, u32 size, int access)
{
	mipsMappedMemory *ptr = cpu->rootBank;
#ifdef DEBUG
#ifdef HYPERDEBUG
	printf("Searching bank for address 0x%08X\n", address);
#endif
#endif	
	while(ptr != NULL) {
		if(address >= ptr->addrStart) {
			if(address + size <= ptr->addrEnd) {
				switch(access) {
					case 2: /* Write access requested */
						if(ptr->flags & FLAG_ROM)
							break;
					case 1: /* Read access requested */
					default:
#ifdef DEBUG
#ifdef HYPERDEBUG
						printf("Found\n");
#endif
#endif	
						return ptr;
				}
			}
		}
		ptr = ptr->next;
	}
#ifdef DEBUG
#ifdef HYPERDEBUG
	printf("Not found\n");
#endif
#endif	
	return NULL;
}	

void changeFlag(mipsCpu* cpu, u32 address, u8 newFlag)
{
	mipsMappedMemory *ptr = getBank(cpu, address, 0, 0);
	ptr->flags = newFlag;
}
	
void unmapMemory(mipsCpu* cpu)
{
	mipsMappedMemory *ptr = cpu->rootBank;
	
	while(ptr != NULL) {
		free(ptr->memory);
		ptr->memory = NULL;
		ptr = ptr->next;
	}
	
	free(cpu->rootBank);
	cpu->rootBank = NULL;
}

/*
 * Functions for memory access.
 */

/*
 * Not endian functions.
 */

u8 readByte(mipsCpu* cpu, u32 address)
{
	mipsMappedMemory *bank = getBank(cpu, address, sizeof(u8), 1);
	if (bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(cpu, 4, 0);
		exit(1);
	}
	return bank->memory[(address ^ bank->addrStart)];
}

void writeByte(mipsCpu* cpu, u32 address, u8 value)
{
	mipsMappedMemory *bank = getBank(cpu, address, sizeof(u8), 2);
	if (bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(cpu, 5, 0);
		exit(1);
	}	
	bank->memory[(address ^ bank->addrStart)] = value;
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
 * Simple memory copy.
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
