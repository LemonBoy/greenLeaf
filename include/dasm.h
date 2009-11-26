#ifndef _DASM_H
#define _DASM_H

#include "types.h"

char* dasmFormat(char *haystack, mipsDasm *dasm);

char *registerName[34];

char* registerToName(mipsRegister reg);

extern mipsInstrTbl cop0InstructionTable[];
extern mipsCopInstrTbl coprocBcInstructionTable[];
extern mipsCopInstrTbl coprocInstructionTable[];
extern mipsInstrTbl regimmInstructionTable[];
extern mipsInstrTbl specialInstructionTable[];
extern mipsInstrTbl instructionTable[];

#include "emulator.h"

INLINE void dasmOpcode(mipsCpu* cpu, u32 opcode, mipsDasm *ret)
{
	mipsDasm *dasm = ret;
	
	dasm->instruction = INSTRUCTION(opcode);
	dasm->rs = RS(opcode);
	dasm->rt = RT(opcode);
	dasm->rd = RD(opcode);
	dasm->shift = SHIFT(opcode);
	dasm->immediate = IMMEDIATE(opcode);
	dasm->jump = JUMP(opcode);
	dasm->funct = FUNCT(opcode);
	
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			dasm->delay = specialInstructionTable[dasm->funct].delay;
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
			dasm->delay = 0;
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			dasm->delay = regimmInstructionTable[dasm->rt].delay;
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
			dasm->delay = 0;
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(dasm->rs <= COPROC_INST_COUNT) {
			if(dasm->rs == 0x8)
				dasm->delay = coprocBcInstructionTable[dasm->rt].delay;
			else if(dasm->rs & 0x10)
				dasm->delay = cop0InstructionTable[dasm->funct].delay;
			else
				dasm->delay = coprocInstructionTable[dasm->rs].delay;
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
#endif
			dasm->delay = 0;
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			dasm->delay = instructionTable[dasm->instruction].delay;
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
			dasm->delay = 0;
		}
	}	
}

#endif
