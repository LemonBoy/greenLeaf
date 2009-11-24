#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "binary.h"
#include "types.h"
#include "emulator.h"
#include "memory.h"

#include "machdep/mach_emulator.h"
#include "machdep/mach_memory.h"
#include "machdep/mach_uart.h"

#define TICK_AT_A_TIME

char ascii(char s)
{
	if(s < 0x20) return '.';
	if(s > 0x7E) return '.';
	return s;
}

void __hexdump(void *d, int len) 
{
	u8 *data;
	int i, off;
	data = (u8*)d;
	for(off = 0; off < len; off += 16) {
		printf("%08X ", off);
		for(i = 0; i < 16; i++) {
			if((i + off) >= len)
				printf("   ");
			else
				printf("%02X ", data[off + i]);
		}
		printf(" | ");
		for(i = 0; i < 16; i++) {
			if((i + off) >= len)
				printf(" ");
			else
				printf("%c", ascii(data[off + i]));
		}
		printf("\n");
	}
}

void bitdump (void * data, int sz)
{
	sz = sz << 2;
	printf("Sz %i\n", sz);
	while (sz--) {
		printf("%X ", (((u8 *)*(&data))[0] >> sz) & 0x1);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	char* filename;
	s32 ret;
	mipsCpu* cpu;
	
	printf("greenLeaf 0.1\n");
	printf("mips emulator by The Lemon Man and SquidMan\n");

	if(argc < 2) {	/* No arguments passed. */
		filename = calloc(5, 1);
		sprintf(filename, "test/mmon");
	}else{
		filename = calloc(strlen(argv[1]) + 1, 1);
		sprintf(filename, argv[1]);
	}

#ifdef DEBUG
	printf("Debug mode enabled\n");
#endif
	printf("Initializing the CPU core...\n");
	cpu = initializeCPU(ENDIANNESS_LE, 0x80000000);
	
	printf("Mapping the ram...\n");
	
	printf("Main memory %d\n",	mapMemory(cpu, 0x80000000, 0x40000, FLAG_RAM));
	printf("Reset vector %d\n",	mapMemory(cpu, 0xBFC00000, 0x40000, FLAG_RAM));
	printf("Additional mem %d\n",	mapMemory(cpu, 0xA0000010, 0x2000, FLAG_RAM));
	
//	ret = openRaw(cpu, filename, 0xBFC00000);
	ret = openElf(cpu, filename);

	printf("Entry %#x\n",	(u32)ret);
	
	printf("Uart %i\n",	setupUart(cpu, 0xB40003f8));
	
	setPC(cpu, ret);
	
	printf("Press enter to run a tick and print the registers...\n");
	printf("Press enter to continue.\n");
	for(;;) {
#ifdef TICK_AT_A_TIME
		fgetc(stdin);
#endif
		runProcessor(cpu);
		//~ printRegisters(cpu);
	}
	
	printf("Execution finished... unmapping the ram\n");
	
	unmapMemory(cpu);
	
	free(filename);
	
	return 1;
}
