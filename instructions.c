#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"

/* README:
 * All the branch instruction follows the one-instruction-delay-slot rule
 * so remember to add it into dasmOpcode rules (dasm.c) for the sake of
 * emulation goodness. */

void cop0Handler(mipsDasm *dasm)
{
	switch(dasm->rs) {
		case 0x0:
			setRegister(dasm->rt, readCop0Register(dasm->rd));
			break;
		case 0x4:
			setCop0Register(dasm->rd, (u32)(dasm->rt));
			break;
	}
}

/*
 * Start of the emulated functions.
 * All the opcode translations should go there.
 */


#define MIPS_INSTRUCTION(name)	mipsInstruction name (mipsDasm *dasm)

/* Add with overflow. */
MIPS_INSTRUCTION( ADD )
{
	setRegister(dasm->rd, readRegister(dasm->rs) + readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate. */
MIPS_INSTRUCTION( ADDI )
{
	setRegister(dasm->rt, readRegister(dasm->rs) + dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate unsigned. */
MIPS_INSTRUCTION( ADDIU )
{
	setRegister(dasm->rt, (u64)(readRegister(dasm->rs)) + (u64)(dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add unsigned. */
MIPS_INSTRUCTION( ADDU )
{
	setRegister(dasm->rd, (u64)(readRegister(dasm->rs)) + (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* And. */
MIPS_INSTRUCTION( AND )
{
	setRegister(dasm->rd, readRegister(dasm->rs) & readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* And immediate. */
MIPS_INSTRUCTION( ANDI )
{
	setRegister(dasm->rt, readRegister(dasm->rs) & dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* Branch if equal. */
MIPS_INSTRUCTION( BEQ )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt))
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero . */
MIPS_INSTRUCTION( BGEZ )
{
	if(readRegister(dasm->rs) >= 0)
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater than zero . */
MIPS_INSTRUCTION( BGTZ )
{
	if(readRegister(dasm->rs) > 0)
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less or equal than zero . */
MIPS_INSTRUCTION( BLEZ )
{
	if(readRegister(dasm->rs) <= 0)
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero . */
MIPS_INSTRUCTION( BLTZ )
{
	if(readRegister(dasm->rs) < 0)
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNE )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt))
		advancePC(dasm->immediate << 2);
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}


/* Divide. */
MIPS_INSTRUCTION( DIV )
{
	setRegister(REGISTER_HI, readRegister(dasm->rs) / readRegister(dasm->rt));
	setRegister(REGISTER_LO, readRegister(dasm->rs) % readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Divide unsigned. */
MIPS_INSTRUCTION( DIVU )
{
	setRegister(REGISTER_HI, (u64)(readRegister(dasm->rs)) / (u64)(readRegister(dasm->rt)));
	setRegister(REGISTER_LO, (u64)(readRegister(dasm->rs)) % (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Jump. */
MIPS_INSTRUCTION( J )
{
	setJump(dasm->jump, 0);
}

/* Jump and link. */
MIPS_INSTRUCTION( JAL )
{
	setJump(dasm->jump, 1);
}

/* Jump and link register. */
MIPS_INSTRUCTION( JALR )
{
	setJump(readRegister(dasm->rs), 1);
}

/* Jump register. */
MIPS_INSTRUCTION( JR )
{
	setPC(readRegister(dasm->rs));
}

/* Load byte. */
MIPS_INSTRUCTION( LB )
{
	setRegister(dasm->rt, emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load byte unsigned. */
MIPS_INSTRUCTION( LBU )
{
	setRegister(dasm->rt, (u8)(emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword unsigned. */
MIPS_INSTRUCTION( LHU )
{
	setRegister(dasm->rt, (u64)(emulatedCpu.readHword((u64)(readRegister(dasm->rs)) + (u64)(dasm->immediate))));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load upper immediate. */
MIPS_INSTRUCTION( LUI )
{
	setRegister(dasm->rt, dasm->immediate << 16);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word. */
MIPS_INSTRUCTION( LW )
{
	setRegister(dasm->rt, emulatedCpu.readWord(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from coprocessor 0. */
MIPS_INSTRUCTION( MFC0 )
{
	cop0Handler(dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from HI. */
MIPS_INSTRUCTION( MFHI )
{
	setRegister(dasm->rd, readRegister(REGISTER_HI));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from LO. */
MIPS_INSTRUCTION( MFLO )
{
	setRegister(dasm->rd, readRegister(REGISTER_LO));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* Multiply. */
MIPS_INSTRUCTION( MULT )
{
	setRegister(33, readRegister(dasm->rs) * readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Multiply unsigned. */
MIPS_INSTRUCTION( MULTU )
{
	setRegister(33, (u64)(readRegister(dasm->rs)) * (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* No-operation. */
MIPS_INSTRUCTION( NOOP )
{
#ifdef DEBUG
	exit(-6);
#endif
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* Or. */
MIPS_INSTRUCTION( OR )
{
	setRegister(dasm->rd, readRegister(dasm->rs) | readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Or immediate. */
MIPS_INSTRUCTION( ORI )
{
	setRegister(dasm->rt, readRegister(dasm->rs) | dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store byte. */
MIPS_INSTRUCTION( SB )
{
	emulatedCpu.writeByte(readRegister(dasm->rs) + (dasm->immediate & 0xFF), readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift left logical. */
MIPS_INSTRUCTION( SLL )
{
	setRegister(dasm->rd, readRegister(dasm->rt) << dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical. */
MIPS_INSTRUCTION( SRL )
{
	setRegister(dasm->rd, readRegister(dasm->rt) >> dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate unsigned. */
MIPS_INSTRUCTION( SLTIU )
{
	if((mipsRegister)readRegister(dasm->rs) < (mipsRegister)dasm->immediate)
		setRegister(dasm->rt, 1);
	else
		setRegister(dasm->rt, 0);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Syscall. */
MIPS_INSTRUCTION( SYSCALL )
{
	generateException(8, dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/* Store word. */
MIPS_INSTRUCTION( SW )
{
	emulatedCpu.writeWord(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* General table with all opcodes. */

mipsInstrTbl instructionTable[] = {
	{NOOP	, ""},
	{NOOP	, ""},
	{J	, "J %j"},
	{JAL	, "JAL %j"},
	{BEQ	, "BEQ %s, %t, %i"},
	{BNE	, "BNE %s, %t, %i"},
	{BLEZ	, "BLEZ %s, %i"},
	{BGTZ	, "BGTZ %s, %i"},
	{ADDI	, "ADDI %t, %s, %i"},
	{ADDIU	, "ADDIU %t, %s, %i"},
	{NOOP	, ""}, // 10
	{NOOP	, ""},
	{ANDI	, "ANDI %t, %s, %i"},
	{ORI	, "ORI %t, %s, %i"},
	{NOOP	, ""},
	{LUI	, "LUI %t, %i"},
	{MFC0	, "MFC0 %t, %d"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 20
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 30	
	{NOOP	, ""},
	{LB	, "LB %t, %i(%s)"},
	{NOOP	, ""},
	{NOOP	, ""},
	{LW	, "LW %t, %i(%s)"},
	{LBU	, "LBU %t, %i(%s)"},
	{LHU	, "LHU %t, %i(%s)"},
	{NOOP	, ""},
	{NOOP	, ""},
	{SB	, "SB %t, %i(%s)"}, // 40
	{NOOP	, ""},
	{NOOP	, ""},
	{SW	, "SW %t, %i(%s)"},
	{NOOP	, ""},
	{NOOP	, ""},	
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 50
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 60
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""} // 70				
};

/* Special opcodes.
 * The first six bits are 000000 and the instruction is encoded in the last six bits. */

mipsInstrTbl specialInstructionTable[] = {
	{SLL	, "SLL %d, %t, %h"},
	{NOOP	, ""},
	{SRL	, "SRL %d, %t, %h"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{JR	, "JR %s"},
	{JALR	, "JALR %s"}, 
	{NOOP	, ""}, // 10
	{SYSCALL, "SYSCALL"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{MFHI	, "MFHI %d"},
	{NOOP	, ""},
	{MFLO	, "MFLO %d"},
	{NOOP	, ""}, 
	{NOOP	, ""}, // 20
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{MULT	, "MULT %s, %t"},
	{MULTU	, "MULTU %s, %t"},
	{DIV	, "DIV %s, %t"},
	{DIVU	, "DIVU %s, %t"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 30
	{NOOP	, ""},
	{NOOP	, ""},
	{ADDU	, "ADDU %d, %s, %t"},
	{NOOP	, ""},
	{NOOP	, ""},	
	{AND	, "AND %d, %s, %t"},
	{OR	, "OR %d, %s, %t"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""} // 40	
};	

/* Regimm opcodes.
 * The first six bits are 000001 and the instruction is encoded in the t register space. */

mipsInstrTbl regimmInstructionTable[] = {
	{BLTZ	, "BLTZ %s, %i"},
	{BGEZ	, "BGEZ %s, %i"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{BGTZ	, "BGTZ %s, %i"},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}, // 10
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""},
	{NOOP	, ""}
};

void execOpcode(mipsDasm *dasm)
{
#ifdef DEBUG		
	printf("Instruction 0x%08X Function 0x%08X\n", dasm->instruction, dasm->funct);
#endif
	if(dasm->instruction != 0)
		instructionTable[dasm->instruction].execute(dasm);
	else if(dasm->instruction == 0)
		specialInstructionTable[dasm->funct].execute(dasm);
	else if(dasm->instruction == 1)
		regimmInstructionTable[dasm->rt].execute(dasm);
}

char* textOpcode(mipsDasm *dasm)
{
	if(dasm->instruction != 0)
		return dasmFormat(instructionTable[dasm->instruction].textDisasm, dasm);
	else if(dasm->instruction == 0)
		return dasmFormat(specialInstructionTable[dasm->funct].textDisasm, dasm);
	else if(dasm->instruction == 1)
		return dasmFormat(regimmInstructionTable[dasm->rt].textDisasm, dasm);
	return "Not implemented";
}
