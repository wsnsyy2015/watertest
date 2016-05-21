#include <intrins.h>
#include "SYSTEM_INIT.h"
#include "global.h"




void Delay_SYSCLK(void){
    unsigned int i;
    for(i=0;i<256;i++);   
} 
//----------------------------
//
//----------------------------------
void Variate_Init()
{
    P20 = 0;          //电磁阀
    bTSKG = ON;
    bBeng = OFF;
    bSzbd = OFF;
    DisplayIndex = 0;
    PassWordH = 0x00;         //BCD
    PassWordL = 0x00;
    InputPassWord1 = 0;
    InputPassWord2 = 0;
    InputPassWord3 = 0;
    InputPassWord4 = 0;
    //F_FIND = 0;
    //F_OUT = T0_Set_LargeRange_Start;
    //F_END = T0_Set_LargeRange_End;
    
        
}
/*************************************************/
/***********系统时钟配置**************************/

void SYSTEM_Init(void){   
    
     OSCXCN=0x67;
     Delay_SYSCLK();
     while(!(OSCXCN&0x80));
     OSCICN=0x88;   ;//选择外部时钟，使能时钟丢失检测器
     RSTSRC=0x00;
     Variate_Init();         
}
/*****************IO口配置**********************/
// P0.0  -  TX0 (UART0), Open-Drain, Digital
    // P0.1  -  RX0 (UART0), Open-Drain, Digital
    // P0.2  -  SCK  (SPI0), Open-Drain, Digital
    // P0.3  -  MISO (SPI0), Open-Drain, Digital
    // P0.4  -  MOSI (SPI0), Open-Drain, Digital
    // P0.5  -  NSS  (SPI0), Open-Drain, Digital
    // P0.6  -  Skipped,     Open-Drain, Digital
    // P0.7  -  Skipped,     Open-Drain, Digital

    // P1.0  -  SDA (SMBus), Open-Drain, Digital
    // P1.1  -  SCL (SMBus), Open-Drain, Digital
    // P1.2  -  TX1 (UART1), Open-Drain, Digital
    // P1.3  -  RX1 (UART1), Open-Drain, Digital
    // P1.4  -  INT0 (Tmr0), Open-Drain, Digital
    // P1.5  -  INT1 (Tmr1), Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital
    // P3.7  -  Unassigned,  Open-Drain, Digital
//----------------------------------------------------

void Port_Init(void){
    //交叉译码器
     XBR0=0x07;
     XBR1=0x14;
     XBR2=0x46;
     
     P1MDIN=0xFF;
     P1MDOUT=0x87;
     P1 |=0x80;
    
     P2MDOUT=0xB7;
     P3MDOUT=0xBF;
     P3 |=0x40;   //P3.7 = 1
     //未使用引脚修改推挽方式
     P74OUT=0xFF;            
}

void ADDA_Init(void){
    
     REF0CN=0x07;//基准电压
     AMX0CF=0x00;//
     ADC0CF=0x60;
     ADC0CN=0xC0;
//     AMX0SL = AIN_Signal;
     DAC0CN=0x80;
     EIP2 |= 0x02;
     EIE2 |= 0x02;//开启ADC0中断
     DAC0L = 0;
     DAC0H = 0;
        
}
//----------------------------
//外部中断初始化
//-------------------------------
void INT_Init(void)
{
    IT1 = 1;   //下降沿
    PX1 = LOW;   //优先级低
    EX1 = ENABLE;   //允许
    //P3IF &= 0xFB;        //int6为下降沿
}

void Timer0_Init(unsigned int F_Set){
     
     TH0 = (unsigned char)((F_Set&0xFF00)>>8);
     TL0 = (unsigned char)(F_Set&0x00FF);  
     CKCON |= (0x01<<3);//system clock
     TMOD |= 0x01;      //16 bits
     PT0 = HIGH;        //priority
     ET0 = ENABLE;      //interrupt enable
     TR0 = ENABLE;      //timer start   
     
}

void Timer1_Init(void){
    
    TH1 = 0x0A;
    TL1 = 0x01;
    CKCON |= (0x01<<4);
    TMOD |= (0x01<<4);
    PT1 = HIGH;
    ET1 = ENABLE;
    TR1 = ENABLE;            
} 
//------------------------------
//定时器3在外部中断1中开启
//-----------------------------
void Timer3_Init(void){
//    TMR3 = 0xF767;
//    TMR3RL = 0xF767;
    EIP2 |= 0x01;
    TMR3 = (0xFFFF-(unsigned int)(250/(1/SYSCLK)));   //250us
    TMR3RL = (0xFFFF-(unsigned int)(250/(1/SYSCLK)));   //重载值      
}






