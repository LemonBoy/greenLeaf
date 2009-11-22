#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "memory.h"
#include "emulator.h"
#include "instructions.h"
#include "dasm.h"

/*
 *  Register related stuff. 
 */

void setRegister(mipsCpu* cpu, mipsReg reg, mipsRegister value)
{
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Write 0x%016llX in %s\n", value, registerToName(reg));
#else
	printf("Write 0x%08X in %s\n", value, registerToName(reg));
#endif
#endif
	/* Keep r0 hardwired to 0 */
	if(reg == 0)
		return;
	cpu->r[reg] = value;
}

mipsRegister readRegister(mipsCpu* cpu, mipsReg reg)
{
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Read %s: 0x%016llX\n", registerToName(reg), cpu->r[reg]);
#else
	printf("Read %s: 0x%08X\n", registerToName(reg), cpu->r[reg]);
#endif	
#endif	
	return cpu->r[reg];
}

void setCopRegister(mipsCpu* cpu, u8 copNumber, mipsReg reg, mipsRegister value)
{
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Cop%i -> Write 0x%016llX in cop%i[%u]\n", copNumber, value, copNumber, reg);
#else
	printf("Cop%i -> Write 0x%08X in cop%i[%u]\n", copNumber, value, copNumber, reg);
#endif	
#endif
	cpu->cr[reg][copNumber] = value;
}

mipsRegister readCopRegister(mipsCpu* cpu, u8 copNumber, mipsReg reg)
{
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Cop%i -> Read cop%i[%u]: 0x%016llX\n", copNumber, copNumber, reg, cpu->cr[reg][copNumber]);
#else
	printf("Cop%i -> Read cop%i[%u]: 0x%08X\n", copNumber, copNumber, reg, cpu->cr[reg][copNumber]);
#endif	
#endif	
	return cpu->cr[reg][copNumber];
}

void advancePC(mipsCpu* cpu, mipsRegister nextPC)
{
	cpu->pc   = cpu->nPc;
	cpu->nPc += nextPC;
#ifdef DEBUG	
#if BITCOUNT == 64
	printf("Advance -> Next pc is 0x%016llX\n", cpu->nPc);
#else
	printf("Advance -> Next pc is 0x%08X\n", cpu->nPc);
#endif	
#endif
}

void doLink(mipsCpu* cpu, int r, int reg)
{
	int regis = 31;
	if(r)
		regis = reg;
	setRegister(cpu, regis, cpu->pc + DEFAULT_INSTRUCTION_PC);
}

void setJump(mipsCpu* cpu, mipsJumpAddress jumpAddress)
{
	cpu->pc  =  cpu->nPc;
	cpu->nPc = (cpu->pc & 0xF0000000) | (jumpAddress << 2);
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Jump -> Next pc is 0x%016llX\n", cpu->nPc);
#else
	printf("Jump -> Next pc is 0x%08X\n", cpu->nPc);
#endif
#endif
}

void setPC(mipsCpu* cpu, mipsRegister nextPC)
{
	cpu->pc  = cpu->nPc;
	cpu->nPc = nextPC;
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Set -> Next pc is 0x%016llX\n", cpu->nPc);
#else
	printf("Set -> Next pc is 0x%08X\n", cpu->nPc);
#endif
#endif
}

mipsRegister getNextPC(mipsCpu* cpu)
{
	return cpu->nPc;
}

mipsRegister getPC(mipsCpu* cpu)
{
	return cpu->pc;
}

void printRegisters(mipsCpu* cpu)
{
	int reg, cop;
	
#if BITCOUNT == 64
	for(reg = 0; reg < 32; reg++)
		printf("%s: 0x%016llX\n", registerToName(reg), cpu->r[reg]);
	for(cop = 0; cop < 3; cop++)
		for(reg = 0; reg < 32; reg++) 
			printf("COP%i %02u: 0x%016llX\n", cop, reg, cpu->cr[reg][cop]);
	printf("HI: 0x%016llX\tLO:      0x%016llX\n", cpu->r[REGISTER_HI], cpu->r[REGISTER_LO]);
	printf("PC: 0x%016llX\tNext PC: 0x%016llX\n", cpu->pc, cpu->nPc);
#else
	for(reg = 0; reg < 32; reg++)
		printf("%s: 0x%08X\n", registerToName(reg), cpu->r[reg]);
	for(cop = 0; cop < 3; cop++)
		for(reg = 0; reg < 32; reg++) 
			printf("COP%i %02u: 0x%08X\n", cop, reg, cpu->cr[reg][cop]);
	printf("HI: 0x%08X\tLO:      0x%08X\n", cpu->r[REGISTER_HI], cpu->r[REGISTER_LO]);
	printf("PC: 0x%08X\tNext PC: 0x%08X\n", cpu->pc, cpu->nPc);
#endif
}

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
			cpu->r[reg] = ((mipsRegister)0) + stackPtr;
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
	return cpu;
}

