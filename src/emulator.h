#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "types.h"
#include "dasm.h"

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

#define EXCEPTION_ADDRESS	0
#define EXCEPTION_SYSCALL	8

typedef struct {
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
	
	u8  (*readByte) (u32 address);
	u16 (*readHword)(u32 address);
	u32 (*readWord) (u32 address);
	u64 (*readDword)(u32 address);	
	
	void (*writeByte) (u32 address, u8 value);
	void (*writeHword)(u32 address, u16 value);
	void (*writeWord) (u32 address, u32 value);	
	void (*writeDword)(u32 address, u64 value);	
} mipsCpu;

/* 
 * The struct holding the state of the emulated cpu. 
 */

mipsCpu emulatedCpu;

/*
 * Function prototypes.
 */

void		initializeCPU	 (u8 endian, u32 stackPtr);
void		runProcessor	 ();
void		executeOpcode	 (u32 opcode);
void		printRegisters	 ();
void		generateException(u32 exception, u32 delay);

void		setCop0Register	(mipsReg reg, mipsRegister value);
mipsRegister	readCop0Register(mipsReg reg);

mipsRegister	getPC		();
mipsRegister	getNextPC	();

void		setRegister	(mipsReg reg, mipsRegister value);
mipsRegister	readRegister	(mipsReg reg);

void		advancePC	(mipsRegister nextPC);
void		setPC		(mipsRegister nextPC);
void		setJump		(mipsJumpAddress jumpAddress);
void		doLink		(int r, int reg);

#endif
