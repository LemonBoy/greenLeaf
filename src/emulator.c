#include <stdio.h>
#include <assert.h>
#include "memory.h"
#include "emulator.h"
#include "instructions.h"
#include "dasm.h"

/*
 *  Register related stuff. 
 */

void setRegister(mipsReg reg, mipsRegister value)
{
#ifdef DEBUG
	printf("Write 0x%016llX in %s\n", value, registerToName(reg));
#endif
	/* Keep r0 hardwired to 0 */
	if(reg == 0)
		return;
	emulatedCpu.r[reg] = value;
}

mipsRegister readRegister(mipsReg reg)
{
#ifdef DEBUG
	printf("Read %s: 0x%016llX\n", registerToName(reg), emulatedCpu.r[reg]);
#endif	
	return emulatedCpu.r[reg];
}

void setCop0Register(mipsReg reg, mipsRegister value)
{
#ifdef DEBUG
	printf("Cop0 -> Write 0x%016llX in cop0[%u]\n", value, reg);
#endif
	emulatedCpu.cop0[reg] = value;
}

mipsRegister readCop0Register(mipsReg reg)
{
#ifdef DEBUG
	printf("Cop0 -> Read cop0[%u]: 0x%016llX\n", reg, emulatedCpu.cop0[reg]);
#endif	
	return emulatedCpu.cop0[reg];
}

void advancePC(mipsRegister nextPC)
{
	emulatedCpu.pc = emulatedCpu.nPc;
	emulatedCpu.nPc += nextPC;
#ifdef DEBUG	
	printf("Advance -> Next pc is 0x%08X\n", emulatedCpu.nPc);
#endif
}

void doLink(int r, int reg)
{
	int regis = 31;
	if(r)
		regis = reg;
	setRegister(regis, emulatedCpu.pc + DEFAULT_INSTRUCTION_PC);
}

void setJump(mipsJumpAddress jumpAddress)
{
	emulatedCpu.pc = emulatedCpu.nPc;
	emulatedCpu.nPc = (emulatedCpu.pc & 0xf0000000) | (jumpAddress << 2);
#ifdef DEBUG	
	printf("Jump -> Next pc is 0x%08X\n", emulatedCpu.nPc);
#endif
}

void setPC(mipsRegister nextPC)
{
	emulatedCpu.pc = emulatedCpu.nPc;
	emulatedCpu.nPc = nextPC;
#ifdef DEBUG	
	printf("Set -> Next pc is 0x%08X\n", emulatedCpu.nPc);
#endif
}

mipsRegister getNextPC()
{
	return emulatedCpu.nPc;
}

mipsRegister getPC()
{
	return emulatedCpu.pc;
}

void printRegisters()
{
	int reg;
	
	for(reg = 0; reg < 32; reg++)
		printf( "%s: 0x%016llX\tCOP0 %2d: 0x%016llX\n", \
			registerToName(reg), emulatedCpu.r[reg], reg, emulatedCpu.cop0[reg]);
	printf("HI: 0x%016llX\tLO:      0x%016llX\n", emulatedCpu.r[REGISTER_HI], emulatedCpu.r[REGISTER_LO]);
	printf("PC: 0x%08X\tNext PC: 0x%08X\n", emulatedCpu.pc, emulatedCpu.nPc);
}

/* 
 * Initialize the cpu emulation core. 
 */

void initializeCPU(u8 endian, u32 stackPtr)
{
	int reg;
	
	for(reg = 0; reg < 34; reg++) {
		emulatedCpu.r[reg] = 0LL;
		if(reg == 29)
			emulatedCpu.r[reg] = 0LL + stackPtr;
	}
	
	for(reg = 0; reg < 32; reg++)
		emulatedCpu.cop0[reg] = 0LL;
	
	emulatedCpu.pc = 0;
	emulatedCpu.nPc = 0;
	emulatedCpu.bOpcode = 0LL;
	
	emulatedCpu.endian = endian;
	
	switch(emulatedCpu.endian) {
		case ENDIANNESS_LE:
#ifdef DEBUG
			printf("Cpu endianess set to little endian\n");
#endif
			emulatedCpu.readByte   = readByteLE;
			emulatedCpu.readHword  = readHwordLE;
			emulatedCpu.readWord   = readWordLE;
			emulatedCpu.readDword  = readDwordLE;
			
			emulatedCpu.writeByte  = writeByteLE;
			emulatedCpu.writeHword = writeHwordLE;
			emulatedCpu.writeWord  = writeWordLE;
			emulatedCpu.writeDword = writeDwordLE;				
			break;
		
		case ENDIANNESS_BE:
#ifdef DEBUG
			printf("Cpu endianess set to big endian\n");
#endif				
			emulatedCpu.readByte  = readByteBE;
			emulatedCpu.readHword = readHwordBE;
			emulatedCpu.readWord  = readWordBE;
			emulatedCpu.readDword = readDwordBE;
			
			emulatedCpu.writeByte  = writeByteBE;
			emulatedCpu.writeHword = writeHwordBE;
			emulatedCpu.writeWord  = writeWordBE;
			emulatedCpu.writeDword = writeDwordBE;					
			break;
		default:
			printf("Error: Endian setting is neither BE nor LE.\n");
			assert((emulatedCpu.endian == ENDIANNESS_BE) || (emulatedCpu.endian == ENDIANNESS_LE));
			break;
	}
}

