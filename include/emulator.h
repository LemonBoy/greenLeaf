#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "dasm.h"

/*
 * Function prototypes.
 */

extern struct _mipsDasm opc;
char* registerToName(mipsRegister reg);

INLINE void setRegister(mipsCpu* cpu, mipsReg reg, mipsRegister value)
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

INLINE mipsRegister readRegister(mipsCpu* cpu, mipsReg reg)
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

INLINE void setCopRegister(mipsCpu* cpu, u8 copNumber, mipsReg reg, mipsRegister value)
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

INLINE mipsRegister readCopRegister(mipsCpu* cpu, u8 copNumber, mipsReg reg)
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

INLINE void advanceCycles(mipsCpu* cpu, int cycles)
{
	cpu->cycles += cycles;
#ifdef DEBUG	
	printf("Cycles is now %d\n", cpu->cycles);
#endif
}

INLINE void advancePC(mipsCpu* cpu, mipsRegister nextPC)
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

INLINE void doLink(mipsCpu* cpu, int r, int reg)
{
	int regis = 31;
	if(r)
		regis = reg;
	setRegister(cpu, regis, cpu->pc + DEFAULT_INSTRUCTION_PC);
}

INLINE void setJump(mipsCpu* cpu, mipsJumpAddress jumpAddress)
{
	cpu->pc  =  cpu->nPc;
	cpu->nPc = (cpu->pc & 0xF0000000) | jumpAddress;
	/*	cpu->nPc = (cpu->pc & 0xF0000000) | (jumpAddress << 2); */
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Jump -> Next pc is 0x%016llX\n", cpu->nPc);
#else
	printf("Jump -> Next pc is 0x%08X\n", cpu->nPc);
#endif
#endif
}

INLINE void setPC(mipsCpu* cpu, mipsRegister nextPC)
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

INLINE mipsRegister getNextPC(mipsCpu* cpu)
{
	return cpu->nPc;
}

INLINE mipsRegister getPC(mipsCpu* cpu)
{
	return cpu->pc;
}

INLINE void printRegisters(mipsCpu* cpu)
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

#define RESET_VECTOR_BEV_0 	(0x0000000080000000)
#define RESET_VECTOR_BEV_1 	(0x00000000BFC00200)
#define DEFAULT_RESET_VECTOR 	(RESET_VECTOR_BEV_1)

/* Todo : Check if whitin another exception.
 Check out page 141 for all the exception codes.
 Check page 152 and 175 for more about the exception vectors */

INLINE void generateException(mipsCpu* cpu, u32 exception, u32 delay)
{
#if BITCOUNT == 64
#define SHIFTVAL	63
#define ANDVAL1		0xFFFFFFFFFFFFFF00
#define ANDVAL2		0x3FFFFFFFFFFFFFFF
#else
#define SHIFTVAL	31
#define ANDVAL1		0xFFFFFF00
#define ANDVAL2		0x3FFFFFFF
#endif
	mipsRegister resetVectorAddress = DEFAULT_RESET_VECTOR;
	
#ifdef DEBUG
	printf("Caught exception 0x%08X\n", exception);
#endif
	
	setCopRegister(cpu, 0, COP0_REG_EPC, cpu->pc);
	setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & ANDVAL1) | (exception << 2));
	
	if (delay) {
		setCopRegister(cpu, 0, COP0_REG_EPC, readCopRegister(cpu, 0, COP0_REG_EPC) - 4);
		/* Set the BV bit in the CAUSE register if we are in a branch delay */
		setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & ANDVAL2) | (((mipsRegister)1) << SHIFTVAL));
	} else {
		setCopRegister(cpu, 0, COP0_REG_CAUSE, (readCopRegister(cpu, 0, COP0_REG_CAUSE) & ANDVAL2) | (((mipsRegister)0) << SHIFTVAL));
	}
	
	if ((readCopRegister(cpu, 0, COP0_REG_STATUS) >> 22) & 0x1) { /* BEV bit is set. */
		resetVectorAddress = RESET_VECTOR_BEV_1;
	} else {
		resetVectorAddress = RESET_VECTOR_BEV_0;
	}
	
	if (!(exception >= EXCEPTION_TLBMOD && exception <= EXCEPTION_TLBSTORE)) { /* Nothing TLB related ? */
		resetVectorAddress += 0x180;
	}
	
#ifdef DEBUG
#if BITCOUNT == 64
	printf("Jumping to the reset vector at 0x%016llX\n", resetVectorAddress);
#else
	printf("Jumping to the reset vector at 0x%08X\n", resetVectorAddress);
#endif
#endif
	
	setPC(cpu, resetVectorAddress);
}

#include "instructions.h"

INLINE void executeOpcode(mipsCpu* cpu, u32 opcode)
{
	dasmOpcode(cpu, opcode, &opc);
	
#ifdef DEBUG
	printf("Should use branch delay : %s\n", (opc.delay) ? "Yes" : "No");
	printf(">> (%08X) %s\n", opcode, textOpcode(cpu, &opc));
#endif
	
	if(opc.delay && cpu->bOpcode == 0) {
		cpu->bOpcode = getNextPC(cpu);
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
		return;
	}else{
		if(opc.delay)
			cpu->bOpcode = 0;
	}
	
	if(cpu->bOpcode != 0) {
		setPC(cpu, cpu->bOpcode);
		return;
	}
	
	execOpcode(cpu, &opc);
}

INLINE void runProcessor(mipsCpu* cpu, int cycles)
{
	if(cycles == 0) cycles = 1;
	memset(&opc, 0, sizeof(mipsDasm));
	for(cpu->cycles = 0; cpu->cycles < cycles; )
		executeOpcode(cpu, cpu->readOpcode(cpu, getNextPC(cpu)));
}

#endif
