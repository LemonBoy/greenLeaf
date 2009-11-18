#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"
#include "instructions.h"

char *registerName[34] = {
	"zr", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",
	"HI", "LO"
};
char* registerToName(mipsRegister reg)
{
	return registerName[reg];
}

char *dasmFormat(char *haystack, mipsDasm *dasm)
{
	char formattedStr[128];
	
	char *hptr = haystack;
	char *fptr = formattedStr;
	
	char fmtBuf[16];
		
	while (*hptr) {
		int len = 1;
		if(*hptr == '%') {
			hptr++;
			switch(*hptr) {
				case 't':
					strcpy(&(*fptr), registerToName(dasm->rt));
					len = strlen(registerToName(dasm->rt));
					break;
				case 's':
					strcpy(&(*fptr), registerToName(dasm->rs));
					len = strlen(registerToName(dasm->rs));
					break;
				case 'd':
					strcpy(&(*fptr), registerToName(dasm->rd));
					len = strlen(registerToName(dasm->rd));
					break;										
				case 'i':
					sprintf(fmtBuf, "0x%04X", dasm->immediate & 0xFFFF);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
				case 'j':
					sprintf(fmtBuf, "%c0x%07X", (dasm->jump & 0x2000000) ? '-' : ' ', dasm->jump & 0x1FFFFFF);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
				case 'h':
					sprintf(fmtBuf, "0x%02X", dasm->shift & 0x1F);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
			}
		}else if (*hptr == 'z') {
			sprintf(fmtBuf, "%u", dasm->funct & 0x3);
			strcpy(&(*fptr), fmtBuf);
			len = strlen(fmtBuf);
		}else{
			*fptr = *hptr;
		}
		fptr += len;
		hptr++;
	}
	
	*fptr = '\0';
	
	return (char *)strdup(formattedStr);
}					

void dasmOpcode(u32 opcode, mipsDasm **ret)
{
	mipsDasm *dasm = malloc(sizeof(mipsDasm));

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
		if(readRegister(dasm->rt) <= REGIMM_INST_COUNT) {
			dasm->delay = regimmInstructionTable[readRegister(dasm->rt)].delay;
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
			dasm->delay = 0;
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(readRegister(dasm->rs) <= COPROC_INST_COUNT) {
			if(readRegister(dasm->rs) == 0x8)
				dasm->delay = coprocBcInstructionTable[readRegister(dasm->rt)].delay;
			else if(readRegister(dasm->rs) & 0x10)
				dasm->delay = cop0InstructionTable[dasm->funct].delay;
			else
				dasm->delay = coprocInstructionTable[readRegister(dasm->rs)].delay;
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
	
	*ret = dasm;
}	
