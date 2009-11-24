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

u8 getHWRevision(mipsCpu* cpu, u32 address)
{
	u32 version = HW_VERSION;
	if(cpu->endian == ENDIANNESS_BE)
		return (version >> (address * 8)) & 0xFF;
	else
		return (version >> ((3 - address) * 8)) & 0xFF;
}

u8 readHWRegByte(mipsCpu* cpu, u32 address)
{
	if((address >= 0x40000000) && (address < 0x40000004)) {
		return getHWRevision(cpu, address - 0x40000000);
	}
	return 0;
}

void writeHWRegByte(mipsCpu* cpu, u32 address, u8 value)
{

}

int HWInit()
{
	return 1;
}
