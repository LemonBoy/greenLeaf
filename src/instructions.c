#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"
#include "instructions.h"

#define MIPS_INSTR_NAME(name)				mips_##name
#define MIPS_INSTR_EXEC(name)				MIPS_INSTR_NAME(name)(cpu, dasm);
#define MIPS_INSTRUCTION(name)				static mipsInstruction MIPS_INSTR_NAME(name) (mipsCpu* cpu, mipsDasm *dasm)
#define INST_ENTRY(name, args, delay, cyc, pc)		{ MIPS_INSTR_NAME(name), #name args, delay, cyc, pc } 

#define MIPS_COP_INSTR_NAME(name)			mips_cop_##name
#define MIPS_COP_INSTR_EXEC(name)			MIPS_COP_INSTR_NAME(name)(cpu, dasm, cop);
#define MIPS_COP_INSTRUCTION(name)			static mipsInstruction MIPS_COP_INSTR_NAME(name)(mipsCpu* cpu, mipsDasm *dasm, int cop)
#define COP_INST_ENTRY(name, args, delay, cyc, pc)	{ MIPS_COP_INSTR_NAME(name), #name args, delay, cyc, pc } 

#define MAXMASK(b)	(((1LL << ((b) - 1)) - 1) + (1LL << ((b) - 1)))

/* #define DOJUMP(loc, size)	setJump(cpu, loc) */
#define DOJUMP(loc, size)	setJump(cpu, _zeroextend(loc << 2, size + 2, BITCOUNT))
/* #define BRANCH(loc, size)	setJump(cpu, getPC(cpu) + loc) */
#define BRANCH(loc, size)	setJump(cpu, getPC(cpu) + _signextend(loc << 2, size + 2, BITCOUNT))

