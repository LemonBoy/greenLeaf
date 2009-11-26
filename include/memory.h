#ifndef _MEMORY_H
#define _MEMORY_H

#include "types.h"
#include "emulator.h"

/* Defines */

#define FLAG_ROM 0x1
#define FLAG_RAM 0x2

/* Prototypes */

int               mapMemory  (mipsCpu* cpu, u32 start, u32 size, u8 flags);
void              changeFlag (mipsCpu* cpu, u32 address, u8 newFlag);
void              unmapMemory(mipsCpu* cpu);

INLINE int checkBankOverlap(mipsCpu* cpu, u32 address, u32 size)
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

INLINE mipsMappedMemory *getBank(mipsCpu* cpu, u32 address, u32 size, int access)
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

#endif
