#include "flash.h"


//-------------------------------------------------
//功能：写FLASH
//FlashAdd: 写入地址
//Data: 待写入数据
//len: 数据长度
//SFLE：是否使用附加的128字节  1  使用  0  不使用
//2016-03-30
//--------------------------------------------------
void FlashWrite(unsigned int FlashAdd,unsigned char *Data,unsigned char len,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char xdata *pwrite;
    EA = 0;         //关中断
    pwrite = (unsigned char xdata*)FlashAdd;
    FLSCL |= 0x01;  //
    PSCTL |= 0x01;  //允许写
    if(SFLE)
    {
        PSCTL |= 0x04;        
    }
    while(len>0)
    {
        len--;
        *(pwrite++) = *(Data++);                    
    }
    if(SFLE)
    {
        PSCTL &= ~0x04;    
    }
    FLSCL &= ~0x01;
    PSCTL &= ~0x01;
    EA = EA_Save;        
}

//-----------------------------------------------------
//功能：从FLASH读一个字节
//返回值：读到的字节
//------------------------------------------------------
#if 1
unsigned char FlashReadByte(unsigned int FlashAdd,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char code *pread;    
    unsigned char Byte;      //暂存字节数据
    EA = 0;
    pread = (unsigned char code*)FlashAdd;
    if(SFLE) PSCTL |= 0x04;
    Byte = *pread;
    if(SFLE) PSCTL &= ~0x04;
    EA = EA_Save;
    return Byte;         
}
#endif
//-------------------------------------------------------
//功能：擦除一个扇区
//
//-------------------------------------------------------

void FlashPageErase(unsigned int FlashAdd,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char xdata *pwrite;
    EA = 0;
    pwrite = (unsigned char xdata*)FlashAdd;
    FLSCL |= 0x01;  // 
    PSCTL |= 0x03;  //允许擦除，允许写
    if(SFLE) PSCTL |= 0x04;
    *pwrite = 0;
    if(SFLE) PSCTL &= ~0x04;
    FLSCL &= ~0x01;
    PSCTL &= ~0x03;
    EA = EA_Save;               
}