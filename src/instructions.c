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
	switch(readRegister(dasm->rs)) {
		case 0x0:
			setRegister(dasm->rt, readCop0Register(dasm->rd));
			break;
		case 0x4:
			setCop0Register(dasm->rd, (u32)(dasm->rt));
			break;
	}
	
	advancePC(DEFAULT_INSTRUCTION_PC);
}

void copxHandler(mipsDasm *dasm)
{
	switch(readRegister(dasm->rs)) {
		case 0x0:
			setRegister(dasm->rt, readCop0Register(dasm->rd));
			break;
		case 0x4:
			setCop0Register(dasm->rd, (u32)(dasm->rt));
			break;
	}
	
	advancePC(DEFAULT_INSTRUCTION_PC);
}

static mipsRegister _signextend(mipsRegister_u input, int origsize, int newsize)
{
	int s = (((input & (1LL << (origsize - 1LL))) > 0LL) ? 1 : 0);
	mipsRegister_u mask;
	if(s) {
		mask = (0LL - 1LL) & ~((1LL << origsize) - 1LL);
		input |= mask;
	}else{
		mask = (1LL << origsize) - 1LL;
		input &= mask;
	}
	return input;
}

static mipsRegister _zeroextend(mipsRegister_u input, int origsize, int newsize)
{
	return input &= (1LL << origsize) - 1LL;
}

/*
 * Start of the emulated functions.
 * All the opcode translations should go there.
 */

#define MIPS_INSTR_NAME(name)			mips_##name
#define MIPS_INSTRUCTION(name)			static mipsInstruction MIPS_INSTR_NAME(name) (mipsDasm *dasm)
#define INST_ENTRY(name, args, delay)		{ MIPS_INSTR_NAME(name), #name args, delay } 

#define MIPS_COP_INSTR_NAME(name)		mips_cop_##name
#define MIPS_COP_INSTRUCTION(name)		static mipsInstruction MIPS_COP_INSTR_NAME(name)(mipsDasm *dasm, int cop)
#define COP_INST_ENTRY(name, args, delay)	{ MIPS_COP_INSTR_NAME(name), #name args, delay } 

