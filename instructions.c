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
	switch (dasm->rs)
	{
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

/* Add with overflow. */

mipsInstruction ADD (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(dasm->rs) + readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate. */

mipsInstruction ADDI (mipsDasm *dasm)
{
	setRegister(dasm->rt, readRegister(dasm->rs) + dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add immediate unsigned. */

mipsInstruction ADDIU (mipsDasm *dasm)
{
	setRegister(dasm->rt, (u64)(readRegister(dasm->rs)) + (u64)(dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* And. */

mipsInstruction AND (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(dasm->rs) & readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* And immediate. */

mipsInstruction ANDI (mipsDasm *dasm)
{
	setRegister(dasm->rt, readRegister(dasm->rs) & dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Add unsigned. */

mipsInstruction ADDU (mipsDasm *dasm)
{
	setRegister(dasm->rd, (u64)(readRegister(dasm->rs)) + (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Branch if equal. */

mipsInstruction BEQ (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) == readRegister(dasm->rt))
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater or equal zero . */

mipsInstruction BGEZ (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) >= 0)
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if greater than zero . */

mipsInstruction BGTZ (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) > 0)
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less or equal than zero . */

mipsInstruction BLEZ (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) <= 0)
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if less than zero . */

mipsInstruction BLTZ (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) < 0)
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Branch if not equal. */

mipsInstruction BNE (mipsDasm *dasm)
{
	if (readRegister(dasm->rs) != readRegister(dasm->rt))
	{
		advancePC(dasm->immediate << 2);
	} else
	{
		advancePC(DEFAULT_INSTRUCTION_PC);
	}
}

/* Divide. */

mipsInstruction DIV (mipsDasm *dasm)
{
	setRegister(REGISTER_HI, readRegister(dasm->rs) / readRegister(dasm->rt));
	setRegister(REGISTER_LO, readRegister(dasm->rs) % readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Divide unsigned. */

mipsInstruction DIVU (mipsDasm *dasm)
{
	setRegister(REGISTER_HI, (u64)(readRegister(dasm->rs)) / (u64)(readRegister(dasm->rt)));
	setRegister(REGISTER_LO, (u64)(readRegister(dasm->rs)) % (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Jump. */

mipsInstruction J (mipsDasm *dasm)
{
	setJump(dasm->jump, 0);
}

/* Jump and link. */

mipsInstruction JAL (mipsDasm *dasm)
{
	setJump(dasm->jump, 1);
}

/* Jump and link register. */

mipsInstruction JALR (mipsDasm *dasm)
{
	setJump(readRegister(dasm->rs), 1);
}

/* Jump register. */

mipsInstruction JR (mipsDasm *dasm)
{
	setPC(readRegister(dasm->rs));
}

/* Load byte. */

mipsInstruction LB (mipsDasm *dasm)
{
	setRegister(dasm->rt, emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load byte unsigned. */

mipsInstruction LBU (mipsDasm *dasm)
{
	setRegister(dasm->rt, (u8)(emulatedCpu.readByte(readRegister(dasm->rs) + dasm->immediate)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load halfword unsigned. */

mipsInstruction LHU (mipsDasm *dasm)
{
	setRegister(dasm->rt, (u64)(emulatedCpu.readHword((u64)(readRegister(dasm->rs)) + (u64)(dasm->immediate))));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load upper immediate. */

mipsInstruction LUI (mipsDasm *dasm)
{
	setRegister(dasm->rt, dasm->immediate << 16);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Load word. */

mipsInstruction LW (mipsDasm *dasm)
{
	setRegister(dasm->rt, emulatedCpu.readWord(readRegister(dasm->rs) + dasm->immediate));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from coprocessor 0. */

mipsInstruction MFC0 (mipsDasm *dasm)
{
	cop0Handler(dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from HI. */

mipsInstruction MFHI (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(REGISTER_HI));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Move from LO. */

mipsInstruction MFLO (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(REGISTER_LO));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Multiply. */

mipsInstruction MULT (mipsDasm *dasm)
{
	setRegister(33, readRegister(dasm->rs) * readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Multiply unsigned. */

mipsInstruction MULTU (mipsDasm *dasm)
{
	setRegister(33, (u64)(readRegister(dasm->rs)) * (u64)(readRegister(dasm->rt)));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* No-operation. */

mipsInstruction NOOP (mipsDasm *dasm)
{
#ifdef DEBUG
	exit(-6);
#endif
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Or. */

mipsInstruction OR (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(dasm->rs) | readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Or immediate. */

mipsInstruction ORI (mipsDasm *dasm)
{
	setRegister(dasm->rt, readRegister(dasm->rs) | dasm->immediate);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Store byte. */

mipsInstruction SB (mipsDasm *dasm)
{
	emulatedCpu.writeByte(readRegister(dasm->rs) + (dasm->immediate & 0xFF), readRegister(dasm->rt));
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift left logical. */

mipsInstruction SLL (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(dasm->rt) << dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Shift right logical. */

mipsInstruction SRL (mipsDasm *dasm)
{
	setRegister(dasm->rd, readRegister(dasm->rt) >> dasm->shift);
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Set if less than immediate unsigned. */

mipsInstruction SLTIU (mipsDasm *dasm)
{
	if ((mipsRegister)readRegister(dasm->rs) < (mipsRegister)dasm->immediate)
	{
		setRegister(dasm->rt, 1);
	} else
	{
		setRegister(dasm->rt, 0);
	}
	advancePC(DEFAULT_INSTRUCTION_PC);
}

/* Syscall. */

mipsInstruction SYSCALL (mipsDasm *dasm)
{
	generateException(8, dasm);
	advancePC(DEFAULT_INSTRUCTION_PC);
}	

/* Store word. */

mipsInstruction SW (mipsDasm *dasm)
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
	printf("Instruction %#x Function %#x\n", dasm->instruction, dasm->funct);
#endif
	if (dasm->instruction != 0) {
		instructionTable[dasm->instruction].execute(dasm);
	} else if (dasm->instruction == 0) {
		specialInstructionTable[dasm->funct].execute(dasm);
	} else if (dasm->instruction == 1) {
		regimmInstructionTable[dasm->rt].execute(dasm);
	}
}

char* textOpcode(mipsDasm *dasm)
{
	if (dasm->instruction != 0) {
		return dasmFormat(instructionTable[dasm->instruction].textDisasm, dasm);
	} else if (dasm->instruction == 0) {
		return dasmFormat(specialInstructionTable[dasm->funct].textDisasm, dasm);
	} else if (dasm->instruction == 1) {
		return dasmFormat(regimmInstructionTable[dasm->rt].textDisasm, dasm);
	}
	return "Not implemented";
}
