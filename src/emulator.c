#include <stdio.h>
#include <assert.h>
#include "memory.h"
#include "emulator.h"
#include "instructions.h"
#include "dasm.h"

/*
 *  Register related stuff. 
 */

void setRegister(mipsRegister reg, u32 value)
{
#ifdef DEBUG
	printf("Write 0x%08X in %s\n", value, registerToName(reg));
#endif
	/* Keep r0 hardwired to 0 */
	if(reg == 0)
		return;
	emulatedCpu.r[reg] = value;
}

mipsRegister readRegister(mipsRegister reg)
{
#ifdef DEBUG
	printf("Read %s\n", registerToName(reg));
#endif	
	return emulatedCpu.r[reg];
}

void setCop0Register(mipsRegister reg, u32 value)
{
#ifdef DEBUG
	printf("Cop0 -> Write 0x%08X in cop0[%i]\n", value, reg);
#endif
	emulatedCpu.cop0[reg] = value;
}

mipsRegister readCop0Register(mipsRegister reg)
{
#ifdef DEBUG
	printf("Cop0 -> Read cop0[%i]\n", reg);
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
		printf( "%s: 0x%08llX\tCOP0 %2d: 0x%08llX\n", \
			registerToName(reg), emulatedCpu.r[reg], reg, emulatedCpu.cop0[reg]);
	printf("HI: 0x%08llX\tLO:      0x%08llX\n", emulatedCpu.r[REGISTER_HI], emulatedCpu.r[REGISTER_LO]);
	printf("PC: 0x%08X\tNext PC: 0x%08X\n", emulatedCpu.pc, emulatedCpu.nPc);
}

/* 
 * Initialize the cpu emulation core. 
 */

void initializeCPU(u8 endian, u32 stackPtr)
{
	int reg;
	
	for(reg = 0; reg < 34; reg++) {
		emulatedCpu.r[reg] = 0x0;
		if(reg == 29)
			emulatedCpu.r[reg] = stackPtr;
	}
	
	for(reg = 0; reg < 32; reg++)
		emulatedCpu.cop0[reg] = 0x0;
	
	emulatedCpu.pc = 0;
	emulatedCpu.nPc = 0;
	emulatedCpu.bOpcode = 0;
	
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

void generateException(int exception, int delay)
{
	printf("Caught exception 0x%08X\n", exception);

	setCop0Register(13, (delay) ? (emulatedCpu.cop0[13] << 31) | 1 : 0);
	setCop0Register(13, (emulatedCpu.cop0[13] & 0xFFFFFF00) | exception << 2);

	setCop0Register(14, emulatedCpu.pc);
	
	if (delay) {
		printf("Setting branch delay\n");
		setCop0Register(14, readCop0Register(14) - 4);
		setCop0Register(13, readCop0Register(13) | 0x80000000);
	}
	
	printf("Jumping to the reset vector\n");
	setPC(0xBFC00200 + 0x180);
}

void executeOpcode(u32 opcode)
{
	mipsDasm *opc;
	
	dasmOpcode(opcode, &opc);

#ifdef DEBUG
	printf("Should use branch delay : %s\n", (opc->delay) ? "Yes" : "No");
	printf(">> %s\n", textOpcode(opc));
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
