#include "flash.h"


//-------------------------------------------------
//���ܣ�дFLASH
//FlashAdd: д���ַ
//Data: ��д������
//len: ���ݳ���
//SFLE���Ƿ�ʹ�ø��ӵ�128�ֽ�  1  ʹ��  0  ��ʹ��
//2016-03-30
//--------------------------------------------------
void FlashWrite(unsigned int FlashAdd,unsigned char *Data,unsigned char len,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char xdata *pwrite;
    EA = 0;         //���ж�
    pwrite = (unsigned char xdata*)FlashAdd;
    FLSCL |= 0x01;  //
    PSCTL |= 0x01;  //����д
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
//���ܣ���FLASH��һ���ֽ�
//����ֵ���������ֽ�
//------------------------------------------------------
#if 1
unsigned char FlashReadByte(unsigned int FlashAdd,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char code *pread;    
    unsigned char Byte;      //�ݴ��ֽ�����
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
//���ܣ�����һ������
//
//-------------------------------------------------------

void FlashPageErase(unsigned int FlashAdd,bit SFLE)
{
    bit EA_Save = EA;
    unsigned char xdata *pwrite;
    EA = 0;
    pwrite = (unsigned char xdata*)FlashAdd;
    FLSCL |= 0x01;  // 
    PSCTL |= 0x03;  //�������������д
    if(SFLE) PSCTL |= 0x04;
    *pwrite = 0;
    if(SFLE) PSCTL &= ~0x04;
    FLSCL &= ~0x01;
    PSCTL &= ~0x03;
    EA = EA_Save;               
}