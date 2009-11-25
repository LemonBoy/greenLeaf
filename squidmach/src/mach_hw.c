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
	fflush(stdout);
#endif
	while((ch = cpu->readByte(cpu, addr++)) != 0) {
		printf("%c", ch);
		fflush(stdout);
	}
}

void doHWPrintHex64(mipsCpu* cpu, u64 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
	fflush(stdout);
#endif
	printf("%016llX\n", val);
	fflush(stdout);
}

void doHWPrintInt64(mipsCpu* cpu, s64 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
	fflush(stdout);
#endif
	printf("%lld", val);
	fflush(stdout);
}

void doHWPrintHex32(mipsCpu* cpu, u32 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
	fflush(stdout);
#endif
	printf("%08X\n", val);
	fflush(stdout);
}

void doHWPrintInt32(mipsCpu* cpu, s32 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
	fflush(stdout);
#endif
	printf("%d", val);
	fflush(stdout);
}

void doHWPrintHex16(mipsCpu* cpu, u16 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
	fflush(stdout);
#endif
	printf("%04X\n", val);
	fflush(stdout);
}

void doHWPrintInt16(mipsCpu* cpu, s16 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
	fflush(stdout);
#endif
	printf("%d", val);
	fflush(stdout);
}

void doHWPrintHex8(mipsCpu* cpu, u8 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
	fflush(stdout);
#endif
	printf("%02X\n", val);
	fflush(stdout);
}

void doHWPrintInt8(mipsCpu* cpu, s8 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
	fflush(stdout);
#endif
	printf("%d", val);
	fflush(stdout);
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
		case HW_ADDRESS_PRINT_HEX:
			doHWPrintHex8(cpu, value);
			break;
		case HW_ADDRESS_PRINT_INT:
			doHWPrintInt8(cpu, value);
			break;
		default:
			break;
	}
}

void writeHWRegHword(mipsCpu* cpu, u32 address, u16 value)
{
	address -= HW_REGS_ADDR;
	switch(address) {
		case HW_ADDRESS_PRINT_HEX:
			doHWPrintHex16(cpu, value);
			break;
		case HW_ADDRESS_PRINT_INT:
			doHWPrintInt16(cpu, value);
			break;
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
		case HW_ADDRESS_PRINT_HEX:
			doHWPrintHex32(cpu, value);
			break;
		case HW_ADDRESS_PRINT_INT:
			doHWPrintInt32(cpu, value);
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
			doHWPrintHex64(cpu, value);
			break;
		case HW_ADDRESS_PRINT_INT:
			doHWPrintInt64(cpu, value);
			break;
		default:
			break;
	}
}

int HWInit()
{
	return 1;
}