#define RESET_VECTOR_BEV_0 	(0x0000000080000000)
#define RESET_VECTOR_BEV_1 	(0x00000000BFC00200)
#define DEFAULT_RESET_VECTOR 	(RESET_VECTOR_BEV_1)

/* Todo : Check if whitin another exception.
   Check out page 141 for all the exception codes.
   Check page 152 and 175 for more about the exception vectors */

/* TODO: Convert to proper 32bit code for 32bit mode! */

void generateException(mipsCpu* cpu, u32 exception, u32 delay)
{
	mipsRegister resetVectorAddress = DEFAULT_RESET_VECTOR;
	
	printf("Caught exception 0x%08X\n", exception);

	setCopRegister(cpu, 0, COP0_REG_EPC, cpu->pc);
	setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & 0xFFFFFF00) | (exception << 2));

	if (delay) {
		setCopRegister(cpu, 0, COP0_REG_EPC, readCopRegister(cpu, 0, COP0_REG_EPC) - 4);
		/* Set the BV bit in the CAUSE register if we are in a branch delay */
		setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & 0x3FFFFF00) | (((mipsRegister)1) << 31));
	} else {
		setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & 0x3FFFFF00) | (((mipsRegister)0) << 31));
	}
	
	if ((readCopRegister(cpu, 0, COP0_REG_STATUS) >> 22) & 0x1) { /* BEV bit is set. */
		resetVectorAddress = RESET_VECTOR_BEV_1;
	} else {
		resetVectorAddress = RESET_VECTOR_BEV_0;
	}
	
	if (!(exception >= EXCEPTION_TLBMOD && exception <= EXCEPTION_TLBSTORE)) { /* Nothing TLB related ? */
		resetVectorAddress += 0x180;
	}

#if BITCOUNT == 64
	printf("Jumping to the reset vector at 0x%016llX\n", resetVectorAddress);
#else
	printf("Jumping to the reset vector at 0x%08X\n", resetVectorAddress);
#endif
	
	setPC(cpu, resetVectorAddress);
}

void executeOpcode(mipsCpu* cpu, u32 opcode)
{
	mipsDasm *opc;
	
	dasmOpcode(cpu, opcode, &opc);

#ifdef DEBUG
	printf("Should use branch delay : %s\n", (opc->delay) ? "Yes" : "No");
#endif

	printf(">> (%08X) %s\n", opcode, textOpcode(cpu, opc));
	
	if(opc->delay && cpu->bOpcode == 0) {
		cpu->bOpcode = getNextPC(cpu);
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
		return;
	}else{
		if(opc->delay)
			cpu->bOpcode = 0;
	}
	
	if(cpu->bOpcode != 0) {
		setPC(cpu, cpu->bOpcode);
		return;
	}

	execOpcode(cpu, opc);
}

void runProcessor(mipsCpu* cpu)
{
	u32 opcode;
	
	if(cpu->endian == ENDIANNESS_LE) {
		opcode = (u8)(cpu->readByte(cpu, getNextPC(cpu) + 3)) << 24 | 
			 (u8)(cpu->readByte(cpu, getNextPC(cpu) + 2)) << 16 | 
			 (u8)(cpu->readByte(cpu, getNextPC(cpu) + 1)) << 8  | 
			 (u8)(cpu->readByte(cpu, getNextPC(cpu) + 0)) << 0;
	}else{
		opcode = (u32)(cpu->readWord(cpu, getNextPC(cpu)));
	}
	
	executeOpcode(cpu, opcode);
}
