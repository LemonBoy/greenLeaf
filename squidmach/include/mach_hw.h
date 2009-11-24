#ifndef MACH_HW_REGS_H
#define MACH_HE_REGS_H

#include "types.h"
#include "emulator.h"

#define HW_VERSION	(0x00000001)

#define HW_REGS_ADDR	(0x40000000)
#define HW_REGS_SIZE	(0x00010000)

u8   readHWRegByte (mipsCpu* cpu, u32 address);
void writeHWRegByte(mipsCpu* cpu, u32 address, u8 value);
int  HWInit        ();

#endif
