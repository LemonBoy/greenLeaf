#ifndef _DASM_H
#define _DASM_H

typedef struct {
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

typedef struct {
	mipsInstruction (*execute)(mipsDasm *dasm);
	char textDisasm[20];
	u8 delay;
} mipsInstrTbl;

typedef struct {
	mipsInstruction (*execute)(mipsDasm *dasm, int cop);
	char textDisasm[20];
	u8 delay;
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
void dasmOpcode(u32 opcode, mipsDasm **ret);

#endif
