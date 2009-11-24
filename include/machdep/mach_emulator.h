#ifndef MACH_EMULATOR_H
#define MACH_EMULATOR_H

#include "types.h"
#include "emulator.h"

mipsCpu*	initializeCPU	 (u8 endian, u32 stackPtr);

#endif
