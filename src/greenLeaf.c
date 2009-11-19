#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "binary.h"
#include "types.h"
#include "emulator.h"
#include "memory.h"
#include "uart.h"

FILE* uartfile;

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
	printf("Addictional mem %d\n",	mapMemory(cpu, 0xA0000010, 0x02000, FLAG_RAM));
	
	ret = openElf(cpu, filename, 0xBFC00000);

	printf("Entry %#x\n",	(u32)ret);
	
	printf("Uart %i\n",	setupUart(cpu, 0xB40003F0));
	
	setPC(cpu, 0xBFC00000);
	
	printf("Press enter to run a tick and print the registers...\n");
	printf("Press enter to continue.\n");
	for(;;) {
		fgetc(stdin);
		runProcessor(cpu);
//		printRegisters(cpu);
	}
	
	printf("Execution finished... unmapping the ram\n");
	unmapMemory(cpu);
	free(filename);
	return 1;
}
