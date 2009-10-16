#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "types.h"
#include "dasm.h"

#define DEFAULT_INSTRUCTION_PC 4

#define INSTRUCTION(opcode) (opcode >> 26)
#define FUNCT(opcode)       (opcode & 0x3F)

#define RS(opcode)          ((opcode >> 21) & 31)
#define RT(opcode)          ((opcode >> 16) & 31)
#define RD(opcode)          ((opcode >> 11) & 31)
#define SHIFT(opcode)       ((opcode >> 6) & 31)
#define IMMEDIATE(opcode)   (opcode & 0xFFFF)
#define JUMP(opcode)        (opcode & 0x3FFFFFF)

typedef struct _mipsCpu
{
	/* Backupped opcode for 1-instr-delay. */
	mipsRegister bOpcode;
	/* Program counters. */
	u32 pc;
	u32 nPc;
	/* Registers */
	mipsRegister r[34];
	mipsRegister cop0[32];	
	/* Ram accessors. */
	u8  endian;
	
	s8  (*readByte)  (u32 address);
	s16 (*readHword)  (u32 address);
	s32 (*readWord) (u32 address);
	s64 (*readDword) (u32 address);	
	
	void (*writeByte) (u32 address, s8 value);
	void (*writeHword) (u32 address, s16 value);
	void (*writeWord)(u32 address, s32 value);	
	void (*writeDword)(u32 address, s64 value);	
} mipsCpu;

/* 
 * The struct holding the state of the emulated cpu. 
 */

mipsCpu emulatedCpu;

/*
 * Function prototypes.
 */

mipsRegister getNextPC();

mipsRegister getPC();

void printRegisters();

void initializeCPU(u8 endian, u32 stackPtr);

void executeOpcode(u32 opcode);

void setRegister(mipsRegister reg, u32 value);

void setCop0Register(mipsRegister reg, u32 value);

mipsRegister readRegister(mipsRegister reg);

mipsRegister readCop0Register(mipsRegister reg);

void advancePC(mipsRegister nextPC);

void setPC(mipsRegister nextPC);

void setJump(mipsJumpAddress jumpAddress, int link);

void generateException(int exception, mipsDasm *instruction);

#endif
