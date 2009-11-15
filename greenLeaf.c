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
	u32 opcode;
	char* filename;
	s32 ret;
	
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
	printf("Mapping the ram...\n");
	
	printf("Main memory %d\n",	mapMemory(0x80000000, 0x40000, FLAG_RAM));
	printf("Reset vector %d\n",	mapMemory(0xBFC00000, 0x40000, FLAG_RAM));
	printf("Addictional mem %d\n",	mapMemory(0xA0000010, 0x02000, FLAG_RAM));
	
	printf("Initializing the CPU core...\n");
	initializeCPU(ENDIANNESS_LE, 0x80000000);

	ret = openElf(filename, 0xBFC00000);

	printf("Entry %#x\n",	(u32)ret);
	
	printf("Uart %i\n",	setupUart(0xB40003F0));
	
	setPC(0xBFC00000);
	
	printf("Press enter to run a tick and print the registers...\n");
	printf("Press enter to continue.\n");
	for(;;) {
		fgetc(stdin);
		if(emulatedCpu.endian == ENDIANNESS_LE) {
			opcode = (u8)(emulatedCpu.readByte(getNextPC() + 3)) << 24 | 
				 (u8)(emulatedCpu.readByte(getNextPC() + 2)) << 16 | 
				 (u8)(emulatedCpu.readByte(getNextPC() + 1)) << 8  | 
				 (u8)(emulatedCpu.readByte(getNextPC()));
		}else{
			opcode = (u32)(emulatedCpu.readWord(getNextPC()));
		}
		
		/*if(getNextPC() >= 0xBFC0002C) {
			break;
		}*/
		
		executeOpcode(opcode);
		printf("Uart character %c\n", readUartByte());
		printRegisters();
	}
	
	printf("Execution finished... unmapping the ram\n");
	unmapMemory();
	free(filename);
	return 1;
}
