#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "binary.h"
#include "types.h"
#include "emulator.h"
#include "memory.h"

#include "mach_emulator.h"
#include "mach_memory.h"
#include "mach_hw.h"

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
	s64 ret;
	mipsCpu* cpu;
	
	printf("SquidEmu\n");
	printf("A test architecture using greenLeaf by SquidMan\n");

	if(argc < 2) {	/* No arguments passed. */
		filename = calloc(5, 1);
		sprintf(filename, "test.elf");
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
	
	ret = mapMemory(cpu, 0x00000000, 0x00010000, FLAG_RAM);
	printf("Fast memory %lld\n",		ret);
	if(ret < 0) {
		printf("Failed.\n");
		exit(7);
	}
	ret = mapMemory(cpu, 0x80000000, 0x00400000, FLAG_RAM);
	printf("Main memory %lld\n",		ret);
	if(ret < 0) {
		printf("Failed.\n");
		exit(6);
	}
	ret = mapMemory(cpu, 0xBFC00000, 0x00400000, FLAG_ROM);
	printf("Bootstrap %lld\n",		ret);
	if(ret < 0) {
		printf("Failed.\n");
		exit(5);
	}
	ret = mapMemory(cpu, 0xC0000000, 0x08000000, FLAG_ROM);
	printf("ROM %lld\n",			ret);
	if(ret < 0) {
		printf("Failed.\n");
		exit(4);
	}
	ret = mapMemory(cpu, 0x10000000, 0x000000FF, FLAG_RAM);
	printf("Bootstrap Support %lld\n",	ret);
	if(ret < 0) {
		printf("Failed.\n");
		exit(3);
	}
	
	printf("Initializing the Hardware...\n");
	if(!HWInit()) {
		printf("Unable to init hardware.\n");
		exit(2);
	}
	
	printf("Loading bootstrap/BIOS...\n");
	ret = openElf64(cpu, "bootstrap.elf");
	if(ret < 0) {
		printf("Error loading bootstrap/BIOS.\n");
		unmapMemory(cpu);
		free(filename);
		exit(1);
	}
	printf("Loaded. Entrypoint 0x%08X\n", (u32)ret);

	printf("Loading ROM...\n");
	ret = openElf64(cpu, filename);
	if(ret < 0) {
		printf("Error loading ROM.\n");
		unmapMemory(cpu);
		free(filename);
		exit(1);
	}
	printf("Loaded.\n");
	
	setPC(cpu, 0xBFC00000);
	
	printf("Press enter to run a tick and print the registers...\n");
	for(;;) {
#ifdef TICK_AT_A_TIME
		fgetc(stdin);
#endif
		runProcessor(cpu);
//		printRegisters(cpu);
	}
	
	printf("Execution finished... unmapping the ram\n");
	
	unmapMemory(cpu);
	
	free(filename);
	
	return 0;
}
