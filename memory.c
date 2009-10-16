#include <stdio.h>
#include <malloc.h>
#include "types.h"
#include "memory.h"
#include "emulator.h"

mipsMappedMemory *rootBank = NULL;
mipsMappedMemory *mappedBanks = NULL;

int mapMemory(u32 start, u32 size, u8 flags)
{
	mipsMappedMemory *newMappedMem = malloc(sizeof(mipsMappedMemory));

	if (checkBankOverlap(start, size))
	{
		return -1;
	}
	
	newMappedMem->addrStart = start;
	newMappedMem->addrEnd = start + size;
	newMappedMem->flags = flags;
	newMappedMem->memory = calloc(1, size);
	
	if (newMappedMem->memory == NULL)
	{
		printf("Out of memory\n");
		return -2;
	}
	
#ifdef DEBUG
	printf("Mapped bank from %#x to %#x wich points to %p\n", newMappedMem->addrStart, newMappedMem->addrEnd, newMappedMem->memory);
#endif		
	
	newMappedMem->next = NULL;
	
	if (mappedBanks == NULL)
	{
		mappedBanks = newMappedMem;
		mappedBanks->next = NULL;
		rootBank = mappedBanks;
	} else
	{
		mappedBanks->next = newMappedMem;
		mappedBanks = newMappedMem;
		mappedBanks->next = NULL;
	}
	
	return 1;
}

int checkBankOverlap(u32 address, u32 size)
{
	mipsMappedMemory *ptr = rootBank;
	
	while (ptr != NULL)
	{
		if (address >= ptr->addrStart)
		{
			if (address + size <= ptr->addrEnd)
			{
				return 1;
			}
		}
		
		ptr = ptr->next;
	}
	
	return 0;
}

mipsMappedMemory *getBank(u32 address, u32 size)
{
	mipsMappedMemory *ptr = rootBank;
#ifdef DEBUG
	printf("Searching bank for address %#x\n", address);
#endif	
	while (ptr != NULL)
	{
		if (address >= ptr->addrStart)
		{
			if (address + size <= ptr->addrEnd)
			{
				return ptr;
			}
		}
		ptr = ptr->next;
	}
		
	return NULL;
}	

void changeFlag(u32 address, u8 newFlag)
{
	mipsMappedMemory *ptr = getBank(address, 0);
	ptr->flags = newFlag;
}
	
void unmapMemory()
{
	mipsMappedMemory *ptr = rootBank;
	
	while (ptr != NULL)
	{
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

s8 readByte(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s8));
	return bank->memory[address ^ bank->addrStart];
}

void writeByte(u32 address, s8 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s8));
	bank->memory[address ^ bank->addrStart] = value;
}

/*
 * Read access.
 */

s8 readByteLE(u32 address)
{
	return readByte(address);
}

s16 readHwordLE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s16));
	return bank->memory[address ^ bank->addrStart    ] <<  8 |
	       bank->memory[address ^ bank->addrStart + 1];
}

s32 readWordLE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s32));
	return bank->memory[address ^ bank->addrStart    ] << 24 |
	       bank->memory[address ^ bank->addrStart + 1] << 16 |
	       bank->memory[address ^ bank->addrStart + 2] <<  8 |
	       bank->memory[address ^ bank->addrStart + 3];
}

s64 readDwordLE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s64));
	return bank->memory[address ^ bank->addrStart    ] << 56 |
	       bank->memory[address ^ bank->addrStart + 1] << 48 |
	       bank->memory[address ^ bank->addrStart + 2] << 40 |
	       bank->memory[address ^ bank->addrStart + 3] << 32 |
	       bank->memory[address ^ bank->addrStart + 4] << 24 |
	       bank->memory[address ^ bank->addrStart + 5] << 16 |
	       bank->memory[address ^ bank->addrStart + 6] <<  8 |
	       bank->memory[address ^ bank->addrStart + 7];	
}

s8 readByteBE(u32 address)
{
	return readByte(address);
}

s16 readHwordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s16));
	return bank->memory[address ^ bank->addrStart + 1] <<  8 |
	       bank->memory[address ^ bank->addrStart    ];
}

