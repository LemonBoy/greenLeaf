#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "memory.h"
#include "emulator.h"

mipsMappedMemory *rootBank    = NULL;
mipsMappedMemory *mappedBanks = NULL;

int checkBankOverlap(u32 address, u32 size);

int mapMemory(u32 start, u32 size, u8 flags)
{
	mipsMappedMemory *newMappedMem = malloc(sizeof(mipsMappedMemory));

	if(checkBankOverlap(start, size))
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
	if(mappedBanks == NULL) {
		mappedBanks = newMappedMem;
		mappedBanks->next = NULL;
		rootBank = mappedBanks;
	}else{
		mappedBanks->next = newMappedMem;
		mappedBanks = newMappedMem;
		mappedBanks->next = NULL;
	}
	
	return 1;
}

int checkBankOverlap(u32 address, u32 size)
{
	mipsMappedMemory *ptr = rootBank;
	
	while(ptr != NULL) {
		if(address >= ptr->addrStart)
			if (address + size <= ptr->addrEnd)
				return 1;
		ptr = ptr->next;
	}
	
	return 0;
}

mipsMappedMemory *getBank(u32 address, u32 size, int access)
{
	mipsMappedMemory *ptr = rootBank;
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
						return ptr;
				}
			}
		}
		ptr = ptr->next;
	}
		
	return NULL;
}	

void changeFlag(u32 address, u8 newFlag)
{
	mipsMappedMemory *ptr = getBank(address, 0, 0);
	ptr->flags = newFlag;
}
	
void unmapMemory()
{
	mipsMappedMemory *ptr = rootBank;
	
	while (ptr != NULL) {
		free(ptr->memory);		
		ptr = ptr->next;
	}
	
	free(rootBank);
}

/*
 * Functions for memory access.
 */

/*
 * Not endian functions.
 */

u8 readByte(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u8), 1);
	if (bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(4, 0);
		exit(1);
	}
	return bank->memory[(address - bank->addrStart)];
}

void writeByte(u32 address, u8 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u8), 2);
	if (bank == NULL) {
		printf("EXCEPTION : %s(%#x)\n", __FUNCTION__, address);
		generateException(5, 0);
		exit(1);
	}	
	bank->memory[(address - bank->addrStart)] = value;
}

/*
 * Read access.
 */

u8 readByteLE(u32 address)
{
	return readByte(address);
}

u16 readHwordLE(u32 address)
{
	return	(readByte(address) <<  8) |
		(readByte(address + 1) <<  0) ;
}

u32 readWordLE(u32 address)
{
	return	(readByte(address) << 24) |
		(readByte(address + 1) << 16) |
		(readByte(address + 2) <<  8) |
		(readByte(address + 3) <<  0) ;
}

u64 readDwordLE(u32 address)
{
	return	((u64)readByte(address) << 56) |
		((u64)readByte(address + 1) << 48) |
		((u64)readByte(address + 2) << 40) |
		((u64)readByte(address + 3) << 32) |
		((u64)readByte(address + 4) << 24) |
		((u64)readByte(address + 5) << 16) |
		((u64)readByte(address + 6) <<  8) |
		((u64)readByte(address + 7) <<  0) ;	
}

u8 readByteBE(u32 address)
{
	return readByte(address);
}

u16 readHwordBE(u32 address)
{
	return	(readByte(address + 1) <<  8) |
		(readByte(address) <<  0);
}

u32 readWordBE(u32 address)
{
	return	(readByte(address + 3) << 24) |
		(readByte(address + 2) << 16) |
		(readByte(address + 1) <<  8) |
		(readByte(address) <<  0) ;
}

u64 readDwordBE(u32 address)
{
	return	((u64)readByte(address + 7) << 56) |
		((u64)readByte(address + 6) << 48) |
		((u64)readByte(address + 5) << 40) |
		((u64)readByte(address + 4) << 32) |
		((u64)readByte(address + 3) << 24) |
		((u64)readByte(address + 2) << 16) |
		((u64)readByte(address + 1) <<  8) |
		((u64)readByte(address) <<  0) ;	
}

/*
 * Write access.
 */

void writeByteLE(u32 address, u8 value)
{
	writeByte(address, value);
}

void writeHwordLE(u32 address, u16 value)
{
	writeByte(address, (u8)(value >> 8));
	writeByte(address + 1, (u8)(value >> 0));
}

void writeWordLE(u32 address, u32 value)
{
	writeByte(address, (u8)(value >> 24));
	writeByte(address + 1, (u8)(value >> 16));
	writeByte(address + 2, (u8)(value >> 8 ));
	writeByte(address + 3, (u8)(value >> 0 ));
}

void writeDwordLE(u32 address, u64 value)
{
	writeByte(address, (u8)(value >> 56));
	writeByte(address + 1, (u8)(value >> 48));
	writeByte(address + 2, (u8)(value >> 40));
	writeByte(address + 3, (u8)(value >> 32));
	writeByte(address + 4, (u8)(value >> 24));
	writeByte(address + 5, (u8)(value >> 16));
	writeByte(address + 6, (u8)(value >> 8 ));
	writeByte(address + 7, (u8)(value >> 0 ));	
}

void writeByteBE(u32 address, u8 value)
{
	writeByte(address, value);
}

void writeHwordBE(u32 address, u16 value)
{
	writeByte(address + 1, (u8)(value >> 8));
	writeByte(address, (u8)(value >> 0));
}

void writeWordBE(u32 address, u32 value)
{
	writeByte(address + 3, (u8)(value >> 24));
	writeByte(address + 2, (u8)(value >> 16));
	writeByte(address + 1, (u8)(value >> 8 ));
	writeByte(address, (u8)(value >> 0 ));
}

void writeDwordBE(u32 address, u64 value)
{
	writeByte(address + 7, (u8)(value >> 56));
	writeByte(address + 6, (u8)(value >> 48));
	writeByte(address + 5, (u8)(value >> 40));
	writeByte(address + 4, (u8)(value >> 32));
	writeByte(address + 3, (u8)(value >> 24));
	writeByte(address + 2, (u8)(value >> 16));
	writeByte(address + 1, (u8)(value >> 8 ));
	writeByte(address, (u8)(value >> 0 ));	
}

/*
 * Simple memory copy.
 */

void memcopy(void *src, u32 address, int size)
{
	int x;
	u8* s = src;
	for (x = 0; x < size; x++, s++)
		writeByte(address + x, *s);
}

void memoryset(u32 address, u8 fill, int size)
{
	int x;
	
	for (x = 0; x < size; x++, address++)
		writeByte(address, fill);
}
