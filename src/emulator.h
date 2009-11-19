#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "types.h"

#define ENDIANNESS_BE		(0x02)
#define ENDIANNESS_LE		(0x01)

#define DEFAULT_INSTRUCTION_PC	4

#define REGISTER_HI 		32
#define REGISTER_LO		33

#define INSTRUCTION(opcode)	(opcode >> 26)
#define FUNCT(opcode)		(opcode & 0x3F)

#define RS(opcode)		((opcode >> 21) & 31)
#define RT(opcode)		((opcode >> 16) & 31)
#define RD(opcode)		((opcode >> 11) & 31)
#define SHIFT(opcode)		((opcode >> 6) & 31)
#define IMMEDIATE(opcode)	(opcode & 0xFFFF)
#define JUMP(opcode)		(opcode & 0x3FFFFFF)

/* TODO: These need to be changed to the proper values!
 * Also, any thing calling generateException should use these! */
#define EXCEPTION_ADDRESS	0
#define EXCEPTION_BREAKPOINT	1
#define EXCEPTION_RESERVED	2
#define EXCEPTION_TRAP		3
#define EXCEPTION_SYSCALL	8

typedef struct _mipsCpu {
	/* Backupped opcode for 1-instr-delay. */
	mipsRegister bOpcode;
	/* Program counters. */
	mipsRegister pc;
	mipsRegister nPc;
	/* Registers */
	mipsRegister r[34];
	mipsRegister cr[32][3];
	/* Ram accessors. */
	u8 endian;
	
	struct _mipsMappedMemory *rootBank;
	struct _mipsMappedMemory *mappedBanks;
	
	u8  (*readByte) (struct _mipsCpu* cpu, u32 address);
	u16 (*readHword)(struct _mipsCpu* cpu, u32 address);
	u32 (*readWord) (struct _mipsCpu* cpu, u32 address);
	u64 (*readDword)(struct _mipsCpu* cpu, u32 address);
	
	void (*writeByte) (struct _mipsCpu* cpu, u32 address, u8 value);
	void (*writeHword)(struct _mipsCpu* cpu, u32 address, u16 value);
	void (*writeWord) (struct _mipsCpu* cpu, u32 address, u32 value);
	void (*writeDword)(struct _mipsCpu* cpu, u32 address, u64 value);
} mipsCpu;

/*
 * Function prototypes.
 */

mipsCpu*	initializeCPU	 (u8 endian, u32 stackPtr);
void		runProcessor	 (mipsCpu* cpu);
void		executeOpcode	 (mipsCpu* cpu, u32 opcode);
void		printRegisters	 (mipsCpu* cpu);
void		generateException(mipsCpu* cpu, u32 exception, u32 delay);

void		setCopRegister	 (mipsCpu* cpu, u8 copNumber, mipsReg reg, mipsRegister value);
mipsRegister 	readCopRegister	 (mipsCpu* cpu, u8 copNumber, mipsReg reg);

mipsRegister	getPC		 (mipsCpu* cpu);
mipsRegister	getNextPC	 (mipsCpu* cpu);

void		setRegister	 (mipsCpu* cpu, mipsReg reg, mipsRegister value);
mipsRegister	readRegister	 (mipsCpu* cpu, mipsReg reg);

void		advancePC	 (mipsCpu* cpu, mipsRegister nextPC);
void		setPC		 (mipsCpu* cpu, mipsRegister nextPC);
void		setJump		 (mipsCpu* cpu, mipsJumpAddress jumpAddress);
void		doLink		 (mipsCpu* cpu, int r, int reg);

#endif
