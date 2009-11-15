#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"
#include "instructions.h"

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

#define MIPS_INSTR_NAME(name)		mips_##name
#define MIPS_INSTRUCTION(name)		static mipsInstruction MIPS_INSTR_NAME( name ) (mipsDasm *dasm)
#define INST_ENTRY(name, args, delay)	{ MIPS_INSTR_NAME( name ), #name args } 

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
MIPS_INSTRUCTION( SUB )
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
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) << readRegister(dasm->rs), readRegister(dasm->rt), 64));
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
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) >> readRegister(dasm->rs), readRegister(dasm->rt), 64));
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
	setRegister(dasm->rd, _signextend(readRegister(dasm->rt) >> readRegister(dasm->rs), readRegister(dasm->rt), 64));
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
	int i;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (31 - (i * 4));
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word right. */
MIPS_INSTRUCTION( LWR )
{
	s64 final = 0;
	int i;
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

/* Load doubleword left. */
MIPS_INSTRUCTION( LDL )
{
	s64 final = 0;
	int i;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (64 - ((i + 1) * 8));
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword right. */
MIPS_INSTRUCTION( LDR )
{
	s64 final = 0;
	int i;
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
	emulatedCpu.writeHword(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1LL << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}
			      
/* Store word. */
MIPS_INSTRUCTION( SW )
{
	emulatedCpu.writeWord(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt) & ((1LL << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word left. */
MIPS_INSTRUCTION( SWL )
{
	int i;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (32 - ((i + 1) * 4)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word right. */
MIPS_INSTRUCTION( SWR )
{
	int i;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (i * 4));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword. */
MIPS_INSTRUCTION( SD )
{
	emulatedCpu.writeDword(readRegister(dasm->rs) + dasm->immediate, readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword left. */
MIPS_INSTRUCTION( SDL )
{
	int i;
	u64 vAddr = _signextend(readRegister(dasm->rs), readRegister(dasm->rs), 64) + dasm->immediate;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (64 - ((i + 1) * 8)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword right. */
MIPS_INSTRUCTION( SDR )
{
	int i;
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
/*00*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*01*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*02*/	INST_ENTRY( J,		" %j",		1 ),
/*03*/	INST_ENTRY( JAL,	" %j",		1 ),
/*04*/	INST_ENTRY( BEQ,	" %s, %t, %i",	1 ),
/*05*/	INST_ENTRY( BNE,	" %s, %t, %i",	1 ),
/*06*/	INST_ENTRY( BLEZ,	" %s, %i",	1 ),
/*07*/	INST_ENTRY( BGTZ,	" %s, %i",	0 ),
/*08*/	INST_ENTRY( ADDI,	" %t, %s, %i",	0 ),
/*09*/	INST_ENTRY( ADDIU,	" %t, %s, %i",	0 ),
/*0A*/	INST_ENTRY( SLTI,	" %t, %s, %i",	0 ),
/*0B*/	INST_ENTRY( SLTIU,	" %t, %s, %i",	0 ),
/*0C*/	INST_ENTRY( ANDI,	" %t, %s, %i",	0 ),
/*0D*/	INST_ENTRY( ORI,	" %t, %s, %i",	0 ),
/*0E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0F*/	INST_ENTRY( LUI,	" %t, %i",	0 ),
/*10*/	INST_ENTRY( MFC0,	" %t, %d",	0 ),
/*11*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*12*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*13*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*14*/	INST_ENTRY( BEQL,	" %s, %t, %i",	1 ), 
/*15*/	INST_ENTRY( BNEL,	" %s, %t, %i",	1 ), 
/*16*/	INST_ENTRY( BLEZL,	" %s, %i",	1 ), 
/*17*/	INST_ENTRY( BGTZL,	" %s, %i",	1 ), 
/*18*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*19*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1A*/	INST_ENTRY( LDL,	" %t, %i(%s)",	0 ),
/*1B*/	INST_ENTRY( LDR,	" %t, %i(%s)",	0 ),
/*1C*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1D*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1F*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*20*/	INST_ENTRY( LB,		" %t, %i(%s)",	0 ),
/*21*/	INST_ENTRY( LH,		" %t, %i(%s)",	0 ),
/*22*/	INST_ENTRY( LWL,	" %t, %i(%s)",	0 ),
/*23*/	INST_ENTRY( LW,		" %t, %i(%s)",	0 ),
/*24*/	INST_ENTRY( LBU,	" %t, %i(%s)",	0 ),
/*25*/	INST_ENTRY( LHU,	" %t, %i(%s)",	0 ),
/*26*/	INST_ENTRY( LWR,	" %t, %i(%s)",	0 ),
/*27*/	INST_ENTRY( LWU,	" %t, %i(%s)",	0 ),
/*28*/	INST_ENTRY( SB,		" %t, %i(%s)",	0 ),
/*29*/	INST_ENTRY( SH,		" %t, %i(%s)",	0 ),
/*2A*/	INST_ENTRY( SWL,	" %t, %i(%s)",	0 ),
/*2B*/	INST_ENTRY( SW,		" %t, %i(%s)",	0 ),
/*2C*/	INST_ENTRY( SDL,	" %t, %i(%s)",	0 ),
/*2D*/	INST_ENTRY( SDR,	" %t, %i(%s)",	0 ),
/*2E*/	INST_ENTRY( SWR,	" %t, %i(%s)",	0 ),
/*2F*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*30*/	INST_ENTRY( LL,		" %t, %i(%s)",	0 ),
/*31*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*32*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*33*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*34*/	INST_ENTRY( LLD,	" %t, %i(%s)",	0 ),
/*35*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*36*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*37*/	INST_ENTRY( LD,		" %t, %i(%s)",	0 ),
/*38*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*39*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*3A*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*3B*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*3C*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*3D*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*3E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*30*/	INST_ENTRY( SD,		" %t, %i(%s)",	0 ),
/*40*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*41*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*42*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*43*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*44*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*45*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*46*/	INST_ENTRY( NOOP,	"",		0 )			/* NOOP */
};

/* Special opcodes.
 * The first six bits are 000000 and the instruction is encoded in the last six bits. */

mipsInstrTbl specialInstructionTable[] = {
/*00*/	INST_ENTRY( SLL,	" %d, %t, %h",	0 ), 
/*01*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*02*/	INST_ENTRY( SRL,	" %d, %t, %h",	0 ), 
/*03*/	INST_ENTRY( SRA,	" %d, %t, %h",	0 ), 
/*04*/	INST_ENTRY( SLLV,	" %d, %t, %s",	0 ), 
/*05*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*06*/	INST_ENTRY( SRLV,	" %d, %t, %s",	0 ), 
/*07*/	INST_ENTRY( SRAV,	" %d, %t, %s",	0 ),
/*08*/	INST_ENTRY( JR,		" %s",		1 ), 
/*09*/	INST_ENTRY( JALR,	" %s",		1 ), 
/*0A*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0B*/	INST_ENTRY( SYSCALL,	"",		0 ), 
/*0C*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0D*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0F*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*10*/	INST_ENTRY( MFHI,	" %d",		0 ), 
/*11*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*12*/	INST_ENTRY( MFLO,	" %d",		0 ), 
/*13*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*14*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*15*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*16*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*17*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*18*/	INST_ENTRY( MULT,	" %s, %t",	0 ), 
/*19*/	INST_ENTRY( MULTU,	" %s, %t",	0 ), 
/*1A*/	INST_ENTRY( DIV,	" %s, %t",	0 ), 
/*1B*/	INST_ENTRY( DIVU,	" %s, %t",	0 ), 
/*1C*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1D*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*1F*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*20*/	INST_ENTRY( ADD,	" %d, %s, %t",	0 ), 
/*21*/	INST_ENTRY( ADDU,	" %d, %s, %t",	0 ), 
/*22*/	INST_ENTRY( SUB,	" %d, %s, %t",	0 ), 
/*23*/	INST_ENTRY( SUBU,	" %d, %s, %t",	0 ), 
/*24*/	INST_ENTRY( AND,	" %d, %s, %t",	0 ), 
/*25*/	INST_ENTRY( OR,		" %d, %s, %t",	0 ), 
/*26*/	INST_ENTRY( XOR,	" %d, %s, %t",	0 ), 
/*27*/	INST_ENTRY( NOR,	" %d, %s, %t",	0 ), 
/*28*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*29*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*2A*/	INST_ENTRY( SLT,	" %d, %s, %t",	0 ), 
/*2B*/	INST_ENTRY( SLTU,	" %t, %s, %i",	0 )
};	

/* Regimm opcodes.
 * The first six bits are 000001 and the instruction is encoded in the t register space. */

mipsInstrTbl regimmInstructionTable[] = {
/*00*/	INST_ENTRY( BLTZ,	" %s, %i",	1 ), 
/*01*/	INST_ENTRY( BGEZ,	" %s, %i",	1 ), 
/*02*/	INST_ENTRY( BLTZL,	" %s, %i",	1 ), 
/*03*/	INST_ENTRY( BGEZL,	" %s, %i",	1 ), 
/*04*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*05*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*06*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*07*/	INST_ENTRY( BGTZ,	" %s, %i",	1 ), 
/*08*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*09*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0A*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0B*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0C*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0D*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0E*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*0F*/	INST_ENTRY( NOOP,	"",		0 ),			/* NOOP */
/*10*/	INST_ENTRY( BLTZAL,	" %s, %i",	1 ), 
/*11*/	INST_ENTRY( BGEZAL,	" %s, %i",	1 ), 
/*12*/	INST_ENTRY( BLTZALL,	" %s, %i",	1 ), 
/*13*/	INST_ENTRY( BGEZALL,	" %s, %i",	1 ), 
/*14*/	INST_ENTRY( NOOP,	"",		0 )			/* NOOP */
};

void execOpcode(mipsDasm *dasm)
{
#ifdef DEBUG		
	printf("Instruction 0x%08X Function 0x%08X rt 0x%08X\n", dasm->instruction, dasm->funct, dasm->rt);
#endif
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			specialInstructionTable[dasm->funct].execute(dasm);
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			regimmInstructionTable[dasm->rt].execute(dasm);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			instructionTable[dasm->instruction].execute(dasm);
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
		}
	}
}

char* textOpcode(mipsDasm *dasm)
{
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			return dasmFormat(specialInstructionTable[dasm->funct].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
			return "Not implemented";
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			return dasmFormat(regimmInstructionTable[dasm->rt].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
			return "Not implemented";
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			return dasmFormat(instructionTable[dasm->instruction].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
			return "Not implemented";
		}
	}
}
