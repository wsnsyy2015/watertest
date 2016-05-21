#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "c8051f020.h"
#include "SYSTEM_INIT.h"
#include "lcd.h"
#include "uart.h"
#include "interrupt.h"
#include "flash.h"
#include "key.h"
#include "smbus.h"
#include "pcf8563.h"
//#include <intrins.h>

#define	SYSCLK	                            22.118400   //22.1148MHZ
#define PWM_Frequency_LargeRange_Start           1500        //1500HZ ´ó·¶Î§ËÑË÷
#define PWM_Frequency_LargeRange_End             1900
#define T0_Set_LargeRange_Start                  (unsigned int)(65536-500000*SYSCLK/PWM_Frequency_LargeRange_Start)
#define T0_Set_LargeRange_End                    (unsigned int)(65536-500000*SYSCLK/PWM_Frequency_LargeRange_End)

#define DISABLE               0
#define ENABLE                1
#define HIGH                  1
#define LOW                   0
#define ON                    1
#define OFF                   0

#define AIN_Signal            0x01
#define AIN_Temperature       0x00



#endif