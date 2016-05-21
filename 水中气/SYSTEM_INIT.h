#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__
#include "global.h"
//#include <c8051f020.h>


#define WDog_Disable()        WDTCN=0xDE;\
                                 WDTCN=0xAD
                         
#define WDog_Set()            WDTCN=0x07                        
#define WDog_Feed()           WDTCN=0xA5

extern void SYSTEM_Init(void);
extern void Port_Init(void);
extern void Timer0_Init(unsigned int F_Set);
extern void Timer1_Init(void);
extern void Timer3_Init(void);
extern void ADDA_Init(void);
//extern void UART0_Init(unsigned int Baudrate);
//extern void UART1_Init(unsigned int Baudrate);

extern void INT_Init(void);


#endif