/*****************************************************************************/
/*  Arithmetic instructions                                                  */
/*****************************************************************************/
/* Add with overflow. */
MIPS_INSTRUCTION( ADD )
{
	s32 val = readRegister(dasm->rs) + readRegister(dasm->rt);
	s64 out = _signextend(val, 32, 64);
	setRegister(dasm->rd, out);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add unsigned. */
MIPS_INSTRUCTION( ADDU )
{
	u32 val = readRegister(dasm->rs) + readRegister(dasm->rt);
	u64 out = _zeroextend(val, 32, 64);
	setRegister(dasm->rd, out);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate. */
MIPS_INSTRUCTION( ADDI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	s32 val = readRegister(dasm->rs) + imm;
	s64 out = _signextend(val, 32, 64);
	setRegister(dasm->rd, out);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate unsigned. */
MIPS_INSTRUCTION( ADDIU )
{
	u64 imm = _zeroextend(dasm->immediate, 16, 64);
	u32 val = ((u32)readRegister(dasm->rs)) + imm;
	u64 out = _zeroextend(val, 32, 64);
	setRegister(dasm->rt, out);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word add with overflow. */
MIPS_INSTRUCTION( DADD )
{
	s64 val = readRegister(dasm->rs) + readRegister(dasm->rt);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word add unsigned. */
MIPS_INSTRUCTION( DADDU )
{
	u64 val = readRegister(dasm->rs) + readRegister(dasm->rt);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word add immediate. */
MIPS_INSTRUCTION( DADDI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	s64 val = readRegister(dasm->rs) + imm;
	setRegister(dasm->rt, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word add immediate unsigned. */
MIPS_INSTRUCTION( DADDIU )
{
	s64 imm = _zeroextend(dasm->immediate, 16, 64);
	u64 val = readRegister(dasm->rs) + imm;
	setRegister(dasm->rt, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Subtract with overflow. */
MIPS_INSTRUCTION( SUB )
{
	s32 tmp = readRegister(dasm->rs) - readRegister(dasm->rt);
	s64 val = _signextend(tmp, 32, 64);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Subtract unsigned. */
MIPS_INSTRUCTION( SUBU )
{
	u32 tmp = readRegister(dasm->rs) - readRegister(dasm->rt);
	u64 val = _zeroextend(tmp, 32, 64);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word subtract with overflow. */
MIPS_INSTRUCTION( DSUB )
{
	s64 val = readRegister(dasm->rs) - readRegister(dasm->rt);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word subtract unsigned. */
MIPS_INSTRUCTION( DSUBU )
{
	u64 val = readRegister(dasm->rs) - readRegister(dasm->rt);
	setRegister(dasm->rd, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Multiply. */
MIPS_INSTRUCTION( MULT )
{
	s32 tmp = readRegister(dasm->rs) * readRegister(dasm->rt);
	s64 hi  = _signextend(tmp, 32, 64);
	setRegister(REGISTER_HI, hi);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Multiply unsigned. */
MIPS_INSTRUCTION( MULTU )
{
	u32 tmp = readRegister(dasm->rs) * readRegister(dasm->rt);
	u64 hi  = _zeroextend(tmp, 32, 64);
	setRegister(REGISTER_HI, hi);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word multiply. */
MIPS_INSTRUCTION( DMULT )
{
	s64 hi = readRegister(dasm->rs) * readRegister(dasm->rt);
	setRegister(REGISTER_HI, hi);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word multiply unsigned. */
MIPS_INSTRUCTION( DMULTU )
{
	u64 hi = readRegister(dasm->rs) * readRegister(dasm->rt);
	setRegister(REGISTER_HI, hi);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Divide. */
MIPS_INSTRUCTION( DIV )
{
	s32 tmp = readRegister(dasm->rs) / readRegister(dasm->rt);
	s64 hi  = _signextend(tmp, 32, 64);
	    tmp = readRegister(dasm->rs) % readRegister(dasm->rt);
	s64 lo  = _signextend(tmp, 32, 64);
	setRegister(REGISTER_HI, hi);
	setRegister(REGISTER_LO, lo);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Divide unsigned. */
MIPS_INSTRUCTION( DIVU )
{
	u32 tmp = readRegister(dasm->rs) / readRegister(dasm->rt);
	u64 hi  = _zeroextend(tmp, 32, 64);
	    tmp = readRegister(dasm->rs) % readRegister(dasm->rt);
	u64 lo  = _zeroextend(tmp, 32, 64);
	setRegister(REGISTER_HI, hi);
	setRegister(REGISTER_LO, lo);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word divide. */
MIPS_INSTRUCTION( DDIV )
{
	s64 hi = readRegister(dasm->rs) / readRegister(dasm->rt);
	s64 lo = readRegister(dasm->rs) % readRegister(dasm->rt);
	setRegister(REGISTER_HI, hi);
	setRegister(REGISTER_LO, lo);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word divide unsigned. */
MIPS_INSTRUCTION( DDIVU )
{
	u64 hi = readRegister(dasm->rs) / readRegister(dasm->rt);
	u64 lo = readRegister(dasm->rs) % readRegister(dasm->rt);
	setRegister(REGISTER_HI, hi);
	setRegister(REGISTER_LO, lo);
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
	s64 imm = _zeroextend(dasm->immediate, 16, 64);
	setRegister(dasm->rt, readRegister(dasm->rs) & imm);
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
	s64 imm = _zeroextend(dasm->immediate, 16, 64);
	setRegister(dasm->rt, readRegister(dasm->rs) | imm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Exclusive or. */
MIPS_INSTRUCTION( XOR )
{
	setRegister(dasm->rd, readRegister(dasm->rs) ^ readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Exclusive or immediate. */
MIPS_INSTRUCTION( XORI )
{
	s64 imm = _zeroextend(dasm->immediate, 16, 64);
	setRegister(dasm->rt, readRegister(dasm->rs) ^ imm);
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
	if(((mipsRegister_u)readRegister(dasm->rs)) < ((mipsRegister_u)readRegister(dasm->rt)))
		setRegister(dasm->rd, 1);
	else
		setRegister(dasm->rd, 0);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate. */
MIPS_INSTRUCTION( SLTI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(readRegister(dasm->rs) < imm)
		setRegister(dasm->rt, 1);
	else
		setRegister(dasm->rt, 0);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate unsigned. */
MIPS_INSTRUCTION( SLTIU )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(((mipsRegister_u)readRegister(dasm->rs)) < imm)
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
	u32 result = readRegister(dasm->rt) << dasm->shift;
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift left logical variable. */
MIPS_INSTRUCTION( SLLV )
{
	u32 result = readRegister(dasm->rt) << readRegister(dasm->rs);
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical. */
MIPS_INSTRUCTION( SRL )
{
	u32 result = readRegister(dasm->rt) >> dasm->shift;
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical variable. */
MIPS_INSTRUCTION( SRLV )
{
	u32 result = readRegister(dasm->rt) >> readRegister(dasm->rs);
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right arithmetic. */
MIPS_INSTRUCTION( SRA )
{
	u32 result = readRegister(dasm->rt) >> dasm->shift;
	result |= readRegister(dasm->rt) << (32 - dasm->shift);
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right arithmetic variable. */
MIPS_INSTRUCTION( SRAV )
{
	u32 result = readRegister(dasm->rt) >> readRegister(dasm->rs);
	result |= readRegister(dasm->rt) << (32 - readRegister(dasm->rs));
	s64 res = _signextend(result, 32, 64);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift left logical. */
MIPS_INSTRUCTION( DSLL )
{
	s64 res = readRegister(dasm->rt) << dasm->shift;
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift left logical variable. */
MIPS_INSTRUCTION( DSLLV )
{
	s64 res = readRegister(dasm->rt) << readRegister(dasm->rs);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right logical. */
MIPS_INSTRUCTION( DSRL )
{
	s64 res = readRegister(dasm->rt) >> dasm->shift;
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right logical variable. */
MIPS_INSTRUCTION( DSRLV )
{
	s64 res = readRegister(dasm->rt) >> readRegister(dasm->rs);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right arithmetic. */
MIPS_INSTRUCTION( DSRA )
{
	s64 res = readRegister(dasm->rt) >> dasm->shift;
	res |= readRegister(dasm->rt) << (64 - dasm->shift);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right arithmetic variable. */
MIPS_INSTRUCTION( DSRAV )
{
	s64 res = readRegister(dasm->rt) >> readRegister(dasm->rs);
	res |= readRegister(dasm->rt) << (64 - readRegister(dasm->rs));
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/* Double-word shift left logical + 32. */
MIPS_INSTRUCTION( DSLL32 )
{
	s64 res = readRegister(dasm->rt) << (dasm->shift + 32);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right logical + 32. */
MIPS_INSTRUCTION( DSRL32 )
{
	s64 res = readRegister(dasm->rt) >> (dasm->shift + 32);
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word shift right arithmetic + 32. */
MIPS_INSTRUCTION( DSRA32 )
{
	s64 res = readRegister(dasm->rt) >> (dasm->shift + 32);
	res |= readRegister(dasm->rt) << (64 - (dasm->shift + 32));
	setRegister(dasm->rd, res);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Conditional Branch instructions                                          */
/*****************************************************************************/
/* Branch on equal. */
MIPS_INSTRUCTION( BEQ )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt))
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch on equal likely. */
MIPS_INSTRUCTION( BEQL )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt)) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNE )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt))
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNEL )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt)) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero. */
MIPS_INSTRUCTION( BGEZ )
{
	if(((s64)readRegister(dasm->rs)) >= 0)
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero likely. */
MIPS_INSTRUCTION( BGEZL )
{
	if(((s64)readRegister(dasm->rs)) >= 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
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
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero and link likely. */
MIPS_INSTRUCTION( BGEZALL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) >= 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater than zero. */
MIPS_INSTRUCTION( BGTZ )
{
	if(readRegister(dasm->rs) > 0)
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater than zero likely. */
MIPS_INSTRUCTION( BGTZL )
{
	if(readRegister(dasm->rs) > 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less or equal than zero. */
MIPS_INSTRUCTION( BLEZ )
{
	if(readRegister(dasm->rs) <= 0)
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less or equal than zero likely. */
MIPS_INSTRUCTION( BLEZL )
{
	if(readRegister(dasm->rs) <= 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero. */
MIPS_INSTRUCTION( BLTZ )
{
	if(readRegister(dasm->rs) < 0)
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero likely. */
MIPS_INSTRUCTION( BLTZL )
{
	if(readRegister(dasm->rs) < 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
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
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
	else
		advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero and link likely. */
MIPS_INSTRUCTION( BLTZALL )
{
	doLink(0, 0);
	if(readRegister(dasm->rs) < 0) {
		advancePC(_signextend(dasm->immediate << 2, 18, 64));
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
	s64 imm = _signextend(dasm->immediate, 16, 64);
	s8 value = emulatedCpu.readByte(readRegister(dasm->rs) + imm);
	setRegister(dasm->rt, _signextend(value, 8, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load byte unsigned. */
MIPS_INSTRUCTION( LBU )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u8 value = emulatedCpu.readByte(readRegister(dasm->rs) + imm);
	setRegister(dasm->rt, _zeroextend(value, 8, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword. */
MIPS_INSTRUCTION( LH )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 2)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	s16 value = emulatedCpu.readHword(off);
	setRegister(dasm->rt, _signextend(value, 16, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword unsigned. */
MIPS_INSTRUCTION( LHU )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 2)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	u16 value = emulatedCpu.readHword(off);
	setRegister(dasm->rt, _zeroextend(value, 16, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load upper immediate. */
MIPS_INSTRUCTION( LUI )
{
	s64 val = _signextend(dasm->immediate << 16, 32, 64);
	setRegister(dasm->rt, val);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word. */
MIPS_INSTRUCTION( LW )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = _zeroextend(readRegister(dasm->rs) + imm, 32, 64);
	if(off % 4)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	s32 value = emulatedCpu.readWord(off);
	s64 out = _signextend(value, 32, 64);
	setRegister(dasm->rt, out);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word left. */
MIPS_INSTRUCTION( LWL )
{
	s64 final = 0;
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
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
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (i * 4);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word unsigned. */
MIPS_INSTRUCTION( LWU )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 4)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	u32 value = emulatedCpu.readWord(off);
	setRegister(dasm->rt, _zeroextend(value, 32, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword. */
MIPS_INSTRUCTION( LD )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 8)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	s64 value = emulatedCpu.readDword(off);
	setRegister(dasm->rt, value);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load doubleword left. */
MIPS_INSTRUCTION( LDL )
{
	s64 final = 0;
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
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
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= emulatedCpu.readByte(vAddr + i) << (i * 8);
	}
	setRegister(dasm->rt, final);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load linked (word). */
MIPS_INSTRUCTION( LL )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 4)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
#ifdef DEBUG
	printf("Instruction LL not properly supported!\n");
#endif
	/* TODO: We need to make this call checkable by SC. */
	s32 value = emulatedCpu.readWord(off);
	setRegister(dasm->rt, _signextend(value, 32, 64));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load linked doubleword. */
MIPS_INSTRUCTION( LLD )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 8)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
#ifdef DEBUG
	printf("Instruction LLD not properly supported!\n");
#endif
	/* TODO: We need to make this call checkable by SCD. */
	s64 value = emulatedCpu.readDword(off);
	setRegister(dasm->rt, value);
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Data Store instructions                                                  */
/*****************************************************************************/
/* Store byte. */
MIPS_INSTRUCTION( SB )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	emulatedCpu.writeByte(readRegister(dasm->rs) + imm, readRegister(dasm->rt) & ((1 << 8) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word. */
MIPS_INSTRUCTION( SH )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 2)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	emulatedCpu.writeHword(off, readRegister(dasm->rt) & ((1 << 16) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word. */
MIPS_INSTRUCTION( SW )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 4)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	emulatedCpu.writeWord(off, readRegister(dasm->rt) & ((1LL << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word left. */
MIPS_INSTRUCTION( SWL )
{
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (32 - ((i + 1) * 4)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word right. */
MIPS_INSTRUCTION( SWR )
{
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (i * 4));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword. */
MIPS_INSTRUCTION( SD )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 8)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	emulatedCpu.writeDword(off, readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword left. */
MIPS_INSTRUCTION( SDL )
{
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (64 - ((i + 1) * 8)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store doubleword right. */
MIPS_INSTRUCTION( SDR )
{
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(dasm->rs) + imm;
	u64 regval = readRegister(dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		emulatedCpu.writeByte(vAddr + i, regval >> (i * 8));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store conditional. */
MIPS_INSTRUCTION( SC )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 4)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	u64 pass;
#ifdef DEBUG
	printf("Instruction SC not properly supported!\n");
#endif
	/* TODO: We need to check to see if the loacation that was loaded earlier was modified */
	/* Until we do so, this call will be set to FAIL every time! */
	pass = 0;
	setRegister(dasm->rt, pass);
	if(pass)
		emulatedCpu.writeWord(off, readRegister(dasm->rt) & ((1LL << 32) - 1));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store conditional double-word. */
MIPS_INSTRUCTION( SCD )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(dasm->rs) + imm;
	if(off % 8)
		generateException(EXCEPTION_ADDRESS, dasm->delay);
	u64 pass;
#ifdef DEBUG
	printf("Instruction SCD not properly supported!\n");
#endif
	/* TODO: We need to check to see if the loacation that was loaded earlier was modified */
	/* Until we do so, this call will be set to FAIL every time! */
	pass = 0;
	setRegister(dasm->rt, pass);
	if(pass)
		emulatedCpu.writeDword(off, readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Special register instructions                                            */
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

/* Move to HI. */
MIPS_INSTRUCTION( MTHI )
{
	setRegister(REGISTER_HI, readRegister(dasm->rd));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move to LO. */
MIPS_INSTRUCTION( MTLO )
{
	setRegister(REGISTER_LO, readRegister(dasm->rd));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Special instructions                                                     */
/*****************************************************************************/
/* Syscall. */
MIPS_INSTRUCTION( SYSCALL )
{
	generateException(EXCEPTION_SYSCALL, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/* Breakpoint. */
MIPS_INSTRUCTION( BREAK )
{
	generateException(EXCEPTION_BREAKPOINT, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/* Synchronize. */
MIPS_INSTRUCTION( SYNC )
{
#ifdef DEBUG
	printf("Instruction SYNC unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/* Cache. */
MIPS_INSTRUCTION( CACHE )
{
#ifdef DEBUG
	printf("Instruction CACHE unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Trap instructions                                                        */
/*****************************************************************************/
/* Trap if Equal. */
MIPS_INSTRUCTION( TEQ )
{
	if(readRegister(dasm->rs) == readRegister(dasm->rt))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Equal Immediate. */
MIPS_INSTRUCTION( TEQI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(readRegister(dasm->rs) == imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Not Equal. */
MIPS_INSTRUCTION( TNE )
{
	if(readRegister(dasm->rs) != readRegister(dasm->rt))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Not Equal Immediate. */
MIPS_INSTRUCTION( TNEI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(readRegister(dasm->rs) != imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Greater than or Equal. */
MIPS_INSTRUCTION( TGE )
{
	if(readRegister(dasm->rs) >= readRegister(dasm->rt))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Greater than or Equal unsigned. */
MIPS_INSTRUCTION( TGEU )
{
	if(((mipsRegister_u)readRegister(dasm->rs)) >= ((mipsRegister_u)readRegister(dasm->rt)))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Greater than or Equal Immediate. */
MIPS_INSTRUCTION( TGEI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(readRegister(dasm->rs) >= imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Greater than or Equal Immediate unsigned. */
MIPS_INSTRUCTION( TGEIU )
{
	u64 imm = _zeroextend(dasm->immediate, 16, 64);
	if(((mipsRegister_u)readRegister(dasm->rs)) >= imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Less than. */
MIPS_INSTRUCTION( TLT )
{
	if(readRegister(dasm->rs) < readRegister(dasm->rt))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Less than unsigned. */
MIPS_INSTRUCTION( TLTU )
{
	if(((mipsRegister_u)readRegister(dasm->rs)) < ((mipsRegister_u)readRegister(dasm->rt)))
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Less than Immediate. */
MIPS_INSTRUCTION( TLTI )
{
	s64 imm = _signextend(dasm->immediate, 16, 64);
	if(readRegister(dasm->rs) < imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Trap if Less than Immediate unsigned. */
MIPS_INSTRUCTION( TLTIU )
{
	u64 imm = _zeroextend(dasm->immediate, 16, 64);
	if(((mipsRegister_u)readRegister(dasm->rs)) < imm)
		generateException(EXCEPTION_TRAP, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  COP0 instructions                                                        */
/*****************************************************************************/
/* Probe TLB for matching entry. */
MIPS_INSTRUCTION( TLBP )
{
#ifdef DEBUG
	printf("Instruction TLBP unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Read Indexed TLB entry. */
MIPS_INSTRUCTION( TLBR )
{
#ifdef DEBUG
	printf("Instruction TLBR unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Write Indexed TLB entry. */
MIPS_INSTRUCTION( TLBWI )
{
#ifdef DEBUG
	printf("Instruction TLBWI unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Write Random TLB entry. */
MIPS_INSTRUCTION( TLBWR )
{
#ifdef DEBUG
	printf("Instruction TLBWR unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Exception return. */
MIPS_INSTRUCTION( ERET )
{
#ifdef DEBUG
	printf("Instruction ERET unsupported!\n");
#endif
	/* TODO: Implement */
	/* We need to kill all the LL/SC pairs here too */
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Coprocessor instructions                                                 */
/*****************************************************************************/
/* Load double-word to coprocessor 1. */
MIPS_INSTRUCTION( LDC1 )
{
#ifdef DEBUG
	printf("Instruction LDC1 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load double-word to coprocessor 2. */
MIPS_INSTRUCTION( LDC2 )
{
#ifdef DEBUG
	printf("Instruction LDC2 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word to coprocessor 1. */
MIPS_INSTRUCTION( LWC1 )
{
#ifdef DEBUG
	printf("Instruction LWC1 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word to coprocessor 2. */
MIPS_INSTRUCTION( LWC2 )
{
#ifdef DEBUG
	printf("Instruction LWC2 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store double-word to coprocessor 1. */
MIPS_INSTRUCTION( SDC1 )
{
#ifdef DEBUG
	printf("Instruction SDC1 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store double-word to coprocessor 2. */
MIPS_INSTRUCTION( SDC2 )
{
#ifdef DEBUG
	printf("Instruction SDC2 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word to coprocessor 1. */
MIPS_INSTRUCTION( SWC1 )
{
#ifdef DEBUG
	printf("Instruction SWC1 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store word to coprocessor 2. */
MIPS_INSTRUCTION( SWC2 )
{
#ifdef DEBUG
	printf("Instruction SWC2 unsupported!\n");
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  Coprocessor sub-instructions                                             */
/*****************************************************************************/
/* Move from coprocessor z. */
MIPS_COP_INSTRUCTION( MFCz )
{
#ifdef DEBUG
	printf("Instruction MFC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move to coprocessor z. */
MIPS_COP_INSTRUCTION( MTCz )
{
#ifdef DEBUG
	printf("Instruction MTC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word move from coprocessor z. */
MIPS_COP_INSTRUCTION( DMFCz )
{
#ifdef DEBUG
	printf("Instruction DMFC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Double-word move to coprocessor z. */
MIPS_COP_INSTRUCTION( DMTCz )
{
#ifdef DEBUG
	printf("Instruction DMTC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move control from coprocessor z. */
MIPS_COP_INSTRUCTION( CFCz )
{
#ifdef DEBUG
	printf("Instruction CFC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move control to coprocessor z. */
MIPS_COP_INSTRUCTION( CTCz )
{
#ifdef DEBUG
	printf("Instruction CTC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Coprocessor branch instructions                                          */
/*****************************************************************************/
/* Branch on coprocessor z false. */
MIPS_COP_INSTRUCTION( BCzF )
{
#ifdef DEBUG
	printf("Instruction BC%dF unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch on coprocessor z false likely. */
MIPS_COP_INSTRUCTION( BCzFL )
{
#ifdef DEBUG
	printf("Instruction BC%dFL unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch on coprocessor z true. */
MIPS_COP_INSTRUCTION( BCzT )
{
#ifdef DEBUG
	printf("Instruction BC%dT unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch on coprocessor z true likely. */
MIPS_COP_INSTRUCTION( BCzTL )
{
#ifdef DEBUG
	printf("Instruction BC%dTL unsupported!\n", cop);
#endif
	/* TODO: Implement */
	advancePC(DEFAULT_INSTRUCTION_PC);
}


/*****************************************************************************/
/*  No-operation instructions                                                */
/*****************************************************************************/
/* Reserved No Operation (holds place for non-exception reserved slots). */
MIPS_INSTRUCTION( RNOP )
{
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Reserved No Operation (holds place for non-exception reserved slots). */
MIPS_COP_INSTRUCTION( RNOP )
{
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Reserved No Operation (holds place for exception-raising reserved slots). */
MIPS_INSTRUCTION( RESV )
{
	generateException(EXCEPTION_RESERVED, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Reserved No Operation (holds place for exception-raising reserved slots). */
MIPS_COP_INSTRUCTION( RESV )
{
	generateException(EXCEPTION_RESERVED, dasm->delay);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/*****************************************************************************/
/*  Instruction Tables.                                                      */
/*****************************************************************************/
/* General table with all opcodes. */
mipsInstrTbl instructionTable[] = {
/*00*/	INST_ENTRY( RNOP,	"",		0 ), /* Special */
/*01*/	INST_ENTRY( RNOP,	"",		0 ), /* Regimm */
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
/*0E*/	INST_ENTRY( XORI,	" %t, %s, %i",	0 ),
/*0F*/	INST_ENTRY( LUI,	" %t, %i",	0 ),
/*10*/	INST_ENTRY( RNOP,	"",		0 ), /* COP0 */
/*11*/	INST_ENTRY( RNOP,	"",		0 ), /* COP1 */
/*12*/	INST_ENTRY( RNOP,	"",		0 ), /* COP2 */
/*13*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*14*/	INST_ENTRY( BEQL,	" %s, %t, %i",	1 ), 
/*15*/	INST_ENTRY( BNEL,	" %s, %t, %i",	1 ), 
/*16*/	INST_ENTRY( BLEZL,	" %s, %i",	1 ), 
/*17*/	INST_ENTRY( BGTZL,	" %s, %i",	1 ), 
/*18*/	INST_ENTRY( DADDI,	" %t, %s, %i",	0 ), 
/*18*/	INST_ENTRY( DADDIU,	" %t, %s, %i",	0 ), 
/*1A*/	INST_ENTRY( LDL,	" %t, %i(%s)",	0 ), 
/*1B*/	INST_ENTRY( LDR,	" %t, %i(%s)",	0 ), 
/*1C*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1D*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1E*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1F*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
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
/*2F*/	INST_ENTRY( CACHE,	" %t, %i(%s)",	0 ),
/*30*/	INST_ENTRY( LL,		" %t, %i(%s)",	0 ),
/*31*/	INST_ENTRY( LWC1,	" %t, %i(%s)",	0 ),
/*32*/	INST_ENTRY( LWC2,	" %t, %i(%s)",	0 ),
/*33*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*34*/	INST_ENTRY( LLD,	" %t, %i(%s)",	0 ),
/*35*/	INST_ENTRY( LDC1,	" %t, %i(%s)",	0 ), 
/*36*/	INST_ENTRY( LDC2,	" %t, %i(%s)",	0 ), 
/*37*/	INST_ENTRY( LD,		" %t, %i(%s)",	0 ),
/*38*/	INST_ENTRY( SC,		" %t, %i(%s)",	0 ),
/*39*/	INST_ENTRY( SWC1,	" %t, %i(%s)",	0 ),
/*3A*/	INST_ENTRY( SWC2,	" %t, %i(%s)",	0 ),
/*3B*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*3C*/	INST_ENTRY( SCD,	" %t, %i(%s)",	0 ),
/*3D*/	INST_ENTRY( SDC1,	" %t, %i(%s)",	0 ),
/*3E*/	INST_ENTRY( SDC2,	" %t, %i(%s)",	0 ),
/*3F*/	INST_ENTRY( SD,		" %t, %i(%s)",	0 ),
};

/* Special opcodes.
 * The first six bits are 000000 and the instruction is encoded in the last six bits. */

mipsInstrTbl specialInstructionTable[] = {
/*00*/	INST_ENTRY( SLL,	" %d, %t, %h",	0 ), 
/*01*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*02*/	INST_ENTRY( SRL,	" %d, %t, %h",	0 ), 
/*03*/	INST_ENTRY( SRA,	" %d, %t, %h",	0 ), 
/*04*/	INST_ENTRY( SLLV,	" %d, %t, %s",	0 ), 
/*05*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*06*/	INST_ENTRY( SRLV,	" %d, %t, %s",	0 ), 
/*07*/	INST_ENTRY( SRAV,	" %d, %t, %s",	0 ),
/*08*/	INST_ENTRY( JR,		" %s",		1 ), 
/*09*/	INST_ENTRY( JALR,	" %s",		1 ), 
/*0A*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*0B*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*0C*/	INST_ENTRY( SYSCALL,	"",		0 ), 
/*0D*/	INST_ENTRY( BREAK,	"",		0 ), 
/*0E*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*0F*/	INST_ENTRY( SYNC,	"",		0 ),
/*10*/	INST_ENTRY( MFHI,	" %d",		0 ), 
/*11*/	INST_ENTRY( MTHI,	" %s",		0 ),
/*12*/	INST_ENTRY( MFLO,	" %d",		0 ), 
/*13*/	INST_ENTRY( MTLO,	" %s",		0 ),
/*14*/	INST_ENTRY( DSLLV,	" %d, %t, %s",	0 ), 
/*15*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*16*/	INST_ENTRY( DSRLV,	" %d, %t, %h",	0 ), 
/*17*/	INST_ENTRY( DSRAV,	" %d, %t, %s",	0 ), 
/*18*/	INST_ENTRY( MULT,	" %s, %t",	0 ), 
/*19*/	INST_ENTRY( MULTU,	" %s, %t",	0 ), 
/*1A*/	INST_ENTRY( DIV,	" %s, %t",	0 ), 
/*1B*/	INST_ENTRY( DIVU,	" %s, %t",	0 ), 
/*1C*/	INST_ENTRY( DMULT,	" %s, %t",	0 ),
/*1D*/	INST_ENTRY( DMULTU,	" %s, %t",	0 ),
/*1E*/	INST_ENTRY( DDIV,	" %s, %t",	0 ),
/*1F*/	INST_ENTRY( DDIVU,	" %s, %t",	0 ),
/*20*/	INST_ENTRY( ADD,	" %d, %s, %t",	0 ), 
/*21*/	INST_ENTRY( ADDU,	" %d, %s, %t",	0 ), 
/*22*/	INST_ENTRY( SUB,	" %d, %s, %t",	0 ), 
/*23*/	INST_ENTRY( SUBU,	" %d, %s, %t",	0 ), 
/*24*/	INST_ENTRY( AND,	" %d, %s, %t",	0 ), 
/*25*/	INST_ENTRY( OR,		" %d, %s, %t",	0 ), 
/*26*/	INST_ENTRY( XOR,	" %d, %s, %t",	0 ), 
/*27*/	INST_ENTRY( NOR,	" %d, %s, %t",	0 ), 
/*28*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*29*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*2A*/	INST_ENTRY( SLT,	" %d, %s, %t",	0 ), 
/*2B*/	INST_ENTRY( SLTU,	" %d, %s, %t",	0 ), 
/*2C*/	INST_ENTRY( DADD,	" %d, %s, %t",	0 ), 
/*2D*/	INST_ENTRY( DADDU,	" %d, %s, %t",	0 ), 
/*2E*/	INST_ENTRY( DSUB,	" %d, %s, %t",	0 ),
/*2F*/	INST_ENTRY( DSUBU,	" %d, %s, %t",	0 ),
/*30*/	INST_ENTRY( TGE,	" %s, %t",	0 ),
/*31*/	INST_ENTRY( TGEU,	" %s, %t",	0 ),
/*32*/	INST_ENTRY( TLT,	" %s, %t",	0 ),
/*33*/	INST_ENTRY( TLTU,	" %s, %t",	0 ),
/*34*/	INST_ENTRY( TEQ,	" %s, %t",	0 ),
/*35*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*36*/	INST_ENTRY( TNE,	" %s, %t",	0 ),
/*37*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*38*/	INST_ENTRY( DSLL,	" %d, %t, %h",	0 ), 
/*39*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*3A*/	INST_ENTRY( DSRL,	" %d, %t, %h",	0 ), 
/*3B*/	INST_ENTRY( DSRA,	" %d, %t, %h",	0 ), 
/*3C*/	INST_ENTRY( DSLL32,	" %d, %t, %h",	0 ), 
/*3D*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*3C*/	INST_ENTRY( DSRL32,	" %d, %t, %h",	0 ), 
/*3F*/	INST_ENTRY( DSRA32,	" %d, %t, %h",	0 ), 
};

/* Regimm opcodes.
 * The first six bits are 000001 and the instruction is encoded in the t register space. */

mipsInstrTbl regimmInstructionTable[] = {
/*00*/	INST_ENTRY( BLTZ,	" %s, %i",	1 ), 
/*01*/	INST_ENTRY( BGEZ,	" %s, %i",	1 ), 
/*02*/	INST_ENTRY( BLTZL,	" %s, %i",	1 ), 
/*03*/	INST_ENTRY( BGEZL,	" %s, %i",	1 ), 
/*04*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*05*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*06*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*07*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*08*/	INST_ENTRY( TGEI,	" %s, %i",	0 ),
/*09*/	INST_ENTRY( TGEIU,	" %s, %i",	0 ),
/*0A*/	INST_ENTRY( TLTI,	" %s, %i",	0 ),
/*0B*/	INST_ENTRY( TLTIU,	" %s, %i",	0 ),
/*0C*/	INST_ENTRY( TEQI,	" %s, %i",	0 ),
/*0D*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*0E*/	INST_ENTRY( TNEI,	" %s, %i",	0 ),
/*0F*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*10*/	INST_ENTRY( BLTZAL,	" %s, %i",	1 ), 
/*11*/	INST_ENTRY( BGEZAL,	" %s, %i",	1 ), 
/*12*/	INST_ENTRY( BLTZALL,	" %s, %i",	1 ), 
/*13*/	INST_ENTRY( BGEZALL,	" %s, %i",	1 ), 
/*14*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*15*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*16*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*17*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*18*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*19*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1A*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1B*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1C*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1D*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1E*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*1F*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
};

/* Coprocessor opcodes.
 * The first six bits are 0100xx (where xx is the cop) and the instruction is encoded 
 * in the s register space. */

mipsCopInstrTbl coprocInstructionTable[] = {
/*00*/	COP_INST_ENTRY( MFCz,		" %t, %d",	0 ),
/*01*/	COP_INST_ENTRY( DMFCz,		" %t, %d",	0 ),
/*02*/	COP_INST_ENTRY( CFCz,		" %t, %d",	0 ),
/*03*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*04*/	COP_INST_ENTRY( MTCz,		" %t, %d",	0 ),
/*05*/	COP_INST_ENTRY( DMTCz,		" %t, %d",	0 ),
/*06*/	COP_INST_ENTRY( CTCz,		" %t, %d",	0 ),
/*07*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*08*/	COP_INST_ENTRY( RNOP,		"",		0 ), /* BC */
/*09*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0A*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0B*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0C*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0D*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0E*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0F*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*10*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*11*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*12*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*13*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*14*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*15*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*16*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*17*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*18*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*19*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1A*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1B*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1C*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1D*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1E*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1F*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
};

/* Coprocessor 0 opcodes.
 * The first seven bits are 0100001 and the instruction is encoded 
 * in the t register space. */
/* COP0 needs to go somewhere >.> */

mipsInstrTbl cop0InstructionTable[] = {
/*00*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*01*/	INST_ENTRY( TLBR,	"",		0 ), 
/*02*/	INST_ENTRY( TLBWI,	"",		0 ), 
/*03*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*04*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*05*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*06*/	INST_ENTRY( TLBWR,	"",		0 ),
/*07*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*08*/	INST_ENTRY( TLBP,	"",		0 ),
/*09*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0A*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0B*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0C*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0D*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0E*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*0F*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*10*/	INST_ENTRY( RESV,	"",		0 ), /* Nothing */
/*11*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*12*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*13*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*14*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*15*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*16*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*17*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*18*/	INST_ENTRY( ERET,	"",		0 ),
/*19*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1A*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1B*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1C*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1D*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1E*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
/*1F*/	INST_ENTRY( RNOP,	"",		0 ), /* Nothing */
};

/* Coprocessor branch opcodes.
 * The first eleven bits are 0100xx01000 (where xx is the cop) and the instruction is encoded 
 * in the t register space. */

mipsCopInstrTbl coprocBcInstructionTable[] = {
/*00*/	COP_INST_ENTRY( BCzF,		" %i",		0 ),
/*01*/	COP_INST_ENTRY( BCzT,		" %i",		0 ),
/*02*/	COP_INST_ENTRY( BCzFL,		" %i",		0 ),
/*03*/	COP_INST_ENTRY( BCzTL,		" %i",		0 ),
/*04*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*05*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*06*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*07*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*08*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*09*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0A*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0B*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0C*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0D*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0E*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*0F*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*10*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*11*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*12*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*13*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*14*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*15*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*16*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*17*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*18*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*19*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1A*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1B*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1C*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1D*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1E*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
/*1F*/	COP_INST_ENTRY( RESV,		"",		0 ), /* Nothing */
};

void execOpcode(mipsDasm *dasm)
{
#ifdef DEBUG		
	printf("Instruction 0x%08X Function 0x%08X\n", dasm->instruction, dasm->funct);
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
		if(readRegister(dasm->rt) <= REGIMM_INST_COUNT) {
			regimmInstructionTable[readRegister(dasm->rt)].execute(dasm);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(readRegister(dasm->rs) <= COPROC_INST_COUNT) {
			if(readRegister(dasm->rs) == 0x8) {
				if(readRegister(dasm->rs) <= COPROC_BC_INST_COUNT) {
					(coprocBcInstructionTable[readRegister(dasm->rs)].execute == MIPS_COP_INSTR_NAME(RNOP)) ? \
						copxHandler(dasm) : coprocBcInstructionTable[readRegister(dasm->rs)].execute(dasm, dasm->instruction & 0x3);
				}else{
#ifdef DEBUG
					printf("rt is too high!\n");
#endif
				}
			}else if(readRegister(dasm->rs) & 0x10) {
				if(dasm->funct <= COPROC_COP0_INST_COUNT) {
					(cop0InstructionTable[dasm->funct].execute == MIPS_INSTR_NAME(RNOP)) ? \
						cop0Handler(dasm) : cop0InstructionTable[dasm->funct].execute(dasm);
				}else{
#ifdef DEBUG
					printf("Function is too high!\n");
#endif
				}
			}else{
				(coprocInstructionTable[readRegister(dasm->rs)].execute == MIPS_COP_INSTR_NAME(RNOP)) ? \
					copxHandler(dasm) : coprocInstructionTable[readRegister(dasm->rs)].execute(dasm, dasm->instruction & 0x3);
			}
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
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
		if(readRegister(dasm->rt) <= REGIMM_INST_COUNT) {
			return dasmFormat(regimmInstructionTable[readRegister(dasm->rt)].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
			return "Not implemented";
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(readRegister(dasm->rs) <= COPROC_INST_COUNT) {
			if(readRegister(dasm->rs) == 0x8)
				return dasmFormat(coprocBcInstructionTable[readRegister(dasm->rt)].textDisasm, dasm);
			else if(readRegister(dasm->rs) & 0x10)
				return dasmFormat(cop0InstructionTable[dasm->funct].textDisasm, dasm);
			else
				return dasmFormat(coprocInstructionTable[readRegister(dasm->rs)].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
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
