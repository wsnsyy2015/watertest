#include "uart.h"
#include "global.h"


void UART0_Init(unsigned int Baudrate){
    
    SCON0 = 0x70; //��ʽ1
    PCON = 0x00;   
    T2CON = 0x34;//T2��UART0�����ʷ�����,TR2=1 
    T2 = Baudrate;
    RCAP2 = Baudrate;
//    RCAP2H = 0xFF;  //9600
//    RCAP2L = 0xB8;    
    P24 = 0;   
} 

#if 1
void UART1_Init(unsigned int Baudrate)
{
    
    SCON1 = 0xF0; //��ʽ3
    PCON = 0x00;
    T4CON = 0x34;
    T4 = Baudrate;   //0xFFB8
    RCAP4 = Baudrate;
//    RCAP4H = 0xFF;
//    RCAP4L = 0xB8;    
    P25 = 0;        
}
#endif


void Uart1_Send_Byte(unsigned char ch)
{
    P25 = HIGH; //����ʹ��
    SCON1 |= 0x08;
    SBUF1 = ch;
    WDog_Feed();
    while((SCON1&0x02) == 0);
    WDog_Feed();
    SCON1 &= 0xFD;  
    P25 = LOW;             
}

/*** ch:�ַ���        ****************************************
**** 0xFE:���崮���������ַ���0xFE��β************************
**************************************************************/
void Uart1_Send_String(unsigned char *ch)
{
    for(;*ch != 0xFE;ch++)
    {
        Uart1_Send_Byte(*ch);    
    }    
}


//void Uart1_Interrupt(void) interrupt 4
//{
  
//}