/* Todo : Maybe those are better in a .h file along with another one with
 * the exceptions number ? */

enum {
	COP0_REG_CONTEXT 	= 4,
	COP0_REG_BADVADDR 	= 8,
	COP0_REG_COUNT 		= 9,
	COP0_REG_COMPARE 	= 11,
	COP0_REG_STATUS 	= 12,
	COP0_REG_CAUSE 		= 13,
	COP0_REG_EPC 		= 14,
	COP0_REG_WATCHLO 	= 18,
	COP0_REG_WATCHHI 	= 19,
	COP0_REG_XCONTEXT 	= 20,
	COP0_REG_ECC 		= 26,
	COP0_REG_CACHEERR 	= 27,
	COP0_REG_ERROREPC 	= 30
} COP0_REGISTERS;

#define RESET_VECTOR_BEV_0 	(0xFFFFFFFF80000000)
#define RESET_VECTOR_BEV_1 	(0xFFFFFFFFBFC00200)
#define DEFAULT_RESET_VECTOR 	(RESET_VECTOR_BEV_1)

/* Todo : Check if whitin another exception.
   Check out page 141 for all the exception codes.
   Check page 152 and 175 for more about the exception vectors */

void generateException(u32 exception, u32 delay)
{
	mipsRegister resetVectorAddress = DEFAULT_RESET_VECTOR;
	
	printf("Caught exception 0x%08X\n", exception);

	setCop0Register(COP0_REG_EPC, emulatedCpu.pc);
	setCop0Register(COP0_REG_CAUSE, (readCop0Register(COP0_REG_CAUSE) & 0xFFFFFFFFFFFFFF00) | (exception << 2));

	if (delay) {
		setCop0Register(COP0_REG_EPC, readCop0Register(COP0_REG_EPC) - 4);
		/* Set the BV bit in the CAUSE register if we are in a branch delay */
		setCop0Register(COP0_REG_CAUSE, (readCop0Register(COP0_REG_CAUSE) & 0x0FFFFFFFFFFFFF00) | ((mipsRegister)1 << 63));
	} else {
		setCop0Register(COP0_REG_CAUSE, (readCop0Register(COP0_REG_CAUSE) & 0x0FFFFFFFFFFFFF00) | ((mipsRegister)0 << 63));
	}
	
	if (readCop0Register(COP0_REG_STATUS) & 0x200000) { /* BEV bit is set. */
		resetVectorAddress = RESET_VECTOR_BEV_1;
	} else {
		resetVectorAddress = RESET_VECTOR_BEV_0;
	}
	
	if (!(exception >= 1 && exception <= 3)) { /* Nothing TLB related ? */
		resetVectorAddress += 0x180;
	}
	
	printf("Jumping to the reset vector at 0x%016llX\n", resetVectorAddress);
	
	setPC(resetVectorAddress);
}

void executeOpcode(u32 opcode)
{
	mipsDasm *opc;
	
	dasmOpcode(opcode, &opc);

#ifdef DEBUG
	printf("Should use branch delay : %s\n", (opc->delay) ? "Yes" : "No");
	printf(">> (%08X) %s\n", opcode, textOpcode(opc));
#endif
	
	if(opc->delay && emulatedCpu.bOpcode == 0) {
		emulatedCpu.bOpcode = getNextPC();
		advancePC(DEFAULT_INSTRUCTION_PC);
		return;
	}else{
		if(opc->delay)
			emulatedCpu.bOpcode = 0;
	}
	
	if(emulatedCpu.bOpcode != 0) {
		setPC(emulatedCpu.bOpcode);
		return;
	}

	execOpcode(opc);
}

void runProcessor()
{
	u32 opcode;
	if(emulatedCpu.endian == ENDIANNESS_LE) {
		opcode = (u8)(emulatedCpu.readByte(getNextPC() + 3)) << 24 | 
			 (u8)(emulatedCpu.readByte(getNextPC() + 2)) << 16 | 
			 (u8)(emulatedCpu.readByte(getNextPC() + 1)) << 8  | 
			 (u8)(emulatedCpu.readByte(getNextPC()));
	}else{
		opcode = (u32)(emulatedCpu.readWord(getNextPC()));
	}
	executeOpcode(opcode);
}


















