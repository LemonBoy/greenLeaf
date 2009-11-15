#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"

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

char* dasmFormat(char *haystack, mipsDasm *dasm)
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
					len += strlen(registerToName(dasm->rt)) - 1;
					break;
				case 's':
					strcpy(&(*fptr), registerToName(dasm->rs));
					len += strlen(registerToName(dasm->rs)) - 1;
					break;
				case 'd':
					strcpy(&(*fptr), registerToName(dasm->rd));
					len += strlen(registerToName(dasm->rd)) - 1;
					break;										
				case 'i':
					sprintf(fmtBuf, "%#x", dasm->immediate);
					strcpy(&(*fptr), fmtBuf);
					len += strlen(fmtBuf) - 1;
					break;
				case 'j':
					sprintf(fmtBuf, "%#x", dasm->jump);
					strcpy(&(*fptr), fmtBuf);
					len += strlen(fmtBuf) - 1;
					break;
				case 'h':
					sprintf(fmtBuf, "%#x", dasm->shift);
					strcpy(&(*fptr), fmtBuf);
					len += strlen(fmtBuf) - 1;
					break;
			}
		}else{
			*fptr = *hptr;
		}
		fptr += len;
		hptr++;
	}
	
	*fptr = '\0';
	
	return strdup(formattedStr);
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
	
	if(dasm->instruction != 0) { /* Normal instructions. */
		switch(dasm->instruction) {
			case 0x2:
			case 0x3:
			case 0x4:
			case 0x5:
			case 0x6:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
				dasm->delay = 1;
				break;
			default:
				dasm->delay = 0;
				break;
		}
	}else if(dasm->instruction == 0) { /* Special instructions. */
		switch(dasm->funct) {
			case 0x8:
			case 0x9:
				dasm->delay = 1;
				break;
			default:
				dasm->delay = 0;
				break;
		}
	}else if(dasm->instruction == 1) { /* REGIMM instructions. */
		switch(dasm->rt) {
			case 0x0:
			case 0x1:
			case 0x3:
			case 0x7:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
				dasm->delay = 1;
				break;
			default:
				dasm->delay = 0;
				break;
		}
	}

	*ret = dasm;
}	