s32 readWordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s64));
	return bank->memory[address ^ bank->addrStart + 3] << 24 |
	       bank->memory[address ^ bank->addrStart + 2] << 16 |
	       bank->memory[address ^ bank->addrStart + 1] <<  8 |
	       bank->memory[address ^ bank->addrStart    ];
}

s64 readDwordBE(u32 address)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s64));
	return bank->memory[address ^ bank->addrStart + 7] << 56 |
	       bank->memory[address ^ bank->addrStart + 6] << 48 |
	       bank->memory[address ^ bank->addrStart + 5] << 40 |
	       bank->memory[address ^ bank->addrStart + 4] << 32 |
	       bank->memory[address ^ bank->addrStart + 3] << 24 |
	       bank->memory[address ^ bank->addrStart + 2] << 16 |
	       bank->memory[address ^ bank->addrStart + 1] <<  8 |
	       bank->memory[address ^ bank->addrStart    ];	
}

/*
 * Write access.
 */

void writeByteLE(u32 address, s8 value)
{
	writeByte(address, value);
}

void writeHwordLE(u32 address, s16 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s16));
	bank->memory[address ^ bank->addrStart    ] = (s8)(value >> 8);
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value);
}

void writeWordLE(u32 address, s32 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s32));
	bank->memory[address ^ bank->addrStart    ] = (s8)(value >> 24);
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value >> 16);
	bank->memory[address ^ bank->addrStart + 2] = (s8)(value >> 8 );
	bank->memory[address ^ bank->addrStart + 3] = (s8)(value);
}

void writeDwordLE(u32 address, s64 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s64));
	bank->memory[address ^ bank->addrStart    ] = (s8)(value >> 56);
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value >> 48);
	bank->memory[address ^ bank->addrStart + 2] = (s8)(value >> 40);
	bank->memory[address ^ bank->addrStart + 3] = (s8)(value >> 32);
	bank->memory[address ^ bank->addrStart + 4] = (s8)(value >> 24);
	bank->memory[address ^ bank->addrStart + 5] = (s8)(value >> 16);
	bank->memory[address ^ bank->addrStart + 6] = (s8)(value >> 8 );
	bank->memory[address ^ bank->addrStart + 7] = (s8)(value);	
}

void writeByteBE(u32 address, s8 value)
{
	writeByte(address, value);
}

void writeHwordBE(u32 address, s16 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s16));
	bank->memory[address ^ bank->addrStart    ] = (s8)(value);
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value >> 8);
}

void writeWordBE(u32 address, s32 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s32));
	bank->memory[address ^ bank->addrStart    ] = (s8)(value)      ;
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value >> 0) ;
	bank->memory[address ^ bank->addrStart + 2] = (s8)(value >> 16);
	bank->memory[address ^ bank->addrStart + 3] = (s8)(value >> 24);
}

void writeDwordBE(u32 address, s64 value)
{
	mipsMappedMemory *bank = getBank(address, sizeof(s64));
	bank->memory[address ^ bank->addrStart + 7] = (s8)(value >> 56);
	bank->memory[address ^ bank->addrStart + 6] = (s8)(value >> 48);
	bank->memory[address ^ bank->addrStart + 5] = (s8)(value >> 40);
	bank->memory[address ^ bank->addrStart + 4] = (s8)(value >> 32);
	bank->memory[address ^ bank->addrStart + 3] = (s8)(value >> 24);
	bank->memory[address ^ bank->addrStart + 2] = (s8)(value >> 16);
	bank->memory[address ^ bank->addrStart + 1] = (s8)(value >> 8 );
	bank->memory[address ^ bank->addrStart    ] = (s8)(value);	
}

/*
 * Simple memory copy.
 */

void memcopy(void *src, u32 address, int size)
{
	int x;
	
	for (x = 0; x < size; x++)
	{
		writeByte(address + x, *(u8*)(src + x));
	}
}

void memoryset(u32 address, s8 fill, int size)
{
	int x;
	
	for (x = 0; x < size; x++)
	{
		writeByte(address + x, fill);
	}
}
