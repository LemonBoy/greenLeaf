#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "memory.h"
#include "emulator.h"
#include "instructions.h"
#include "dasm.h"

#include "mach_memory.h"

/* 
 * Initialize the cpu emulation core. 
 */

mipsCpu* initializeCPU(u32 stackPtr)
{
	mipsCpu* cpu = calloc(sizeof(mipsCpu), 1);
	int reg, cop;
	
	for(reg = 0; reg < 34; reg++) {
		cpu->r[reg] = ((mipsRegister)0);
		if(reg == 29)
			cpu->r[reg] = ((mipsRegister_u)stackPtr);
	}
	
	for(cop = 0; cop < 3; cop++)
		for(reg = 0; reg < 32; reg++)
			cpu->cr[reg][cop] = ((mipsRegister)0);
	
	cpu->pc = ((mipsRegister)0);
	cpu->nPc = ((mipsRegister)0);
	cpu->bOpcode = ((mipsRegister)0);
	
	cpu->endian = ENDIANNESS_LE;
	cpu->rootBank = NULL;
	cpu->mappedBanks = NULL;
	
#ifdef DEBUG
	printf("Cpu endianess set to little endian\n");
#endif
	cpu->readByte   = readByteLE;
	cpu->readHword  = readHwordLE;
	cpu->readWord   = readWordLE;
	cpu->readDword  = readDwordLE;
	cpu->writeByte  = writeByteLE;
	cpu->writeHword = writeHwordLE;
	cpu->writeWord  = writeWordLE;
	cpu->writeDword = writeDwordLE;
	cpu->readOpcode = readOpcode;
	return cpu;
}
