#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "binary.h"
#include "types.h"
#include "emulator.h"
#include "memory.h"

void uartOutput(char c)
{
	FILE * f = fopen("uart.txt", "a+");
	fwrite(&c, 1, 1, f);
	fclose(f);
}

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
	for (off=0; off<len; off += 16) 
	{
		printf("%08x ", off);
		for(i=0; i<16; i++)
			if((i+off)>=len) printf("   ");
			else printf("%02x ",data[off+i]);
		
		printf(" ");
		for(i=0; i<16; i++)
			if((i+off)>=len) printf(" ");
			else printf("%c",ascii(data[off+i]));
	printf("\n");
	}
}

int main ()
{
	u8 *b;
	u32 opcode;
	
	printf("greenLeaf 0.1\n");
	printf("mips emulator by The Lemon Man\n");
#ifdef DEBUG
	printf("Debug mode enabled\n");
#endif

	printf("Mapping the ram...\n");
	
	printf("Main memory %i\n", mapMemory(0x80000000, 0x40000, FLAG_RAM));
	printf("Reset vector %i\n", mapMemory(0xbfc00000, 0x40000, FLAG_RAM));
	printf("Addictional mem %i\n", mapMemory(0xa0000010, 0x2000, FLAG_RAM));
	
	printf("Initializing the CPU core...\n");

	initializeCPU(0x01, 0x80000000);

	printf("Entry %#x\n", openElf("mmon", 0xbfc00000));
	
	printf("Uart %i\n", setupUart(0xb40003f0));
	
	emulatedCpu.nPc = (0xbfc00000);
	
	for (;;)
	{
		if (emulatedCpu.endian == 0x01) {			
			opcode = (u8)(emulatedCpu.readByte(getNextPC() + 3)) << 24 |
			 (u8)(emulatedCpu.readByte(getNextPC() + 2)) << 16 | 
			 (u8)(emulatedCpu.readByte(getNextPC() + 1)) << 8 | 
			 (u8)(emulatedCpu.readByte(getNextPC()));			 
		} else {
			opcode = (u32)(emulatedCpu.readWord(getNextPC()));
		}
		
		//~ if (getNextPC() >= 0xbfc0002c) {
			//~ break;
		//~ }
		
		executeOpcode(opcode);
		
		printf("Uart character %c\n", readUartByte());
		
		fgetc(stdin);
		
		printRegisters();
		
		fgetc(stdin);
	}
	
	printf("Execution finished...unmapping the ram\n");
	
	unmapMemory();
	
	return 1;
}
