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
	newMappedMem->size	= size;
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
			if(address + size <= ptr->addrEnd)
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
#ifdef DEBUG
#ifdef HYPERDEBUG
		printf("\n\tBankstart 0x%08X\n", ptr->addrStart);
		printf("\tBankend 0x%08X\n", ptr->addrEnd);
#endif
#endif	
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
						printf("Found. Size: 0x%08X\n", ptr->size);
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
