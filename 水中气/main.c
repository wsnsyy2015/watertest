
#include "global.h"


void main(void){
    unsigned char maintmp[2];
    EA = DISABLE;
    WDog_Set();
    WDog_Disable();
    SYSTEM_Init();
    Port_Init();   
    maintmp[0] = FlashReadByte(0x6A06,SELECT_64K);
    if(maintmp[0] == 0x00)//不需要大范围调试，读出频率
    {        
        bTSKG  = ON;
        FreqLargeRangeSearchONOFF = OFF;
        bValveState = OFF;
        maintmp[0] = FlashReadByte(0x6A00,SELECT_64K);
        maintmp[1] = FlashReadByte(0x6A01,SELECT_64K);
        uiHostUpValue = maintmp[1];
        uiHostUpValue += (maintmp[0]<<8);
        maintmp[0] = FlashReadByte(0x6A02,SELECT_64K);
        maintmp[1] = FlashReadByte(0x6A03,SELECT_64K);
        uiHostDownValue = maintmp[1];
        uiHostDownValue += (maintmp[0]<<8);
        maintmp[0] = FlashReadByte(0x6A07,SELECT_64K);
        maintmp[1] = FlashReadByte(0x6A08,SELECT_64K);
        F_FIND = maintmp[1];
        F_FIND += (maintmp[0]<<8);
        maintmp[0] = FlashReadByte(0x6A0B,SELECT_64K);
        maintmp[1] = FlashReadByte(0x6A0C,SELECT_64K);
        uiSignalZeroPosition = maintmp[1];
        uiSignalZeroPosition += (maintmp[0]<<8);
        maintmp[0] = FlashReadByte(0x6A0D,SELECT_64K);
        maintmp[1] = FlashReadByte(0x6A0E,SELECT_64K);
        uiSignalRange = maintmp[1];
        uiSignalRange += (maintmp[0]<<8);
        F_START = F_FIND - SmallFreqRange;
        F_END = F_FIND + SmallFreqRange;
        F_OUT = F_START;
        F_ADC = 0;
        uiPWMCount = 0;
        TH0 = (unsigned char)(F_OUT>>8);
        TL0 = (unsigned char)(F_OUT&0xFF);
    }
    else
    {
        uiHostUpValue = 2000;
        uiHostDownValue = 500;
        uiSignalZeroPosition = 0;
        uiSignalRange = 2000;
        bTSKG = ON;
        FreqLargeRangeSearchONOFF = ON;
        F_ADC = 0;
        F_FIND = 0;
        bValveState = OFF;
        F_OUT = T0_Set_LargeRange_Start;
        F_END = T0_Set_LargeRange_End;
        uiPWMCount = 0;
        TH0 = (unsigned char)(F_OUT>>8);
        TL0 = (unsigned char)(F_OUT&0xFF);
        
//2016-04-01_updata           
    }
    bChangeTime = 0;
    bChange = 0; 
    UART0_Init(BaudRate_9600);
    UART1_Init(BaudRate_9600);
    ADDA_Init();
    Timer3_Init();
    INT_Init();
    Timer0_Init(F_OUT);
    Timer1_Init();    
    SMBus_Init();    
    EA = ENABLE;
    SM_Send(CHIP_PCF8563,0x00,0x00);
    while(1)
    {        
        WDog_Feed();        
        PCF865Read();
        KeyService();
        LCDRefresh();
    }
}

//-----------------------------------------------
#if 0

#endif