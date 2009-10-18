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
	printf( "Mapped bank from %#x to %#x wich points to %p\n", \
		newMappedMem->addrStart, newMappedMem->addrEnd, newMappedMem->memory);
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
	printf("Searching bank for address %#x\n", address);
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
	return bank->memory[(address ^ bank->addrStart)];
}

void writeByte(u32 address, u8 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u8), 2);
	bank->memory[(address ^ bank->addrStart)] = value;
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
	mipsMappedMemory *bank = getBank(address, sizeof(u16), 1);
	return	(bank->memory[(address ^ bank->addrStart)    ] <<  8) |
		(bank->memory[(address ^ bank->addrStart) + 1] <<  0) ;
}

u32 readWordLE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u32), 1);
	return	(bank->memory[(address ^ bank->addrStart)    ] << 24) |
		(bank->memory[(address ^ bank->addrStart) + 1] << 16) |
		(bank->memory[(address ^ bank->addrStart) + 2] <<  8) |
		(bank->memory[(address ^ bank->addrStart) + 3] <<  0) ;
}

u64 readDwordLE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u64), 1);
	return	(bank->memory[(address ^ bank->addrStart)    ] << 56) |
		(bank->memory[(address ^ bank->addrStart) + 1] << 48) |
		(bank->memory[(address ^ bank->addrStart) + 2] << 40) |
		(bank->memory[(address ^ bank->addrStart) + 3] << 32) |
		(bank->memory[(address ^ bank->addrStart) + 4] << 24) |
		(bank->memory[(address ^ bank->addrStart) + 5] << 16) |
		(bank->memory[(address ^ bank->addrStart) + 6] <<  8) |
		(bank->memory[(address ^ bank->addrStart) + 7] <<  0) ;	
}

u8 readByteBE(u32 address)
{
	return readByte(address);
}

u16 readHwordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u16), 1);
	return	(bank->memory[(address ^ bank->addrStart) + 1] <<  8) |
		(bank->memory[(address ^ bank->addrStart)    ] <<  0);
}

u32 readWordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u32), 1);
	return	(bank->memory[(address ^ bank->addrStart) + 3] << 24) |
		(bank->memory[(address ^ bank->addrStart) + 2] << 16) |
		(bank->memory[(address ^ bank->addrStart) + 1] <<  8) |
		(bank->memory[(address ^ bank->addrStart)    ] <<  0) ;
}

u64 readDwordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u64), 1);
	return	(bank->memory[(address ^ bank->addrStart) + 7] << 56) |
		(bank->memory[(address ^ bank->addrStart) + 6] << 48) |
		(bank->memory[(address ^ bank->addrStart) + 5] << 40) |
		(bank->memory[(address ^ bank->addrStart) + 4] << 32) |
		(bank->memory[(address ^ bank->addrStart) + 3] << 24) |
		(bank->memory[(address ^ bank->addrStart) + 2] << 16) |
		(bank->memory[(address ^ bank->addrStart) + 1] <<  8) |
		(bank->memory[(address ^ bank->addrStart)    ] <<  0) ;	
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
	mipsMappedMemory *bank = getBank(address, sizeof(u16), 2);
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 8);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 0);
}

void writeWordLE(u32 address, u32 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u32), 2);
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 24);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 16);
	bank->memory[(address ^ bank->addrStart) + 2] = (u8)(value >> 8 );
	bank->memory[(address ^ bank->addrStart) + 3] = (u8)(value >> 0 );
}

void writeDwordLE(u32 address, u64 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u64), 2);
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 56);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 48);
	bank->memory[(address ^ bank->addrStart) + 2] = (u8)(value >> 40);
	bank->memory[(address ^ bank->addrStart) + 3] = (u8)(value >> 32);
	bank->memory[(address ^ bank->addrStart) + 4] = (u8)(value >> 24);
	bank->memory[(address ^ bank->addrStart) + 5] = (u8)(value >> 16);
	bank->memory[(address ^ bank->addrStart) + 6] = (u8)(value >> 8 );
	bank->memory[(address ^ bank->addrStart) + 7] = (u8)(value >> 0 );	
}

void writeByteBE(u32 address, u8 value)
{
	writeByte(address, value);
}

void writeHwordBE(u32 address, u16 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u16), 2);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 8);
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 0);
}

void writeWordBE(u32 address, u32 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u32), 2);
	bank->memory[(address ^ bank->addrStart) + 3] = (u8)(value >> 24);
	bank->memory[(address ^ bank->addrStart) + 2] = (u8)(value >> 16);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 8 );
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 0 );
}

void writeDwordBE(u32 address, u64 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(u64), 2);
	bank->memory[(address ^ bank->addrStart) + 7] = (u8)(value >> 56);
	bank->memory[(address ^ bank->addrStart) + 6] = (u8)(value >> 48);
	bank->memory[(address ^ bank->addrStart) + 5] = (u8)(value >> 40);
	bank->memory[(address ^ bank->addrStart) + 4] = (u8)(value >> 32);
	bank->memory[(address ^ bank->addrStart) + 3] = (u8)(value >> 24);
	bank->memory[(address ^ bank->addrStart) + 2] = (u8)(value >> 16);
	bank->memory[(address ^ bank->addrStart) + 1] = (u8)(value >> 8 );
	bank->memory[(address ^ bank->addrStart)    ] = (u8)(value >> 0 );	
}

/*
 * Simple memory copy.
 */

void memcopy(void *src, u32 address, int size)
{
	int x;
	
	for (x = 0; x < size; x++)
		writeByte(address + x, *(u8*)(src + x));
}

void memoryset(u32 address, u8 fill, int size)
{
	int x;
	
	for (x = 0; x < size; x++)
		writeByte(address + x, fill);
}
