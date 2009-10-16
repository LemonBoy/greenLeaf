#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "types.h"
#include "emulator.h"
#include "dasm.h"

char *registerToName(mipsRegister reg)
{
	return registerName[reg];
}

char * dasmFormat(char *haystack, mipsDasm *dasm)
{
	char formattedStr[128];
	
	char *hptr = haystack;
	char *fptr = formattedStr;
	
	char fmtBuf[16];
		
	while (*hptr)
	{
		int len = 1;

		if (*hptr == '%')
		{
			hptr++;
			switch (*hptr)
			{
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
					len += strlen(fmtBuf);
					break;
				case 'j':
					sprintf(fmtBuf, "%#x", dasm->jump);
					strcpy(&(*fptr), fmtBuf);
					len += strlen(fmtBuf);
					break;
				case 'h':
					sprintf(fmtBuf, "%#x", dasm->shift);
					strcpy(&(*fptr), fmtBuf);
					len += strlen(fmtBuf);
					break;
			}
		} else {
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
	
	if (dasm->instruction != 0)
	{
		switch (dasm->instruction)
		{
			case 0x2:
			case 0x3:
			case 0x4:
			case 0x5:
				dasm->delay = 1; break;
			default:
				dasm->delay = 0;
		}
	} else
	{
		switch (dasm->funct)
		{
			case 0x8:
			case 0x9:
				dasm->delay = 1; break;
			default:
				dasm->delay = 0;
		}
	}	

	*ret = dasm;
}	
