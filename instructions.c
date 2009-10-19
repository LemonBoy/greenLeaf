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

static mipsRegister _signextend(mipsRegister input, mipsRegister sign, int size)
{
	return (input & ~(1 << (size - 1))) | (sign & (1 << (size - 1)));
}

/*
 * Start of the emulated functions.
 * All the opcode translations should go there.
 */


#define MIPS_INSTRUCTION(name)	mipsInstruction name (mipsDasm *dasm)

/*****************************************************************************/
/*  Arithmetic instructions                                                  */
/*****************************************************************************/
/* Add with overflow. */
MIPS_INSTRUCTION( ADD )
{
	setRegister(dasm->rd, readRegister(dasm->rs) + readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add unsigned. */
MIPS_INSTRUCTION( ADDU )
{
	setRegister(dasm->rd, (u64)(readRegister(dasm->rs)) + (u64)(readRegister(dasm->rt)));
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

/* Subtract with overflow. */
MIPS_INSTRUCTION( SUB )git@github.com:LemonBoy/greenLeaf.git
{
	setRegister(dasm->rd, readRegister(dasm->rs) - readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Subtract unsigned. */
MIPS_INSTRUCTION( SUBU )
{
	setRegister(dasm->rd, (u64)(readRegister(dasm->rs)) - (u64)(readRegister(dasm->rt)));
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


/*****************************************************************************/
/*  Logical instructions                                                     */
/*****************************************************************************/
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

/* Exclusive or. */
MIPS_INSTRUCTION( XOR )
{
	setRegister(dasm->rd, readRegister(dasm->rs) ^ readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Nor. */
MIPS_INSTRUCTION( NOR )
{
	setRegister(dasm->rd, ~(readRegister(dasm->rs) | readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than. */
MIPS_INSTRUCTION( SLT )
{
	if(readRegister(dasm->rs) < readRegister(dasm->rt))
		setRegister(dasm->rd, 1);
	else
		setRegister(dasm->rd, 0);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate unsigned. */
MIPS_INSTRUCTION( SLTU )
{
	if((mipsRegister)readRegister(dasm->rs) < (mipsRegister)readRegister(dasm->rt))
		setRegister(dasm->rd, 1);
	else
		setRegister(dasm->rd, 0);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate. */
MIPS_INSTRUCTION( SLTI )
{
	if(readRegister(dasm->rs) < dasm->immediate)
		setRegister(dasm->rt, 1);
	else
		setRegister(dasm->rt, 0);
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


/*****************************************************************************/
/*  Bitwise Shift instructions                                               */
/*****************************************************************************/
/* Shift left logical. */
MIPS_INSTRUCTION( SLL )
{
	setRegister(dasm->rd, readRegister(dasm->rt) << dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift left logical variable. */
MIPS_INSTRUCTION( SLLV )
{
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) << readRegister(dasm->rs), readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical. */
MIPS_INSTRUCTION( SRL )
{
	setRegister(dasm->rd, readRegister(dasm->rt) >> dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical variable. */
MIPS_INSTRUCTION( SRLV )
{
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) >> readRegister(dasm->rs), readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right arithmetic. */
MIPS_INSTRUCTION( SRA )
{
	/* TODO: Sign Extend */
	setRegister(dasm->rd, readRegister(dasm->rt) >> dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right arithmetic variable. */
MIPS_INSTRUCTION( SRAV )
{
	/* TODO: Sign Extend */
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) >> readRegister(dasm->rs), readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Conditional Branch instructions                                          */
/*****************************************************************************/
/* Branch on equal. */
MIPS_INSTRUCTION( BEQ )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt))
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch on equal likely. */
MIPS_INSTRUCTION( BEQL )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt)) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero. */
MIPS_INSTRUCTION( BGEZ )
{
	if(readRegister(dasm->rs) >= 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero likely. */
MIPS_INSTRUCTION( BGEZL )
{
	if(readRegister(dasm->rs) >= 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero and link. */
MIPS_INSTRUCTION( BGEZAL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) >= 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero and link likely. */
MIPS_INSTRUCTION( BGEZALL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) >= 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater than zero. */
MIPS_INSTRUCTION( BGTZ )
{
	if(readRegister(dasm->rs) > 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater than zero likely. */
MIPS_INSTRUCTION( BGTZL )
{
	if(readRegister(dasm->rs) > 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less or equal than zero. */
MIPS_INSTRUCTION( BLEZ )
{
	if(readRegister(dasm->rs) <= 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less or equal than zero likely. */
MIPS_INSTRUCTION( BLEZL )
{
	if(readRegister(dasm->rs) <= 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero. */
MIPS_INSTRUCTION( BLTZ )
{
	if(readRegister(dasm->rs) < 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero likely. */
MIPS_INSTRUCTION( BLTZL )
{
	if(readRegister(dasm->rs) < 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero and link. */
MIPS_INSTRUCTION( BLTZAL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) < 0)
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero and link likely. */
MIPS_INSTRUCTION( BLTZALL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) < 0) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNE )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt))
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNEL )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt)) {
		advancePC(_signextend(dasm->immediate << 2, dasm->immediate, 16));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}


/*****************************************************************************/
/*  Unconditional Jump instructions                                          */
/*****************************************************************************/
/* Jump. */
MIPS_INSTRUCTION( J )
{
	setJump(dasm->jump);
}

/* Jump and link. */
MIPS_INSTRUCTION( JAL )
{
	doLink(0, 0);
	setJump(dasm->jump);
}

/* Jump and link register. */
MIPS_INSTRUCTION( JALR )
{
	doLink(1, dasm->rd);
	setJump(readRegister(dasm->rs));
}

/* Jump register. */
MIPS_INSTRUCTION( JR )
{
	setJump(readRegister(dasm->rs));
}


/*****************************************************************************/
/*  Data Load instructions                                                   */
/*****************************************************************************/
/* Load byte. */
MIPS_INSTRUCTION( LB )
{
	setRegister(dasm->rt, (s8)(emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load byte unsigned. */
MIPS_INSTRUCTION( LBU )
{
	setRegister(dasm->rt, emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword. */
MIPS_INSTRUCTION( LH )
{
	setRegister(dasm->rt, (s16)(emulatedCpu.readHword(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword unsigned. */
MIPS_INSTRUCTION( LHU )
{
	setRegister(dasm->rt, emulatedCpu.readHword(readRegister(dasm->rs) + dasm->immediate));
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
	setRegister(dasm->rt, (s32)(emulatedCpu.readWord(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word left. */
MIPS_INSTRUCTION( LWL )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << 31 - (i * 4);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word right. */
MIPS_INSTRUCTION( LWR )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (i * 4);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word unsigned. */
MIPS_INSTRUCTION( LWU )
{
	setRegister(dasm->rt, emulatedCpu.readWord(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword. */
MIPS_INSTRUCTION( LD )
{
	setRegister(dasm->rt, (s64)(emulatedCpu.readDword(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword unsigned. */
MIPS_INSTRUCTION( LDU )
{
	setRegister(dasm->rt, emulatedCpu.readDword(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword left. */
MIPS_INSTRUCTION( LDL )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << 64 - ((i + 1) * 8);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword right. */
MIPS_INSTRUCTION( LDR )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (i * 8);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load linked (word). */
MIPS_INSTRUCTION( LL )
{
	/* I'm sure I need to do other things here... */
	setRegister(dasm->rt, emulatedCpu.readWord(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load linked doubleword. */
MIPS_INSTRUCTION( LLD )
{
	/* I'm sure I need to do other things here... */
	setRegister(dasm->rt, emulatedCpu.readDword(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Data Store instructions                                                  */
/*****************************************************************************/
/* Store byte. */
MIPS_INSTRUCTION( SB )
{
	emulatedCpu.writeByte(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1 << 8) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word. */
MIPS_INSTRUCTION( SH )
{
	emulatedCpu.writeHword(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1 << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}
			      
/* Store word. */
MIPS_INSTRUCTION( SW )
{
	emulatedCpu.writeWord(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1 << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word left. */
MIPS_INSTRUCTION( SWL )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (32 - ((i + 1) * 4)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word right. */
MIPS_INSTRUCTION( SWR )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (i * 4));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword. */
MIPS_INSTRUCTION( SD )
{
	emulatedCpu.writeDword(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1 << 64) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword left. */
MIPS_INSTRUCTION( SDL )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (64 - ((i + 1) * 8)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword right. */
MIPS_INSTRUCTION( SDR )
{
	s64 final = 0;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (i * 8));
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Coprocessor instructions                                                 */
/*****************************************************************************/
/* Move from coprocessor 0. */
MIPS_INSTRUCTION( MFC0 )
{
	cop0Handler(dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Special instructions                                                     */
/*****************************************************************************/
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

/* No-operation. */
MIPS_INSTRUCTION( NOOP )
{
#ifdef DEBUG
	exit(-6);
#endif
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Syscall. */
MIPS_INSTRUCTION( SYSCALL )
{
	generateException(8, dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/*****************************************************************************/
/*  Instruction Tables.                                                      */
/*****************************************************************************/
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
	{SLTI	, "SLTI %t, %s, %i"}, // 10
	{SLTIU	, "SLTIU %t, %s, %i"},
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
	{SRA	, "SRA %d, %t, %h"},
	{SLLV	, "SLLV %d, %t, %s"},
	{NOOP	, ""},
	{SRLV	, "SLRLV %d, %t, %s"},
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
	{SUB	, "SUB %d, %s, %t"},
	{SUBU	, "SUBU %d, %s, %t"},	
	{AND	, "AND %d, %s, %t"},
	{OR	, "OR %d, %s, %t"},
	{XOR	, "XOR %d, %s, %t"},
	{NOR	, "NOR %d, %s, %t"},
	{NOOP	, ""}, // 40	
       	{NOOP	, ""},
        {SLT	, "SLT %d, %s, %t"},
        {SLTU	, "SLTU %t, %s, %i"}	

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
