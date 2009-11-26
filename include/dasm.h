#ifndef _DASM_H
#define _DASM_H

#include "types.h"
#include "emulator.h"

typedef struct _mipsDasm {
	u8 delay;
	u8 instruction;
	s8 rs;
	s8 rt;
	s8 rd;
	s32 shift;
	s16 immediate;
	s32 jump;
	s8 funct;
} mipsDasm;

typedef struct _mipsInstrTbl {
	mipsInstruction (*execute)(mipsCpu* cpu, mipsDasm *dasm);
	char textDisasm[20];
	u8 delay;
	u8 cycles;
	u8 pc;
} mipsInstrTbl;

typedef struct _mipsCopInstrTbl {
	mipsInstruction (*execute)(mipsCpu* cpu, mipsDasm *dasm, int cop);
	char textDisasm[20];
	u8 delay;
	u8 cycles;
	u8 pc;
} mipsCopInstrTbl;

/* static char *registerName[34] = {
	"zr" , "r1" , "r2" , "r3" , "r4" , "r5" , "r6" , "r7" , "r8" , "r9" , "r10",
	"r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20",
	"r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30",
	"lr", "HI", "LO"
}; */

char *registerName[34];

char* registerToName(mipsRegister reg);
char* dasmFormat(char *haystack, mipsDasm *dasm);

#include "instructions.h"

extern struct _mipsInstrTbl cop0InstructionTable[];
extern struct _mipsCopInstrTbl coprocBcInstructionTable[];
extern struct _mipsCopInstrTbl coprocInstructionTable[];
extern struct _mipsInstrTbl regimmInstructionTable[];
extern struct _mipsInstrTbl specialInstructionTable[];
extern struct _mipsInstrTbl instructionTable[];

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
