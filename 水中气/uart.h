#ifndef __UART_H__
#define __UART_H__
//#include <c8051f020.h>
#include "global.h"

#define UART0_DE       P24
#define UART1_DE       P25

#define BaudRate_4800         (unsigned int)(65536L-((SYSCLK*1000000L)/(4800L*32L)))
#define BaudRate_9600         (unsigned int)(65536L-((SYSCLK*1000000L)/(9600L*32L)))
#define BaudRate_19200        (unsigned int)(65536L-((SYSCLK*1000000L)/(19200L*32L)))

extern void Uart1_Send_Byte(unsigned char ch);
extern void Uart1_Send_String(unsigned char *ch);
extern void UART0_Init(unsigned int Baudrate);
extern void UART1_Init(unsigned int Baudrate);
//extern void UART1_Init(void);



#endif