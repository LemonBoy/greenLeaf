#ifndef _DASM_H
#define _DASM_H

typedef struct _mipsDasm
{
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

typedef struct _mipsInstructionTable
{
	mipsInstruction (*execute)(mipsDasm *dasm);
	u8 textDisasm[20];
} mipsInstrTbl;

//~ static char *registerName[34] = {
	//~ "zr" , "r1" , "r2" , "r3" , "r4" , "r5" , "r6" , "r7" , "r8" , "r9" , "r10",
	//~ "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20",
	//~ "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30",
	//~ "lr", "HI", "LO"
//~ };

static char *registerName[34] = {
	"zr" , "at" , "v0" , "v1" , "a0" , "a1" , "a2" , "a3" , "t0" , "t1" , "t2",
	"t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4",
	"s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp",
	"ra", "HI", "LO"
};

char *registerToName(mipsRegister reg);

char * dasmFormat(char *haystack, mipsDasm *dasm);

void dasmOpcode(u32 opcode, mipsDasm **ret);

#endif
