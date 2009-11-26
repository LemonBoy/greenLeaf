#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "types.h"
#include "emulator.h"

#define NORMAL_INST_COUNT	0x3F
#define SPECIAL_INST_COUNT	0x3F
#define REGIMM_INST_COUNT	0x1F
#define COPROC_INST_COUNT	0x1F
#define COPROC_BC_INST_COUNT	0x1F
#define COPROC_COP0_INST_COUNT	0x1F

void execOpcode(mipsCpu* cpu, void *d);
char* textOpcode(mipsCpu* cpu, void *d);

#endif
