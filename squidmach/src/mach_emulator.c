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

mipsCpu* initializeCPU(u8 endian, u32 stackPtr)
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
	
	cpu->endian = endian;
	cpu->rootBank = NULL;
	cpu->mappedBanks = NULL;
	
	switch(cpu->endian) {
		case ENDIANNESS_LE:
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
			break;
		
		case ENDIANNESS_BE:
#ifdef DEBUG
			printf("Cpu endianess set to big endian\n");
#endif
			cpu->readByte  = readByteBE;
			cpu->readHword = readHwordBE;
			cpu->readWord  = readWordBE;
			cpu->readDword = readDwordBE;
			
			cpu->writeByte  = writeByteBE;
			cpu->writeHword = writeHwordBE;
			cpu->writeWord  = writeWordBE;
			cpu->writeDword = writeDwordBE;
			break;
		default:
			printf("Error: Endian setting is neither BE nor LE.\n");
			assert((cpu->endian == ENDIANNESS_BE) || (cpu->endian == ENDIANNESS_LE));
			break;
	}
	cpu->readOpcode = readOpcode;
	return cpu;
}
