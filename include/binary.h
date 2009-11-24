#ifndef _BINARY_H
#define _BINARY_H

#include "types.h"
#include "emulator.h"

u32 openElf32(mipsCpu* cpu, char *path);
u64 openElf64(mipsCpu* cpu, char *path);
u32 openRaw(mipsCpu* cpu, char *path, u32 addr);

#endif
