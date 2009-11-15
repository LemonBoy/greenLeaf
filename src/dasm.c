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
					sprintf(fmtBuf, "%#x", dasm->immediate);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
				case 'j':
					sprintf(fmtBuf, "%#x", dasm->jump);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
				case 'h':
					sprintf(fmtBuf, "%#x", dasm->shift);
					strcpy(&(*fptr), fmtBuf);
					len = strlen(fmtBuf);
					break;
			}
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
		if(dasm->rt <= REGIMM_INST_COUNT) {
			dasm->delay = regimmInstructionTable[dasm->rt].delay;
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
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
