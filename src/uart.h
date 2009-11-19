#ifndef _UART_H
#define _UART_H
#include "types.h"

#define UART_REG_RBR_THR 0
#define UART_REG_IER 	 1
#define UART_REG_IIR_FCR 2
#define UART_REG_LCR 	 3
#define UART_REG_MCR  	 4
#define UART_REG_LSR 	 5
#define UART_REG_MSR  	 6
#define UART_REG_SCR 	 7

int setupUart(mipsCpu* cpu, u32 baseAddr);
u8 readUartByte(mipsCpu* cpu);
void writeUartByte(mipsCpu* cpu);

#endif
