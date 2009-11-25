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
#include "graphics.h"
#include "input.h"

u32 getHWRevision(mipsCpu* cpu)
{
	return HW_VERSION;
}

void doHWPower(mipsCpu* cpu, u32 value)
{
	if(value & HW_POWERON_GRAPHICS) {
#ifdef DEBUG
		printf("Graphics turned on.\n");
#endif
		if(!graphics_poweron())
#ifdef DEBUG
			printf("FAILED.\n");
#else
			;
#endif
	}
	if(value & HW_POWERON_INPUT) {
#ifdef DEBUG
		printf("Input turned on.\n");
#endif
		if(!input_poweron())
#ifdef DEBUG
			printf("FAILED.\n");
#else
			;
#endif
	}
}

void doHWPrintText(mipsCpu* cpu, u32 addr)
{
	char ch;
#ifdef DEBUG
	printf("Printing Text: ");
#endif
	while((ch = cpu->readByte(cpu, addr++)) != 0) {
		graphics_printf("%c", ch);
		}
}

void doHWPrintHex64(mipsCpu* cpu, u64 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
#endif
	graphics_printf("%016llX", val);
}

void doHWPrintInt64(mipsCpu* cpu, s64 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
#endif
	graphics_printf("%lld", val);
}

void doHWPrintHex32(mipsCpu* cpu, u32 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
#endif
	graphics_printf("%08X", val);
}

void doHWPrintInt32(mipsCpu* cpu, s32 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
#endif
	graphics_printf("%d", val);
}

void doHWPrintHex16(mipsCpu* cpu, u16 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
#endif
	graphics_printf("%04X", val);
}

void doHWPrintInt16(mipsCpu* cpu, s16 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
#endif
	graphics_printf("%d", val);
}

void doHWPrintHex8(mipsCpu* cpu, u8 val)
{
#ifdef DEBUG
	printf("Printing Hex: ");
#endif
	graphics_printf("%02X", val);
}

void doHWPrintInt8(mipsCpu* cpu, s8 val)
{
#ifdef DEBUG
	printf("Printing Integer: ");
#endif
	graphics_printf("%d", val);
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
		case HW_ADDRESS_POWER:
			doHWPower(cpu, value);
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
	if(!graphics_setup())
		return 0;
	if(!input_setup())
		return 0;
	return 1;
}

void HWPreUpdate()
{
	input_pre_update();
	graphics_pre_update();
}

void HWPostUpdate()
{
	graphics_post_update();
	input_post_update();
}