void cop0Handler(mipsCpu* cpu, mipsDasm *dasm)
{
	switch(readRegister(cpu, dasm->rs)) {
		case 0x0:
			setRegister(cpu, dasm->rt, readCopRegister(cpu, 0, dasm->rd));
			break;
		case 0x4:
			setCopRegister(cpu, 0, dasm->rd, (u32)(dasm->rt));
			break;
	}
	
	advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

void copxHandler(mipsCpu* cpu, mipsDasm *dasm)
{
	switch(readRegister(cpu, dasm->rs)) {
		case 0x0:
			setRegister(cpu, dasm->rt, readCopRegister(cpu, 0, dasm->rd));
			break;
		case 0x4:
			setCopRegister(cpu, 0, dasm->rd, (u32)(dasm->rt));
			break;
	}
	
	advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

INLINE static mipsRegister _signextend(mipsRegister_u input, int origsize, int newsize)
{
	if(origsize == newsize)
		return input;
	int s = (((input & (1LL << (origsize - 1LL))) > 0LL) ? 1 : 0);
	mipsRegister_u mask;
	if(s) {
		mask = (0LL - 1LL) & ~MAXMASK(origsize);
		input |= mask;
	}else{
		mask = MAXMASK(origsize);
		input &= mask;
	}
	return input;
}

INLINE static mipsRegister _zeroextend(mipsRegister_u input, int origsize, int newsize)
{
	if(origsize == newsize)
		return input;
	return input &= MAXMASK(origsize);
}

/*
 * Start of the emulated functions.
 * All the opcode translations should go there.
 */

/*****************************************************************************/
/*  No-operation instructions                                                */
/*****************************************************************************/
/* Reserved No Operation (holds place for non-exception reserved slots). */
MIPS_INSTRUCTION( RNOP )
{
}

/* Reserved No Operation (holds place for non-exception reserved slots). */
MIPS_COP_INSTRUCTION( RNOP )
{
}

/* Reserved No Operation (holds place for exception-raising reserved slots). */
MIPS_INSTRUCTION( RESV )
{
	generateException(cpu, EXCEPTION_RESERVEDINSTR, dasm->delay);
}

/* Reserved No Operation (holds place for exception-raising reserved slots). */
MIPS_COP_INSTRUCTION( RESV )
{
	generateException(cpu, EXCEPTION_RESERVEDINSTR, dasm->delay);
}


/*****************************************************************************/
/*  Arithmetic instructions                                                  */
/*****************************************************************************/
/* Add with overflow. */
MIPS_INSTRUCTION( ADD )
{
	s32 val = readRegister(cpu, dasm->rs) + readRegister(cpu, dasm->rt);
	mipsRegister out = _signextend(val, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, out);
}

/* Add unsigned. */
MIPS_INSTRUCTION( ADDU )
{
	u32 val = readRegister(cpu, dasm->rs) + readRegister(cpu, dasm->rt);
	mipsRegister_u out = _zeroextend(val, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, out);
}

/* Add immediate. */
MIPS_INSTRUCTION( ADDI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	s32 val = readRegister(cpu, dasm->rs) + imm;
	mipsRegister out = _signextend(val, 32, BITCOUNT);
	setRegister(cpu, dasm->rt, out);
}

/* Add immediate unsigned. */
MIPS_INSTRUCTION( ADDIU )
{
	mipsRegister_u imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	u32 val = ((u32)readRegister(cpu, dasm->rs)) + imm;
	mipsRegister_u out = _zeroextend(val, 32, BITCOUNT);
	setRegister(cpu, dasm->rt, out);
}

/* Double-word add with overflow. */
MIPS_INSTRUCTION( DADD )
{
#if BITCOUNT == 64
	s64 val = readRegister(cpu, dasm->rs) + readRegister(cpu, dasm->rt);
	setRegister(cpu, dasm->rd, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word add unsigned. */
MIPS_INSTRUCTION( DADDU )
{
#if BITCOUNT == 64
	u64 val = readRegister(cpu, dasm->rs) + readRegister(cpu, dasm->rt);
	setRegister(cpu, dasm->rd, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word add immediate. */
MIPS_INSTRUCTION( DADDI )
{
#if BITCOUNT == 64
	s64 imm = _signextend(dasm->immediate, 16, 64);
	s64 val = readRegister(cpu, dasm->rs) + imm;
	setRegister(cpu, dasm->rt, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word add immediate unsigned. */
MIPS_INSTRUCTION( DADDIU )
{
#if BITCOUNT == 64
	s64 imm = _zeroextend(dasm->immediate, 16, 64);
	u64 val = readRegister(cpu, dasm->rs) + imm;
	setRegister(cpu, dasm->rt, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Subtract with overflow. */
MIPS_INSTRUCTION( SUB )
{
	s32 tmp = readRegister(cpu, dasm->rs) - readRegister(cpu, dasm->rt);
	mipsRegister val = _signextend(tmp, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, val);
}

/* Subtract unsigned. */
MIPS_INSTRUCTION( SUBU )
{
	u32 tmp = readRegister(cpu, dasm->rs) - readRegister(cpu, dasm->rt);
	mipsRegister_u val = _zeroextend(tmp, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, val);
}

/* Double-word subtract with overflow. */
MIPS_INSTRUCTION( DSUB )
{
#if BITCOUNT == 64
	s64 val = readRegister(cpu, dasm->rs) - readRegister(cpu, dasm->rt);
	setRegister(cpu, dasm->rd, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word subtract unsigned. */
MIPS_INSTRUCTION( DSUBU )
{
#if BITCOUNT == 64
	u64 val = readRegister(cpu, dasm->rs) - readRegister(cpu, dasm->rt);
	setRegister(cpu, dasm->rd, val);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Multiply. */
MIPS_INSTRUCTION( MULT )
{
	s32 tmp = readRegister(cpu, dasm->rs) * readRegister(cpu, dasm->rt);
	mipsRegister hi  = _signextend(tmp, 32, BITCOUNT);
	setRegister(cpu, REGISTER_HI, hi);
}

/* Multiply unsigned. */
MIPS_INSTRUCTION( MULTU )
{
	u32 tmp = readRegister(cpu, dasm->rs) * readRegister(cpu, dasm->rt);
	mipsRegister_u hi  = _zeroextend(tmp, 32, BITCOUNT);
	setRegister(cpu, REGISTER_HI, hi);
}

/* Double-word multiply. */
MIPS_INSTRUCTION( DMULT )
{
#if BITCOUNT == 64
	s64 hi = readRegister(cpu, dasm->rs) * readRegister(cpu, dasm->rt);
	setRegister(cpu, REGISTER_HI, hi);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word multiply unsigned. */
MIPS_INSTRUCTION( DMULTU )
{
#if BITCOUNT == 64
	u64 hi = readRegister(cpu, dasm->rs) * readRegister(cpu, dasm->rt);
	setRegister(cpu, REGISTER_HI, hi);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Divide. */
MIPS_INSTRUCTION( DIV )
{
	s32          tmp = readRegister(cpu, dasm->rs) / readRegister(cpu, dasm->rt);
	mipsRegister hi  = _signextend(tmp, 32, BITCOUNT);
		     tmp = readRegister(cpu, dasm->rs) % readRegister(cpu, dasm->rt);
	mipsRegister lo  = _signextend(tmp, 32, BITCOUNT);
	setRegister(cpu, REGISTER_HI, hi);
	setRegister(cpu, REGISTER_LO, lo);
}

/* Divide unsigned. */
MIPS_INSTRUCTION( DIVU )
{
	u32            tmp = readRegister(cpu, dasm->rs) / readRegister(cpu, dasm->rt);
	mipsRegister_u hi  = _zeroextend(tmp, 32, BITCOUNT);
	               tmp = readRegister(cpu, dasm->rs) % readRegister(cpu, dasm->rt);
	mipsRegister_u lo  = _zeroextend(tmp, 32, BITCOUNT);
	setRegister(cpu, REGISTER_HI, hi);
	setRegister(cpu, REGISTER_LO, lo);
}

/* Double-word divide. */
MIPS_INSTRUCTION( DDIV )
{
#if BITCOUNT == 64
	s64 hi = readRegister(cpu, dasm->rs) / readRegister(cpu, dasm->rt);
	s64 lo = readRegister(cpu, dasm->rs) % readRegister(cpu, dasm->rt);
	setRegister(cpu, REGISTER_HI, hi);
	setRegister(cpu, REGISTER_LO, lo);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word divide unsigned. */
MIPS_INSTRUCTION( DDIVU )
{
#if BITCOUNT == 64
	u64 hi = readRegister(cpu, dasm->rs) / readRegister(cpu, dasm->rt);
	u64 lo = readRegister(cpu, dasm->rs) % readRegister(cpu, dasm->rt);
	setRegister(cpu, REGISTER_HI, hi);
	setRegister(cpu, REGISTER_LO, lo);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}


/*****************************************************************************/
/*  Logical instructions                                                     */
/*****************************************************************************/
/* And. */
MIPS_INSTRUCTION( AND )
{
	setRegister(cpu, dasm->rd, readRegister(cpu, dasm->rs) & readRegister(cpu, dasm->rt));
}

/* And immediate. */
MIPS_INSTRUCTION( ANDI )
{
	mipsRegister imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	setRegister(cpu, dasm->rt, readRegister(cpu, dasm->rs) & imm);
}

/* Or. */
MIPS_INSTRUCTION( OR )
{
	setRegister(cpu, dasm->rd, readRegister(cpu, dasm->rs) | readRegister(cpu, dasm->rt));
}

/* Or immediate. */
MIPS_INSTRUCTION( ORI )
{
	mipsRegister imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	setRegister(cpu, dasm->rt, readRegister(cpu, dasm->rs) | imm);
}

/* Exclusive or. */
MIPS_INSTRUCTION( XOR )
{
	setRegister(cpu, dasm->rd, readRegister(cpu, dasm->rs) ^ readRegister(cpu, dasm->rt));
}

/* Exclusive or immediate. */
MIPS_INSTRUCTION( XORI )
{
	mipsRegister imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	setRegister(cpu, dasm->rt, readRegister(cpu, dasm->rs) ^ imm);
}

/* Nor. */
MIPS_INSTRUCTION( NOR )
{
	setRegister(cpu, dasm->rd, ~(readRegister(cpu, dasm->rs) | readRegister(cpu, dasm->rt)));
}

/* Set if less than. */
MIPS_INSTRUCTION( SLT )
{
	if(readRegister(cpu, dasm->rs) < readRegister(cpu, dasm->rt))
		setRegister(cpu, dasm->rd, 1);
	else
		setRegister(cpu, dasm->rd, 0);
}

/* Set if less than immediate unsigned. */
MIPS_INSTRUCTION( SLTU )
{
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) < ((mipsRegister_u)readRegister(cpu, dasm->rt)))
		setRegister(cpu, dasm->rd, 1);
	else
		setRegister(cpu, dasm->rd, 0);
}

/* Set if less than immediate. */
MIPS_INSTRUCTION( SLTI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	if(readRegister(cpu, dasm->rs) < imm)
		setRegister(cpu, dasm->rt, 1);
	else
		setRegister(cpu, dasm->rt, 0);
}

/* Set if less than immediate unsigned. */
MIPS_INSTRUCTION( SLTIU )
{
	mipsRegister_u imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) < imm)
		setRegister(cpu, dasm->rt, 1);
	else
		setRegister(cpu, dasm->rt, 0);
}


/*****************************************************************************/
/*  Bitwise Shift instructions                                               */
/*****************************************************************************/
/* Shift left logical. */
MIPS_INSTRUCTION( SLL )
{
	u32 result = readRegister(cpu, dasm->rt) << dasm->shift;
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Shift left logical variable. */
MIPS_INSTRUCTION( SLLV )
{
	u32 result = readRegister(cpu, dasm->rt) << readRegister(cpu, dasm->rs);
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Shift right logical. */
MIPS_INSTRUCTION( SRL )
{
	u32 result = readRegister(cpu, dasm->rt) >> dasm->shift;
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Shift right logical variable. */
MIPS_INSTRUCTION( SRLV )
{
	u32 result = readRegister(cpu, dasm->rt) >> readRegister(cpu, dasm->rs);
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Shift right arithmetic. */
MIPS_INSTRUCTION( SRA )
{
	u32 result = readRegister(cpu, dasm->rt) >> dasm->shift;
	   result |= readRegister(cpu, dasm->rt) << (32 - dasm->shift);
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Shift right arithmetic variable. */
MIPS_INSTRUCTION( SRAV )
{
	u32 result = readRegister(cpu, dasm->rt) >> readRegister(cpu, dasm->rs);
	   result |= readRegister(cpu, dasm->rt) << (32 - readRegister(cpu, dasm->rs));
	mipsRegister res = _signextend(result, 32, BITCOUNT);
	setRegister(cpu, dasm->rd, res);
}

/* Double-word shift left logical. */
MIPS_INSTRUCTION( DSLL )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) << dasm->shift;
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift left logical variable. */
MIPS_INSTRUCTION( DSLLV )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) << readRegister(cpu, dasm->rs);
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right logical. */
MIPS_INSTRUCTION( DSRL )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> dasm->shift;
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right logical variable. */
MIPS_INSTRUCTION( DSRLV )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> readRegister(cpu, dasm->rs);
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right arithmetic. */
MIPS_INSTRUCTION( DSRA )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> dasm->shift;
	res |= readRegister(cpu, dasm->rt) << (64 - dasm->shift);
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right arithmetic variable. */
MIPS_INSTRUCTION( DSRAV )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> readRegister(cpu, dasm->rs);
	res |= readRegister(cpu, dasm->rt) << (64 - readRegister(cpu, dasm->rs));
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift left logical + 32. */
MIPS_INSTRUCTION( DSLL32 )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) << (dasm->shift + 32);
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right logical + 32. */
MIPS_INSTRUCTION( DSRL32 )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> (dasm->shift + 32);
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word shift right arithmetic + 32. */
MIPS_INSTRUCTION( DSRA32 )
{
#if BITCOUNT == 64
	s64 res = readRegister(cpu, dasm->rt) >> (dasm->shift + 32);
	res |= readRegister(cpu, dasm->rt) << (64 - (dasm->shift + 32));
	setRegister(cpu, dasm->rd, res);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/*****************************************************************************/
/*  Conditional Branch instructions                                          */
/*****************************************************************************/
/* Branch on equal. */
MIPS_INSTRUCTION( BEQ )
{
	if(readRegister(cpu, dasm->rs) == readRegister(cpu, dasm->rt))
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch on equal likely. */
MIPS_INSTRUCTION( BEQL )
{
	if(readRegister(cpu, dasm->rs) == readRegister(cpu, dasm->rt)) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNE )
{
	if(readRegister(cpu, dasm->rs) != readRegister(cpu, dasm->rt))
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if not equal. */
MIPS_INSTRUCTION( BNEL )
{
	if(readRegister(cpu, dasm->rs) != readRegister(cpu, dasm->rt)) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero. */
MIPS_INSTRUCTION( BGEZ )
{
	if(readRegister(cpu, dasm->rs) >= 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero likely. */
MIPS_INSTRUCTION( BGEZL )
{
	if(readRegister(cpu, dasm->rs) >= 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero and link. */
MIPS_INSTRUCTION( BGEZAL )
{
	doLink(cpu, 0, 0);
	if(readRegister(cpu, dasm->rs) >= 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater or equal zero and link likely. */
MIPS_INSTRUCTION( BGEZALL )
{
	doLink(cpu, 0, 0);
	if(readRegister(cpu, dasm->rs) >= 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater than zero. */
MIPS_INSTRUCTION( BGTZ )
{
	if(readRegister(cpu, dasm->rs) > 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if greater than zero likely. */
MIPS_INSTRUCTION( BGTZL )
{
	if(readRegister(cpu, dasm->rs) > 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less or equal than zero. */
MIPS_INSTRUCTION( BLEZ )
{
	if(readRegister(cpu, dasm->rs) <= 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if less or equal than zero likely. */
MIPS_INSTRUCTION( BLEZL )
{
	if(readRegister(cpu, dasm->rs) <= 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero. */
MIPS_INSTRUCTION( BLTZ )
{
	if(readRegister(cpu, dasm->rs) < 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero likely. */
MIPS_INSTRUCTION( BLTZL )
{
	if(readRegister(cpu, dasm->rs) < 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero and link. */
MIPS_INSTRUCTION( BLTZAL )
{
	doLink(cpu, 0, 0);
	if(readRegister(cpu, dasm->rs) < 0)
		BRANCH(dasm->immediate, 16);
	else
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
}

/* Branch if less than zero and link likely. */
MIPS_INSTRUCTION( BLTZALL )
{
	doLink(cpu, 0, 0);
	if(readRegister(cpu, dasm->rs) < 0) {
		BRANCH(dasm->immediate, 16);
	}else{
		/* We need to nullify the instruction in the branch delay slot here. */
		advancePC(cpu, DEFAULT_INSTRUCTION_PC);
	}
}


/*****************************************************************************/
/*  Unconditional Jump instructions                                          */
/*****************************************************************************/
/* Jump. */
MIPS_INSTRUCTION( J )
{
	DOJUMP(dasm->jump, 26);
}

/* Jump and link. */
MIPS_INSTRUCTION( JAL )
{
	doLink(cpu, 0, 0);
	DOJUMP(dasm->jump, 26);
}

/* Jump and link register. */
MIPS_INSTRUCTION( JALR )
{
	doLink(cpu, 1, dasm->rd);
	setPC(cpu, readRegister(cpu, dasm->rs));
}

/* Jump register. */
MIPS_INSTRUCTION( JR )
{
	setPC(cpu, readRegister(cpu, dasm->rs));
}


/*****************************************************************************/
/*  Data Load instructions                                                   */
/*****************************************************************************/
/* Load byte. */
MIPS_INSTRUCTION( LB )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	s8 value = cpu->readByte(cpu, readRegister(cpu, dasm->rs) + imm);
	setRegister(cpu, dasm->rt, _signextend(value, 8, BITCOUNT));
}

/* Load byte unsigned. */
MIPS_INSTRUCTION( LBU )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	u8 value = cpu->readByte(cpu, readRegister(cpu, dasm->rs) + imm);
	setRegister(cpu, dasm->rt, _zeroextend(value, 8, BITCOUNT));
}

/* Load halfword. */
MIPS_INSTRUCTION( LH )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 2)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	s16 value = cpu->readHword(cpu, off);
	setRegister(cpu, dasm->rt, _signextend(value, 16, BITCOUNT));
}

/* Load halfword unsigned. */
MIPS_INSTRUCTION( LHU )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 2)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	u16 value = cpu->readHword(cpu, off);
	setRegister(cpu, dasm->rt, _zeroextend(value, 16, BITCOUNT));
}

/* Load upper immediate. */
MIPS_INSTRUCTION( LUI )
{
	mipsRegister val = _signextend(dasm->immediate << 16, 32, BITCOUNT);
	setRegister(cpu, dasm->rt, val);
}

/* Load word. */
MIPS_INSTRUCTION( LW )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 4)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	s32 value = cpu->readWord(cpu, off);
	setRegister(cpu, dasm->rt, _signextend(value, 32, BITCOUNT));
}

/* Load word left. */
MIPS_INSTRUCTION( LWL )
{
	mipsRegister final = 0;
	int i;
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u vAddr = readRegister(cpu, dasm->rs) + imm;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= cpu->readByte(cpu, vAddr + i) << (31 - (i * 4));
	}
	setRegister(cpu, dasm->rt, final);
}

/* Load word right. */
MIPS_INSTRUCTION( LWR )
{
	mipsRegister final = 0;
	int i;
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u vAddr = readRegister(cpu, dasm->rs) + imm;
	for(i = 0; i < (vAddr % 4); i++) {
		final |= cpu->readByte(cpu, vAddr + i) << (i * 4);
	}
	setRegister(cpu, dasm->rt, final);
}

/* Load word unsigned. */
MIPS_INSTRUCTION( LWU )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 4)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	u32 value = cpu->readWord(cpu, off);
	setRegister(cpu, dasm->rt, _zeroextend(value, 32, BITCOUNT));
}

/* Load doubleword. */
MIPS_INSTRUCTION( LD )
{
#if BITCOUNT == 64
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(cpu, dasm->rs) + imm;
	if(off % 8)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	s64 value = cpu->readDword(cpu, off);
	setRegister(cpu, dasm->rt, value);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Load doubleword left. */
MIPS_INSTRUCTION( LDL )
{
#if BITCOUNT == 64
	s64 final = 0;
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(cpu, dasm->rs) + imm;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= cpu->readByte(cpu, vAddr + i) << (64 - ((i + 1) * 8));
	}
	setRegister(cpu, dasm->rt, final);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Load doubleword right. */
MIPS_INSTRUCTION( LDR )
{
#if BITCOUNT == 64
	s64 final = 0;
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(cpu, dasm->rs) + imm;
	for(i = 0; i < (vAddr % 8); i++) {
		final |= cpu->readByte(cpu, vAddr + i) << (i * 8);
	}
	setRegister(cpu, dasm->rt, final);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Load linked (word). */
MIPS_INSTRUCTION( LL )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 4)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
#ifdef DEBUG
	printf("Instruction LL not properly supported!\n");
#endif
	/* TODO: We need to make this call checkable by SC. */
	s32 value = cpu->readWord(cpu, off);
	setRegister(cpu, dasm->rt, _signextend(value, 32, BITCOUNT));
}

/* Load linked doubleword. */
MIPS_INSTRUCTION( LLD )
{
#if BITCOUNT == 64
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(cpu, dasm->rs) + imm;
	if(off % 8)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
#ifdef DEBUG
	printf("Instruction LLD not properly supported!\n");
#endif
	/* TODO: We need to make this call checkable by SCD. */
	s64 value = cpu->readDword(cpu, off);
	setRegister(cpu, dasm->rt, value);
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}


/*****************************************************************************/
/*  Data Store instructions                                                  */
/*****************************************************************************/
/* Store byte. */
MIPS_INSTRUCTION( SB )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	cpu->writeByte(cpu, off, readRegister(cpu, dasm->rt) & MAXMASK(8));
}

/* Store word. */
MIPS_INSTRUCTION( SH )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 2)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	cpu->writeHword(cpu, off, readRegister(cpu, dasm->rt) & MAXMASK(16));
}

/* Store word. */
MIPS_INSTRUCTION( SW )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 4)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	cpu->writeWord(cpu, off, readRegister(cpu, dasm->rt) & MAXMASK(32));
}

/* Store word left. */
MIPS_INSTRUCTION( SWL )
{
	int i;
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u vAddr = readRegister(cpu, dasm->rs) + imm;
	mipsRegister_u regval = readRegister(cpu, dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		cpu->writeByte(cpu, vAddr + i, regval >> (32 - ((i + 1) * 4)));
}

/* Store word right. */
MIPS_INSTRUCTION( SWR )
{
	int i;
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u vAddr = readRegister(cpu, dasm->rs) + imm;
	mipsRegister_u regval = readRegister(cpu, dasm->rt);
	for(i = (vAddr % 4); i < 4; i++)
		cpu->writeByte(cpu, vAddr + i, regval >> (i * 4));
}

/* Store doubleword. */
MIPS_INSTRUCTION( SD )
{
#if BITCOUNT == 64
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(cpu, dasm->rs) + imm;
	if(off % 8)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	cpu->writeDword(cpu, off, readRegister(cpu, dasm->rt));
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Store doubleword left. */
MIPS_INSTRUCTION( SDL )
{
#if BITCOUNT == 64
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(cpu, dasm->rs) + imm;
	u64 regval = readRegister(cpu, dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		cpu->writeByte(cpu, vAddr + i, regval >> (64 - ((i + 1) * 8)));
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Store doubleword right. */
MIPS_INSTRUCTION( SDR )
{
#if BITCOUNT == 64
	int i;
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 vAddr = readRegister(cpu, dasm->rs) + imm;
	u64 regval = readRegister(cpu, dasm->rt);
	for(i = (vAddr % 8); i < 8; i++)
		cpu->writeByte(cpu, vAddr + i, regval >> (i * 8));
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Store conditional. */
MIPS_INSTRUCTION( SC )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	mipsRegister_u off = readRegister(cpu, dasm->rs) + imm;
	if(off % 4)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	mipsRegister_u pass;
#ifdef DEBUG
	printf("Instruction SC not properly supported!\n");
#endif
	/* TODO: We need to check to see if the loacation that was loaded earlier was modified */
	/* Until we do so, this call will be set to FAIL every time! */
	pass = 0;
	setRegister(cpu, dasm->rt, pass);
	if(pass)
		cpu->writeWord(cpu, off, readRegister(cpu, dasm->rt) & MAXMASK(32));
}

/* Store conditional double-word. */
MIPS_INSTRUCTION( SCD )
{
#if BITCOUNT == 64
	s64 imm = _signextend(dasm->immediate, 16, 64);
	u64 off = readRegister(cpu, dasm->rs) + imm;
	if(off % 8)
		generateException(cpu, EXCEPTION_BADADDR_STORE, dasm->delay);
	u64 pass;
#ifdef DEBUG
	printf("Instruction SCD not properly supported!\n");
#endif
	/* TODO: We need to check to see if the loacation that was loaded earlier was modified */
	/* Until we do so, this call will be set to FAIL every time! */
	pass = 0;
	setRegister(cpu, dasm->rt, pass);
	if(pass)
		cpu->writeDword(cpu, off, readRegister(cpu, dasm->rt));
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/*****************************************************************************/
/*  Special register instructions                                            */
/*****************************************************************************/
/* Move from HI. */
MIPS_INSTRUCTION( MFHI )
{
	setRegister(cpu, dasm->rd, readRegister(cpu, REGISTER_HI));
}

/* Move from LO. */
MIPS_INSTRUCTION( MFLO )
{
	setRegister(cpu, dasm->rd, readRegister(cpu, REGISTER_LO));
}

/* Move to HI. */
MIPS_INSTRUCTION( MTHI )
{
	setRegister(cpu, REGISTER_HI, readRegister(cpu, dasm->rd));
}

/* Move to LO. */
MIPS_INSTRUCTION( MTLO )
{
	setRegister(cpu, REGISTER_LO, readRegister(cpu, dasm->rd));
}

/*****************************************************************************/
/*  Special instructions                                                     */
/*****************************************************************************/
/* Syscall. */
MIPS_INSTRUCTION( SYSCALL )
{
	generateException(cpu, EXCEPTION_SYSCALL, dasm->delay);
}	

/* Breakpoint. */
MIPS_INSTRUCTION( BREAK )
{
	generateException(cpu, EXCEPTION_BREAKPOINT, dasm->delay);
}	

/* Synchronize. */
MIPS_INSTRUCTION( SYNC )
{
#ifdef DEBUG
	printf("Instruction SYNC unsupported!\n");
#endif
	/* TODO: Implement */
}	

/* Cache. */
MIPS_INSTRUCTION( CACHE )
{
#ifdef DEBUG
	printf("Instruction CACHE unsupported!\n");
#endif
	/* TODO: Implement */
}

/*****************************************************************************/
/*  Trap instructions                                                        */
/*****************************************************************************/
/* Trap if Equal. */
MIPS_INSTRUCTION( TEQ )
{
	if(readRegister(cpu, dasm->rs) == readRegister(cpu, dasm->rt))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Equal Immediate. */
MIPS_INSTRUCTION( TEQI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	if(readRegister(cpu, dasm->rs) == imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Not Equal. */
MIPS_INSTRUCTION( TNE )
{
	if(readRegister(cpu, dasm->rs) != readRegister(cpu, dasm->rt))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Not Equal Immediate. */
MIPS_INSTRUCTION( TNEI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	if(readRegister(cpu, dasm->rs) != imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Greater than or Equal. */
MIPS_INSTRUCTION( TGE )
{
	if(readRegister(cpu, dasm->rs) >= readRegister(cpu, dasm->rt))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Greater than or Equal unsigned. */
MIPS_INSTRUCTION( TGEU )
{
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) >= ((mipsRegister_u)readRegister(cpu, dasm->rt)))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Greater than or Equal Immediate. */
MIPS_INSTRUCTION( TGEI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	if(readRegister(cpu, dasm->rs) >= imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Greater than or Equal Immediate unsigned. */
MIPS_INSTRUCTION( TGEIU )
{
	mipsRegister_u imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) >= imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Less than. */
MIPS_INSTRUCTION( TLT )
{
	if(readRegister(cpu, dasm->rs) < readRegister(cpu, dasm->rt))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Less than unsigned. */
MIPS_INSTRUCTION( TLTU )
{
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) < ((mipsRegister_u)readRegister(cpu, dasm->rt)))
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Less than Immediate. */
MIPS_INSTRUCTION( TLTI )
{
	mipsRegister imm = _signextend(dasm->immediate, 16, BITCOUNT);
	if(readRegister(cpu, dasm->rs) < imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
}

/* Trap if Less than Immediate unsigned. */
MIPS_INSTRUCTION( TLTIU )
{
	mipsRegister_u imm = _zeroextend(dasm->immediate, 16, BITCOUNT);
	if(((mipsRegister_u)readRegister(cpu, dasm->rs)) < imm)
		generateException(cpu, EXCEPTION_TRAP, dasm->delay);
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
}

/* Read Indexed TLB entry. */
MIPS_INSTRUCTION( TLBR )
{
#ifdef DEBUG
	printf("Instruction TLBR unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Write Indexed TLB entry. */
MIPS_INSTRUCTION( TLBWI )
{
#ifdef DEBUG
	printf("Instruction TLBWI unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Write Random TLB entry. */
MIPS_INSTRUCTION( TLBWR )
{
#ifdef DEBUG
	printf("Instruction TLBWR unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Exception return. */
MIPS_INSTRUCTION( ERET )
{
	setPC(cpu, readCopRegister(cpu, 0, COP0_REG_EPC));
	/* TODO: We need to kill all the LL/SC pairs here */
}


/*****************************************************************************/
/*  Coprocessor instructions                                                 */
/*****************************************************************************/
/* Load double-word to coprocessor 1. */
MIPS_INSTRUCTION( LDC1 )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction LDC1 unsupported!\n");
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Load double-word to coprocessor 2. */
MIPS_INSTRUCTION( LDC2 )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction LDC2 unsupported!\n");
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Load word to coprocessor 1. */
MIPS_INSTRUCTION( LWC1 )
{
#ifdef DEBUG
	printf("Instruction LWC1 unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Load word to coprocessor 2. */
MIPS_INSTRUCTION( LWC2 )
{
#ifdef DEBUG
	printf("Instruction LWC2 unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Store double-word to coprocessor 1. */
MIPS_INSTRUCTION( SDC1 )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction SDC1 unsupported!\n");
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Store double-word to coprocessor 2. */
MIPS_INSTRUCTION( SDC2 )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction SDC2 unsupported!\n");
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Store word to coprocessor 1. */
MIPS_INSTRUCTION( SWC1 )
{
#ifdef DEBUG
	printf("Instruction SWC1 unsupported!\n");
#endif
	/* TODO: Implement */
}

/* Store word to coprocessor 2. */
MIPS_INSTRUCTION( SWC2 )
{
#ifdef DEBUG
	printf("Instruction SWC2 unsupported!\n");
#endif
	/* TODO: Implement */
}


/*****************************************************************************/
/*  Coprocessor sub-instructions                                             */
/*****************************************************************************/
/* Move from coprocessor z. */
MIPS_COP_INSTRUCTION( MFCz )
{
	setRegister(cpu, dasm->rt, readCopRegister(cpu, cop, dasm->rd));
}

/* Move to coprocessor z. */
MIPS_COP_INSTRUCTION( MTCz )
{
	setCopRegister(cpu, cop, dasm->rd, readRegister(cpu, dasm->rt));
}

/* Double-word move from coprocessor z. */
MIPS_COP_INSTRUCTION( DMFCz )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction DMFC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Double-word move to coprocessor z. */
MIPS_COP_INSTRUCTION( DMTCz )
{
#if BITCOUNT == 64
#ifdef DEBUG
	printf("Instruction DMTC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
#else
	MIPS_INSTR_EXEC( RESV )
#endif
}

/* Move control from coprocessor z. */
MIPS_COP_INSTRUCTION( CFCz )
{
#ifdef DEBUG
	printf("Instruction CFC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
}

/* Move control to coprocessor z. */
MIPS_COP_INSTRUCTION( CTCz )
{
#ifdef DEBUG
	printf("Instruction CTC%d unsupported!\n", cop);
#endif
	/* TODO: Implement */
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
}

/* Branch on coprocessor z false likely. */
MIPS_COP_INSTRUCTION( BCzFL )
{
#ifdef DEBUG
	printf("Instruction BC%dFL unsupported!\n", cop);
#endif
	/* TODO: Implement */
}

/* Branch on coprocessor z true. */
MIPS_COP_INSTRUCTION( BCzT )
{
#ifdef DEBUG
	printf("Instruction BC%dT unsupported!\n", cop);
#endif
	/* TODO: Implement */
}

/* Branch on coprocessor z true likely. */
MIPS_COP_INSTRUCTION( BCzTL )
{
#ifdef DEBUG
	printf("Instruction BC%dTL unsupported!\n", cop);
#endif
	/* TODO: Implement */
}

/*****************************************************************************/
/*  Instruction Tables.                                                      */
/*****************************************************************************/
/* General table with all opcodes. */
mipsInstrTbl instructionTable[] = {
/*00*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Special */
/*01*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Regimm */
/*02*/	INST_ENTRY( J,		" %j",		1, DEFAULT_CYCLES, 0 ),
/*03*/	INST_ENTRY( JAL,	" %j",		1, DEFAULT_CYCLES, 0 ),
/*04*/	INST_ENTRY( BEQ,	" %s, %t, %i",	1, DEFAULT_CYCLES, 0 ),
/*05*/	INST_ENTRY( BNE,	" %s, %t, %i",	1, DEFAULT_CYCLES, 0 ),
/*06*/	INST_ENTRY( BLEZ,	" %s, %i",	1, DEFAULT_CYCLES, 0 ),
/*07*/	INST_ENTRY( BGTZ,	" %s, %i",	1, DEFAULT_CYCLES, 0 ),
/*08*/	INST_ENTRY( ADDI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*09*/	INST_ENTRY( ADDIU,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0A*/	INST_ENTRY( SLTI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0B*/	INST_ENTRY( SLTIU,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0C*/	INST_ENTRY( ANDI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0D*/	INST_ENTRY( ORI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0E*/	INST_ENTRY( XORI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0F*/	INST_ENTRY( LUI,	" %t, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*10*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* COP0 */
/*11*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* COP1 */
/*12*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* COP2 */
/*13*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*14*/	INST_ENTRY( BEQL,	" %s, %t, %i",	1, DEFAULT_CYCLES, 0 ), 
/*15*/	INST_ENTRY( BNEL,	" %s, %t, %i",	1, DEFAULT_CYCLES, 0 ), 
/*16*/	INST_ENTRY( BLEZL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*17*/	INST_ENTRY( BGTZL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*18*/	INST_ENTRY( DADDI,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*18*/	INST_ENTRY( DADDIU,	" %t, %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1A*/	INST_ENTRY( LDL,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1B*/	INST_ENTRY( LDR,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1C*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1D*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1E*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1F*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*20*/	INST_ENTRY( LB,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*21*/	INST_ENTRY( LH,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*22*/	INST_ENTRY( LWL,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*23*/	INST_ENTRY( LW,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*24*/	INST_ENTRY( LBU,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*25*/	INST_ENTRY( LHU,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*26*/	INST_ENTRY( LWR,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*27*/	INST_ENTRY( LWU,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*28*/	INST_ENTRY( SB,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*29*/	INST_ENTRY( SH,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2A*/	INST_ENTRY( SWL,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2B*/	INST_ENTRY( SW,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2C*/	INST_ENTRY( SDL,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2D*/	INST_ENTRY( SDR,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2E*/	INST_ENTRY( SWR,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2F*/	INST_ENTRY( CACHE,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*30*/	INST_ENTRY( LL,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*31*/	INST_ENTRY( LWC1,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*32*/	INST_ENTRY( LWC2,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*33*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*34*/	INST_ENTRY( LLD,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*35*/	INST_ENTRY( LDC1,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*36*/	INST_ENTRY( LDC2,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*37*/	INST_ENTRY( LD,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*38*/	INST_ENTRY( SC,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*39*/	INST_ENTRY( SWC1,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*3A*/	INST_ENTRY( SWC2,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*3B*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*3C*/	INST_ENTRY( SCD,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*3D*/	INST_ENTRY( SDC1,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*3E*/	INST_ENTRY( SDC2,	" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*3F*/	INST_ENTRY( SD,		" %t, %i(%s)",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
};

/* Special opcodes.
 * The first six bits are 000000 and the instruction is encoded in the last six bits. */

mipsInstrTbl specialInstructionTable[] = {
/*00*/	INST_ENTRY( SLL,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*01*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*02*/	INST_ENTRY( SRL,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*03*/	INST_ENTRY( SRA,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*04*/	INST_ENTRY( SLLV,	" %d, %t, %s",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*05*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*06*/	INST_ENTRY( SRLV,	" %d, %t, %s",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*07*/	INST_ENTRY( SRAV,	" %d, %t, %s",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*08*/	INST_ENTRY( JR,		" %s",		1, DEFAULT_CYCLES, 0 ), 
/*09*/	INST_ENTRY( JALR,	" %s",		1, DEFAULT_CYCLES, 0 ), 
/*0A*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0B*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0C*/	INST_ENTRY( SYSCALL,	"",		0, DEFAULT_CYCLES, 0 ), 
/*0D*/	INST_ENTRY( BREAK,	"",		0, DEFAULT_CYCLES, 0 ), 
/*0E*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0F*/	INST_ENTRY( SYNC,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*10*/	INST_ENTRY( MFHI,	" %d",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*11*/	INST_ENTRY( MTHI,	" %s",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*12*/	INST_ENTRY( MFLO,	" %d",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*13*/	INST_ENTRY( MTLO,	" %s",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*14*/	INST_ENTRY( DSLLV,	" %d, %t, %s",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*15*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*16*/	INST_ENTRY( DSRLV,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*17*/	INST_ENTRY( DSRAV,	" %d, %t, %s",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*18*/	INST_ENTRY( MULT,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*19*/	INST_ENTRY( MULTU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1A*/	INST_ENTRY( DIV,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1B*/	INST_ENTRY( DIVU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*1C*/	INST_ENTRY( DMULT,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*1D*/	INST_ENTRY( DMULTU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*1E*/	INST_ENTRY( DDIV,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*1F*/	INST_ENTRY( DDIVU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*20*/	INST_ENTRY( ADD,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*21*/	INST_ENTRY( ADDU,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*22*/	INST_ENTRY( SUB,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*23*/	INST_ENTRY( SUBU,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*24*/	INST_ENTRY( AND,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*25*/	INST_ENTRY( OR,		" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*26*/	INST_ENTRY( XOR,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*27*/	INST_ENTRY( NOR,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*28*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*29*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*2A*/	INST_ENTRY( SLT,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*2B*/	INST_ENTRY( SLTU,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*2C*/	INST_ENTRY( DADD,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*2D*/	INST_ENTRY( DADDU,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*2E*/	INST_ENTRY( DSUB,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*2F*/	INST_ENTRY( DSUBU,	" %d, %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*30*/	INST_ENTRY( TGE,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*31*/	INST_ENTRY( TGEU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*32*/	INST_ENTRY( TLT,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*33*/	INST_ENTRY( TLTU,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*34*/	INST_ENTRY( TEQ,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*35*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*36*/	INST_ENTRY( TNE,	" %s, %t",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*37*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*38*/	INST_ENTRY( DSLL,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*39*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*3A*/	INST_ENTRY( DSRL,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*3B*/	INST_ENTRY( DSRA,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*3C*/	INST_ENTRY( DSLL32,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*3D*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*3C*/	INST_ENTRY( DSRL32,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*3F*/	INST_ENTRY( DSRA32,	" %d, %t, %h",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
};

/* Regimm opcodes.
 * The first six bits are 000001 and the instruction is encoded in the t register space. */

mipsInstrTbl regimmInstructionTable[] = {
/*00*/	INST_ENTRY( BLTZ,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*01*/	INST_ENTRY( BGEZ,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*02*/	INST_ENTRY( BLTZL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*03*/	INST_ENTRY( BGEZL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*04*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*05*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*06*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*07*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*08*/	INST_ENTRY( TGEI,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*09*/	INST_ENTRY( TGEIU,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0A*/	INST_ENTRY( TLTI,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0B*/	INST_ENTRY( TLTIU,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0C*/	INST_ENTRY( TEQI,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0D*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0E*/	INST_ENTRY( TNEI,	" %s, %i",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*0F*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*10*/	INST_ENTRY( BLTZAL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*11*/	INST_ENTRY( BGEZAL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*12*/	INST_ENTRY( BLTZALL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*13*/	INST_ENTRY( BGEZALL,	" %s, %i",	1, DEFAULT_CYCLES, 0 ), 
/*14*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*15*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*16*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*17*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*18*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*19*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1A*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1B*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1C*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1D*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1E*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1F*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
};

/* Coprocessor opcodes.
 * The first six bits are 0100xx (where xx is the cop) and the instruction is encoded 
 * in the s register space. */

mipsCopInstrTbl coprocInstructionTable[] = {
/*00*/	COP_INST_ENTRY( MFCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*01*/	COP_INST_ENTRY( DMFCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*02*/	COP_INST_ENTRY( CFCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*03*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*04*/	COP_INST_ENTRY( MTCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*05*/	COP_INST_ENTRY( DMTCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*06*/	COP_INST_ENTRY( CTCz,		" %t, %d",	0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*07*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*08*/	COP_INST_ENTRY( RNOP,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* BC */
/*09*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0A*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0B*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0C*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0D*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0E*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0F*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*10*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*11*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*12*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*13*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*14*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*15*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*16*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*17*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*18*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*19*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1A*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1B*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1C*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1D*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1E*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1F*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
};

/* Coprocessor 0 opcodes.
 * The first seven bits are 0100001 and the instruction is encoded 
 * in the t register space. */
/* COP0 needs to go somewhere >.> */

mipsInstrTbl cop0InstructionTable[] = {
/*00*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*01*/	INST_ENTRY( TLBR,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*02*/	INST_ENTRY( TLBWI,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), 
/*03*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*04*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*05*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*06*/	INST_ENTRY( TLBWR,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*07*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*08*/	INST_ENTRY( TLBP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ),
/*09*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0A*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0B*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0C*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0D*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0E*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0F*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*10*/	INST_ENTRY( RESV,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*11*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*12*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*13*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*14*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*15*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*16*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*17*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*18*/	INST_ENTRY( ERET,	"",		0, DEFAULT_CYCLES, 8 ),
/*19*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1A*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1B*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1C*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1D*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1E*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1F*/	INST_ENTRY( RNOP,	"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
};

/* Coprocessor branch opcodes.
 * The first eleven bits are 0100xx01000 (where xx is the cop) and the instruction is encoded 
 * in the t register space. */

mipsCopInstrTbl coprocBcInstructionTable[] = {
/*00*/	COP_INST_ENTRY( BCzF,		" %i",		1, DEFAULT_CYCLES, 0 ),
/*01*/	COP_INST_ENTRY( BCzT,		" %i",		1, DEFAULT_CYCLES, 0 ),
/*02*/	COP_INST_ENTRY( BCzFL,		" %i",		1, DEFAULT_CYCLES, 0 ),
/*03*/	COP_INST_ENTRY( BCzTL,		" %i",		1, DEFAULT_CYCLES, 0 ),
/*04*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*05*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*06*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*07*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*08*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*09*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0A*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0B*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0C*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0D*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0E*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*0F*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*10*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*11*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*12*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*13*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*14*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*15*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*16*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*17*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*18*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*19*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1A*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1B*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1C*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1D*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1E*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
/*1F*/	COP_INST_ENTRY( RESV,		"",		0, DEFAULT_CYCLES, DEFAULT_INSTRUCTION_PC ), /* Nothing */
};

void execOpcode(mipsCpu* cpu, mipsDasm *dasm)
{
#ifdef DEBUG		
	printf("Instruction 0x%08X Function 0x%08X\n", dasm->instruction, dasm->funct);
#endif
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			specialInstructionTable[dasm->funct].execute(cpu, dasm);
			advanceCycles(cpu, specialInstructionTable[dasm->funct].cycles);
			advancePC(cpu, specialInstructionTable[dasm->funct].pc);
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			regimmInstructionTable[dasm->rt].execute(cpu, dasm);
			advanceCycles(cpu, regimmInstructionTable[dasm->rt].cycles);
			advancePC(cpu, regimmInstructionTable[dasm->rt].pc);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if (dasm->rs <= COPROC_INST_COUNT) {
			if( dasm->rs == 0x8) {
				if(dasm->rs <= COPROC_BC_INST_COUNT) {
					if(coprocBcInstructionTable[dasm->rt].execute == MIPS_COP_INSTR_NAME(RNOP))
						copxHandler(cpu, dasm);
					else {
						coprocBcInstructionTable[dasm->rt].execute(cpu, dasm, dasm->instruction & 0x3);
						advanceCycles(cpu, coprocBcInstructionTable[dasm->rt].cycles);
						advancePC(cpu, coprocBcInstructionTable[dasm->rt].pc);
					}
				}else{
#ifdef DEBUG
					printf("rt is too high!\n");
#endif
				}
			}else if(dasm->rs & 0x10) {
				if(dasm->funct <= COPROC_COP0_INST_COUNT) {
					if(cop0InstructionTable[dasm->funct].execute == MIPS_INSTR_NAME(RNOP))
						cop0Handler(cpu, dasm);
					else {
						cop0InstructionTable[dasm->funct].execute(cpu, dasm);
						advanceCycles(cpu, cop0InstructionTable[dasm->funct].cycles);
						advancePC(cpu, cop0InstructionTable[dasm->funct].pc);
					}
				}else{
#ifdef DEBUG
					printf("Function is too high!\n");
#endif
				}
			}else{
				if(coprocInstructionTable[dasm->rs].execute == MIPS_COP_INSTR_NAME(RNOP))
					copxHandler(cpu, dasm);
				else {
					coprocInstructionTable[dasm->rs].execute(cpu, dasm, dasm->instruction & 0x3);
					advanceCycles(cpu, coprocInstructionTable[dasm->rs].cycles);
					advancePC(cpu, coprocInstructionTable[dasm->rs].pc);
				}
			}
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
#endif
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			instructionTable[dasm->instruction].execute(cpu, dasm);
			advanceCycles(cpu, instructionTable[dasm->instruction].cycles);
			advancePC(cpu, instructionTable[dasm->instruction].pc);
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
		}
	}
}

char* textOpcode(mipsCpu* cpu, mipsDasm *dasm)
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
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(dasm->rs <= COPROC_INST_COUNT) {
			if(dasm->rs == 0x8)
				return dasmFormat(coprocBcInstructionTable[dasm->rt].textDisasm, dasm);
			else if(dasm->rs & 0x10)
				return dasmFormat(cop0InstructionTable[dasm->funct].textDisasm, dasm);
			else
				return dasmFormat(coprocInstructionTable[dasm->rs].textDisasm, dasm);
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
