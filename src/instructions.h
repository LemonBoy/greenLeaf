#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "types.h"
#include "emulator.h"
#include "dasm.h"

#define NORMAL_INST_COUNT	0x3F
#define SPECIAL_INST_COUNT	0x3F
#define REGIMM_INST_COUNT	0x1F
#define COPROC_INST_COUNT	0x1F
#define COPROC_BC_INST_COUNT	0x1F
#define COPROC_COP0_INST_COUNT	0x1F

extern mipsInstrTbl    cop0InstructionTable[];
extern mipsCopInstrTbl coprocBcInstructionTable[];
extern mipsCopInstrTbl coprocInstructionTable[];
extern mipsInstrTbl    regimmInstructionTable[];
extern mipsInstrTbl    specialInstructionTable[];
extern mipsInstrTbl    instructionTable[];

void execOpcode(mipsDasm *dasm);
char* textOpcode(mipsDasm *dasm);

#endif
