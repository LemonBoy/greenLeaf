#include <stdio.h>
#include "types.h"
#include "memory.h"
#include "emulator.h"
#include "uart.h"

static u8  _uartBuffer;
static u32 _uartAddress;

int setupUart(mipsCpu* cpu, u32 baseAddr)
{
	if(mapMemory(cpu, baseAddr, 0xF, FLAG_RAM) < 0) {
#ifdef DEBUG
		printf("Cannot allocate space for UART buffer\n");
#endif
		return -1;
	}
		
	_uartAddress = baseAddr;
	
	/* Set up parameters. */
	cpu->writeByte(cpu, _uartAddress + UART_REG_LCR, 0xE0);
	
	return 1;
}

u8 readUartByte(mipsCpu* cpu)
{
	if(_uartAddress == 0) {
#ifdef DEBUG
		printf("Uart not initialized\n");
#endif		
		return 0;
	}
	
	if(cpu->readByte(cpu, _uartAddress + UART_REG_IIR_FCR) & 0x20)
		cpu->writeByte(cpu, _uartAddress + UART_REG_IIR_FCR, 0x0);
	
	return cpu->readByte(cpu, _uartAddress + UART_REG_RBR_THR);
}

void writeUartByte(mipsCpu* cpu)
{
	/* This function needs work! It's not correct! */
	if(_uartAddress == 0) {
#ifdef DEBUG
		printf("Uart not initialized\n");
#endif		
		return;
	}
	cpu->writeByte(cpu, _uartAddress + UART_REG_RBR_THR, _uartBuffer);
}
