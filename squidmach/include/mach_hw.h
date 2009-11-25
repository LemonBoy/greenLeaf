#ifndef MACH_HW_REGS_H
#define MACH_HE_REGS_H

#include "types.h"
#include "emulator.h"

enum {
	HW_ADDRESS_VERSION	= 0x00000000,
	HW_ADDRESS_PRINT_TEXT	= 0x00000004,
	HW_ADDRESS_PRINT_HEX	= 0x00000008,
	HW_ADDRESS_PRINT_INT	= 0x00000010,
};

#define HW_VERSION	(0x00000001)

#define HW_REGS_ADDR	(0x40000000)
#define HW_REGS_SIZE	(0x00010000)

u8   readHWRegByte  (mipsCpu* cpu, u32 address);
u16  readHWRegHword (mipsCpu* cpu, u32 address);
u32  readHWRegWord  (mipsCpu* cpu, u32 address);
u64  readHWRegDword (mipsCpu* cpu, u32 address);
void writeHWRegByte (mipsCpu* cpu, u32 address, u8  value);
void writeHWRegHword(mipsCpu* cpu, u32 address, u16 value);
void writeHWRegWord (mipsCpu* cpu, u32 address, u32 value);
void writeHWRegDword(mipsCpu* cpu, u32 address, u64 value);
int  HWInit         ();

#endif
