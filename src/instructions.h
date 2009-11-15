#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "types.h"
#include "emulator.h"
#include "dasm.h"

#define NORMAL_INST_COUNT	0x46
#define SPECIAL_INST_COUNT	0x2B
#define REGIMM_INST_COUNT	0x14

extern mipsInstrTbl regimmInstructionTable[];
extern mipsInstrTbl specialInstructionTable[];
extern mipsInstrTbl instructionTable[];

void execOpcode(mipsDasm *dasm);
char* textOpcode(mipsDasm *dasm);

#endif
