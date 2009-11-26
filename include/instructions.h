#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "types.h"
#include "emulator.h"

extern mipsInstrTbl cop0InstructionTable[];
extern mipsCopInstrTbl coprocBcInstructionTable[];
extern mipsCopInstrTbl coprocInstructionTable[];
extern mipsInstrTbl regimmInstructionTable[];
extern mipsInstrTbl specialInstructionTable[];
extern mipsInstrTbl instructionTable[];

#include "dasm.h"

void cop0Handler(mipsCpu* cpu, mipsDasm *dasm);
void copxHandler(mipsCpu* cpu, mipsDasm *dasm);

INLINE void execOpcode(mipsCpu* cpu, void *d)
{
	mipsDasm *dasm = d;
#ifdef DEBUG		
	printf("Instruction 0x%08X Function 0x%08X\n", dasm->instruction, dasm->funct);
#endif
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			specialInstructionTable[dasm->funct].execute(cpu, dasm);
			advanceCycles(cpu, specialInstructionTable[dasm->funct].cycles);
			advancePC(cpu, specialInstructionTable[dasm->funct].pc);
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			regimmInstructionTable[dasm->rt].execute(cpu, dasm);
			advanceCycles(cpu, regimmInstructionTable[dasm->rt].cycles);
			advancePC(cpu, regimmInstructionTable[dasm->rt].pc);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if (dasm->rs <= COPROC_INST_COUNT) {
			if(dasm->rs == 0x8) {
				if(dasm->rs <= COPROC_BC_INST_COUNT) {
					if(coprocBcInstructionTable[dasm->rt].cycles == 0)
						copxHandler(cpu, dasm);
					else {
						coprocBcInstructionTable[dasm->rt].execute(cpu, dasm, dasm->instruction & 0x3);
						advanceCycles(cpu, coprocBcInstructionTable[dasm->rt].cycles);
						advancePC(cpu, coprocBcInstructionTable[dasm->rt].pc);
					}
				}else{
#ifdef DEBUG
					printf("rt is too high!\n");
#endif
				}
			}else if(dasm->rs & 0x10) {
				if(dasm->funct <= COPROC_COP0_INST_COUNT) {
					if(cop0InstructionTable[dasm->funct].cycles == 0)
						cop0Handler(cpu, dasm);
					else {
						cop0InstructionTable[dasm->funct].execute(cpu, dasm);
						advanceCycles(cpu, cop0InstructionTable[dasm->funct].cycles);
						advancePC(cpu, cop0InstructionTable[dasm->funct].pc);
					}
				}else{
#ifdef DEBUG
					printf("Function is too high!\n");
#endif
				}
			}else{
				if(coprocInstructionTable[dasm->rs].cycles == 0)
					copxHandler(cpu, dasm);
				else {
					coprocInstructionTable[dasm->rs].execute(cpu, dasm, dasm->instruction & 0x3);
					advanceCycles(cpu, coprocInstructionTable[dasm->rs].cycles);
					advancePC(cpu, coprocInstructionTable[dasm->rs].pc);
				}
			}
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
#endif
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			instructionTable[dasm->instruction].execute(cpu, dasm);
			advanceCycles(cpu, instructionTable[dasm->instruction].cycles);
			advancePC(cpu, instructionTable[dasm->instruction].pc);
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
		}
	}
}

INLINE char* textOpcode(mipsCpu* cpu, void *d)
{
	mipsDasm *dasm = dasm;
	if(dasm->instruction == 0) {
		if(dasm->funct <= SPECIAL_INST_COUNT) {
			return dasmFormat(specialInstructionTable[dasm->funct].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("Function is too high!\n");
#endif
			return "Not implemented";
		}
	}else if(dasm->instruction == 1) {
		if(dasm->rt <= REGIMM_INST_COUNT) {
			return dasmFormat(regimmInstructionTable[dasm->rt].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("rt is too high!\n");
#endif
			return "Not implemented";
		}
	}else if((dasm->instruction & ~0x3) == 0x10) {
		if(dasm->rs <= COPROC_INST_COUNT) {
			if(dasm->rs == 0x8)
				return dasmFormat(coprocBcInstructionTable[dasm->rt].textDisasm, dasm);
			else if(dasm->rs & 0x10)
				return dasmFormat(cop0InstructionTable[dasm->funct].textDisasm, dasm);
			else
				return dasmFormat(coprocInstructionTable[dasm->rs].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("rs is too high!\n");
#endif
			return "Not implemented";
		}
	}else{
		if(dasm->instruction <= NORMAL_INST_COUNT) {
			return dasmFormat(instructionTable[dasm->instruction].textDisasm, dasm);
		}else{
#ifdef DEBUG
			printf("Instruction is too high!\n");
#endif
			return "Not implemented";
		}
	}
}

#endif
