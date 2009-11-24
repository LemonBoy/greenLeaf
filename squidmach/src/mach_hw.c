#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "binary.h"
#include "types.h"
#include "emulator.h"
#include "memory.h"

#include "mach_memory.h"
#include "mach_hw.h"

u32 getHWRevision(mipsCpu* cpu)
{
	return HW_VERSION;
}

void doHWPrintText(mipsCpu* cpu, u32 addr)
{
	char ch;
#ifdef DEBUG
	printf("Printing Text: ");
#endif
	while((ch = cpu->readByte(cpu, addr++)) != 0) {
		printf("%c", ch);
	}
}

void doHWPrintHex(mipsCpu* cpu, u64 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
#endif
	printf("%016llX\n", val);
}

u8 readHWRegByte(mipsCpu* cpu, u32 address)
{
	u8 ret = 0;
	address -= HW_REGS_ADDR;
	switch(address) {
		default:
			break;
	}
	return ret;
}

u16 readHWRegHword(mipsCpu* cpu, u32 address)
{
	u16 ret = 0;
	address -= HW_REGS_ADDR;
	switch(address) {
		default:
			break;
	}
	return ret;
}

u32 readHWRegWord(mipsCpu* cpu, u32 address)
{
	u32 ret = 0;
	address -= HW_REGS_ADDR;
	switch(address) {
		case HW_ADDRESS_VERSION:
			ret = getHWRevision(cpu);
			break;
		default:
			break;
	}
	return ret;
}

u64 readHWRegDword(mipsCpu* cpu, u32 address)
{
	u64 ret = 0LL;
	address -= HW_REGS_ADDR;
	switch(address) {
		default:
			break;
	}
	return ret;
}

void writeHWRegByte(mipsCpu* cpu, u32 address, u8 value)
{
	address -= HW_REGS_ADDR;
	switch(address) {
		default:
			break;
	}
}

void writeHWRegHword(mipsCpu* cpu, u32 address, u16 value)
{
	address -= HW_REGS_ADDR;
	switch(address) {
		default:
			break;
	}
}

void writeHWRegWord(mipsCpu* cpu, u32 address, u32 value)
{
	address -= HW_REGS_ADDR;
	switch(address) {
		case HW_ADDRESS_PRINT_TEXT:
			doHWPrintText(cpu, value);
			break;
		default:
			break;
	}
}

void writeHWRegDword(mipsCpu* cpu, u32 address, u64 value)
{
	address -= HW_REGS_ADDR;
	switch(address) {
		case HW_ADDRESS_PRINT_HEX:
			doHWPrintHex(cpu, value);
			break;
		default:
			break;
	}
}

int HWInit()
{
	return 1;
}


