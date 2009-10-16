#include <stdio.h>
#include "types.h"
#include "memory.h"
#include "emulator.h"

static u8 uartBuffer;
static u32 uartAddress;
           
int setupUart(u32 baseAddr)
{
	if (mapMemory(baseAddr, 0xf, FLAG_RAM) < 0)
	{
#ifdef DEBUG
		printf("Cannot allocate space for UART buffer\n");
#endif
		return -1;
	}
	
	uartAddress = baseAddr;
	
	/* Set up parameters. */
	emulatedCpu.writeByte(uartAddress + UART_REG_LCR, 0xE0);
	
	return 1;
}

u8 readUartByte()
{
	if (uartAddress == 0)
	{
#ifdef DEBUG
		printf("Uart not initialized\n");
#endif		
		return 0;
	}
	
	if (emulatedCpu.readByte(uartAddress + UART_REG_IIR_FCR) & 0x20)
	{
		emulatedCpu.writeByte(uartAddress + UART_REG_IIR_FCR, 0x0);
	}
	
	return emulatedCpu.readByte(uartAddress + UART_REG_RBR_THR);
}
