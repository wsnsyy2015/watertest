#include "global.h"

unsigned char Second = 0;
unsigned char Minute = 0;
unsigned char Hour = 0;

#if 0
/**
  *****************************************************************************
  * @Name   : 将BCD转换为BIN
  *
  * @Brief  : none
  *
  * @Input  : BCDValue: 输入BCD
  *
  * @Output : none
  *
  * @Return : BIN格式数值
  *****************************************************************************
**/
static unsigned char RTC_Bcd2ToBin(unsigned char BCDValue)
{
	unsigned char tmp = 0;
	
	tmp = ((unsigned char)(BCDValue & (unsigned char)0xF0) >> (unsigned char)0x04) * 10;
	return (tmp + (BCDValue & (unsigned char)0x0F));
}

#endif

//--------------------------------------------------
//读时间
//BCD格式数据
//---------------------------------------------------
void PCF865Read(void)
{
    unsigned char TimeBuf[3];
    TimeBuf[0] = (0x7F&SM_Receive(CHIP_PCF8563,REG_MIN));
    TimeBuf[1] = (0x3F&SM_Receive(CHIP_PCF8563,REG_HOUR)); 
    //TimeBuf[3] = (0x7F&SM_Receive(CHIP_PCF8563,REG_SEC));
    Minute = TimeBuf[0];               //RTC_Bcd2ToBin(TimeBuf[0]);
    Hour = TimeBuf[1];                    //RTC_Bcd2ToBin(TimeBuf[1]); 
    //Second = TimeBuf[2];                //RTC_Bcd2ToBin(TimeBuf[2]);
}



