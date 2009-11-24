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

int               mapMemory       (mipsCpu* cpu, u32 start, u32 size, u8 flags);
void              changeFlag      (mipsCpu* cpu, u32 address, u8 newFlag);
int               checkBankOverlap(mipsCpu* cpu, u32 address, u32 size);
mipsMappedMemory *getBank         (mipsCpu* cpu, u32 address, u32 size, int access);
void              unmapMemory     (mipsCpu* cpu);

#endif
