#include <c8051f020.h>
#include "global.h"
#include "lcd.h"
#include "uart.h"


unsigned char PreMinute = 0xFF;

//unsigned char SelectFlag = 0;
//unsigned char BackLightDelay = 0;
unsigned char idata temp[6];
//unsigned char idata table[4];

unsigned int idata RectanglePos[2];
unsigned int idata TrianglePos[2];
//unsigned int StarPos[2];       //*����
unsigned int idata FillPlacePos[4];   //�������

///��ʾͼƬ����
unsigned char code CMD7000[]={0xAA,0x70,0x00,0xCC,0x33,0xC3,0x3C,0xFE};
unsigned char code CMD7001[]={0xAA,0x70,0x01,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7002[]={0xAA,0x70,0x02,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7003[]={0xAA,0x70,0x03,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7004[]={0xAA,0x70,0x04,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7005[]={0xAA,0x70,0x05,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7006[]={0xAA,0x70,0x06,0xCC,0x33,0xC3,0x3C,0xFE}; 
//unsigned char code CMD7007[]={0xAA,0x70,0x07,0xCC,0x33,0xC3,0x3C,0xFE};


unsigned char code GB_TEMP[6] = {0xCE,0xC2,0xB6,0xC8,0x3A,0xFE};                           //���¶�:��
unsigned char code GB_SIGNAL[6] = {0xD0,0xC5,0xBA,0xC5,0x3A,0xFE};                         //"�ź�:"
unsigned char code GB_STATE[6] = {0xD7,0xB4,0xCC,0xAC,0x3A,0xFE};                          //"״̬:"

unsigned char code END_CMD[5] = {0xCC,0x33,0xC3,0x3C,0xFE};                             //֡β
unsigned int code CursorPos[2] = {0x011B,0x000C};             //���λ��

//----------------------------------------
/*��ʱms*/
//----------------------------------------
//void DelayMs(unsigned int t)
//{
//    unsigned char i,j;
//    unsigned char Y=11;
//    unsigned char X=200;
//    do 
//	{
//     j=Y;
//     do 
//		{
//		i=X;
//		do 
//		    {
//			_nop_();
//			_nop_();//n��NOP
//			}while(--i);
//		}while(--j);     
//    }while( --t );  
//}

/***֡β*/
void LCDComEnd(void)
{
    Uart1_Send_String(END_CMD);
}
/*����*/
void LCDClear(void)
{
    Uart1_Send_Byte(0xAA);
    Uart1_Send_Byte(0x52); 
    LCDComEnd();   
}
/*************�������*********************
******PWM_T : 0x00~0x3F    ***************/
//Ϊ������ͬ�ж��ظ����õľ��棬����������ͬ�ĺ���//
void T1LCDBackLight(unsigned char PWM_T)
{
    Uart1_Send_Byte(0xAA);
    Uart1_Send_Byte(0x5F);
    Uart1_Send_Byte(PWM_T);
    LCDComEnd();        
}

//void KEYLCDBackLight(unsigned char PWM_T)
//{
//    Uart1_Send_Byte(0xAA);
//    Uart1_Send_Byte(0x5F);
//    Uart1_Send_Byte(PWM_T);
//    LCDComEnd();        
//}
//-----------------------------------------------------------------------------------------
/******�ı���ʾ��ʽ:*******************************************************************
****** AA 98 <X> <Y> <LIB ID> <C MOD> <C DOT> <Fcolor> <Bcolor> <String> <end/> *******
***************************************************************************************/
//(unsigned int X,unsigned int Y,unsigned int Fcolor,unsigned int Bcolor,unsigned int Str)
//   ��ʾ������ˮ������⡱
//--------------------------------------------------------------------------------------
void LCDDisplayStringTitle(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = 0x00;    //X
    temp[3] = 0x2F;
    temp[4] = 0x00;     //Y
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x38;
    temp[1] = 0x23;
    temp[2] = 0x81;
    temp[3] = 0x03;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x1F;
    temp[1] = 0x82;
    temp[2] = 0x08;
    temp[3] = 0xD4;
    temp[4] = 0xDA;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xCF;
    temp[1] = 0xDF;
    temp[2] = 0xCB;
    temp[3] = 0xAE;
    temp[4] = 0xD6;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xD0;
    temp[1] = 0xC6;
    temp[2] = 0xF8;
    temp[3] = 0xBC;
    temp[4] = 0xE0;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xB2;
    temp[1] = 0xE2;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);
}

/****************��ʾPicID����ͼƬ***************
*************************************************/
#if 1
void LCDDisplayPic(unsigned char PicID)
{
    switch(PicID)
    {
        case 0:
            Uart1_Send_String(CMD7000);
            break;
        case 1:
            Uart1_Send_String(CMD7001);
            break;       
        default:
            Uart1_Send_String(CMD7001);
            break;                    
    }
    //DelayMs(100);
}
#endif

//��ʾ�ź�
void LCDDisplaySignalValue(void)
{
	temp[0] = 0xAA;
	temp[1] = 0x98;			//ѡ���ֿ�
	temp[2] = 0x00;			//X����
	temp[3] = 0x2F;
	temp[4] = 0x00;			//Y����
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = 0x86;
	temp[1] = 0x23;			//32����
	temp[2] = 0xC1;			//ǰ��ɫ����ɫ����ʾ �ı�������ʾ
	temp[3] = 0x03;			// 32*32	
	temp[4] = 0x40;
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = 0x00;
	temp[1] = 0x84;
	temp[2] = 0x18;
	temp[3] = 0xD0;		// �� �� GB2312��
	temp[4] = 0xC5;	
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = 0xBA;		// ��
	temp[1] = 0xC5;
	temp[2] = 0x3A;
	temp[3] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = Signal/1000 + 0x30;
	temp[1] = (Signal%1000)/100 + 0x30;
	temp[2] = (Signal%100)/10 + 0x30;
	temp[3] = (Signal%10) + 0x30;	
	temp[4] = 0xFE;
	Uart1_Send_String(temp);
	Uart1_Send_String(END_CMD);
}

//-------------------------------------------------
//
//�¶�
//-----------------------------------------------
void LCDDisplayTemperatureValue(void)
{
    temp[0] = 0xAA;
	temp[1] = 0x98;			//ѡ���ֿ�
	temp[2] = 0x00;			//X����
	temp[3] = 0x2F;
	temp[4] = 0x00;			//Y����
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = 0x5F;
	temp[1] = 0x23;			//32����
	temp[2] = 0xC1;			//ǰ��ɫ����ɫ����ʾ �ı�������ʾ
	temp[3] = 0x03;			// 32*32	
	temp[4] = 0x40;
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = 0x00;
	temp[1] = 0x84;
	temp[2] = 0x18;
	temp[3] = 0xFE; 
	Uart1_Send_String(temp);
	Uart1_Send_String(GB_TEMP);
	temp[0] =((int)(Temperature*100))/10000 + 0x30; 
	if(temp[0] == 0x30)
	{
	    temp[0] = (((int)(Temperature*100))%10000)/1000 +0x30; 
	    temp[1] = (((int)(Temperature*100))%1000)/100 + 0x30; 
	    temp[2] = 0x2E;
	    temp[3] = (((int)(Temperature*100))%100)/10 + 0x30;
	    temp[4] = 0xFE; 
	}
	else 
	{
    	temp[1] = (((int)(Temperature*100))%10000)/1000 +0x30;
    	temp[2] = (((int)(Temperature*100))%1000)/100 + 0x30;
    	temp[3] = 0x2E;
    	temp[4] = (((int)(Temperature*100))%100)/10 + 0x30;
    	temp[5] = 0xFE;
    }
	Uart1_Send_String(temp);
	temp[0] = (((int)(Temperature*100))%10) + 0x30;
	temp[1] = 0xA1;
	temp[2] = 0xE6;
	temp[3] = 0xFE;
	Uart1_Send_String(temp);	
	Uart1_Send_String(END_CMD);
}
//-------------------------------------------------
//��ʾ״̬
//���������������ر�
//------------------------------------------------
void LCDDisplayState(void)
{
    temp[0] = 0xAA;
	temp[1] = 0x98;			//ѡ���ֿ�
	temp[2] = 0x00;			//X����
	temp[3] = 0x2F;
	temp[4] = 0x00;			//Y����
	temp[5] = 0xFE; 
	Uart1_Send_String(temp);
	temp[0] = 0xAD;
	temp[1] = 0x23;			//32����
	temp[2] = 0xC1;			//ǰ��ɫ����ɫ����ʾ �ı�������ʾ
	temp[3] = 0x03;			// 32*32	
	temp[4] = 0x40;
	temp[5] = 0xFE;  
	Uart1_Send_String(temp);
    temp[0] = 0x00;
	temp[1] = 0x84;
	temp[2] = 0x18;
	temp[3] = 0xFE;
	Uart1_Send_String(temp);
	Uart1_Send_String(GB_STATE);          //��״̬����
	temp[0] = 0xB1;                       //����
	temp[1] = 0xA8;
	temp[2] = 0xBE;
	temp[3] = 0xAF;
	temp[4] = 0xFE;
	Uart1_Send_String(temp);
	if(bValveState == ON)    
	{
	    temp[0] = 0xBF;     //����
	    temp[1] = 0xAA;
	    temp[2] = 0xC6;
	    temp[3] = 0xF4;
	    temp[4] = 0xFE;
	    Uart1_Send_String(temp);
	    Uart1_Send_String(END_CMD);    
	}
	else
	{
	    temp[0] = 0xB9;         //�ر�
	    temp[1] = 0xD8;
	    temp[2] = 0xB1;
	    temp[3] = 0xD5;
	    temp[4] = 0xFE;
	    Uart1_Send_String(temp);
	    Uart1_Send_String(END_CMD);    
	} 
	    
}
//---------------------------------------------
//��ʾ��������
//-----------------------------------------------
void LCDDisplayWorkState(void)
{
    temp[0] = 0xAA;
	temp[1] = 0x98;			//ѡ���ֿ�
	temp[2] = 0x00;			//X����
	temp[3] = 0x7F;
	temp[4] = 0x00;			//Y����
	temp[5] = 0xFE; 
	Uart1_Send_String(temp);
	temp[0] = 0xCF;
	temp[1] = 0x23;			//32����
	temp[2] = 0xC1;			//ǰ��ɫ����ɫ����ʾ �ı�������ʾ
	temp[3] = 0x03;			// 32*32	
	temp[4] = 0x40;
	temp[5] = 0xFE;  
	Uart1_Send_String(temp);
    temp[0] = 0x00;
	temp[1] = 0x84;
	temp[2] = 0x18;
	temp[3] = 0xFE;
	Uart1_Send_String(temp); 
	if(bTSKG == OFF)
	{
	    temp[0] = 0xB9;     //��������
	    temp[1] = 0xA4;
	    temp[2] = 0xD7;
	    temp[3] = 0xF7;
	    temp[4] = 0xD5;
	    temp[5] = 0xFE; 
	    Uart1_Send_String(temp);
	    temp[0] = 0xFD;
	    temp[1] = 0xB3;
	    temp[2] = 0xA3;
	    temp[3] = 0xFE;
	    Uart1_Send_String(temp);
	    Uart1_Send_String(END_CMD);  
	}       
	else
	{
	    temp[0] = 0xD5;       //���ڵ���
	    temp[1] = 0xFD;
	    temp[2] = 0xD4;
	    temp[3] = 0xDA;
	    //temp[4] = 0xB5;
	    temp[4] = 0xB8;
	    temp[5] = 0xFE;
	    Uart1_Send_String(temp);
	    //temp[0] = 0xF7;
	    //temp[1] = 0xCA;
	    //temp[2] = 0xD4;
	    temp[0] = 0xFA;
	    temp[1] = 0xD7;
	    temp[2] = 0xD9;
	    temp[3] = 0xFE;
	    Uart1_Send_String(temp);
	    Uart1_Send_String(END_CMD);
	}
}

//-------------------------------------------------
//���õ�ǰ��ɫ��
// AA 40  <FC> <BC> CC 33 C3 3C 
//--------------------------------------------------
void SetMixColor(unsigned int FColor,unsigned int BColor)
{
    temp[0] = 0xAA;
    temp[1] = 0x40;
    temp[2] = (unsigned char)(FColor>>8);
    temp[3] = (unsigned char)(FColor&0xFF);
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)(BColor>>8);
    temp[1] = (unsigned char)(BColor&0xFF);
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);        
}

//----------------------------------------------------------
//
//��ʾ���š�*��
//----------------------------------------------------------
//void LCDDisplayStar(unsigned int Xpos,unsigned int Ypos)
//{
//    temp[0] = 0xAA;
//    temp[1] = 0x98;
//    temp[2] = (unsigned char)(Xpos>>8);
//    temp[3] = (unsigned char)(Xpos&0xFF);
//    temp[4] = (unsigned char)(Ypos>>8);
//    temp[5] = 0xFE;
//    Uart1_Send_String(temp);
//    temp[0] = (unsigned char)(Ypos&0xFF);
//    temp[1] = 0x22;
//    temp[2] = 0x81;
//    temp[3] = 0x02;
//    temp[4] = 0xF8;
//    temp[5] = 0xFE;
//    Uart1_Send_String(temp);
//    temp[0] = 0x00;
//    temp[1] = 0x84;
//    temp[2] = 0x18;
//    temp[3] = 0xA1;
//    temp[4] = 0xF8;
//    temp[5] = 0xFE;
//    Uart1_Send_String(temp);
//    Uart1_Send_String(END_CMD);    
//}

//-----------------------------------------------------
//
//��ʾ��
//-----------------------------------------------------
void LCDDisplayRectangle(unsigned int Xpos,unsigned int Ypos)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = (unsigned char)(Xpos>>8);
    temp[3] = (unsigned char)(Xpos&0xFF);
    temp[4] = (unsigned char)(Ypos>>8);
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)(Ypos&0xFF);
    temp[1] = 0x22;
    temp[2] = 0x81;
    temp[3] = 0x02;
    temp[4] = 0xF8;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;
    temp[1] = 0x84;
    temp[2] = 0x18;
    temp[3] = 0xA1;         //��
    temp[4] = 0xF4;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);          
}

//--------------------------------------------------
//
//��ʾ��
//---------------------------------------------------
void LCDDisplayTriangle(unsigned int Xpos,unsigned int Ypos)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = (unsigned char)(Xpos>>8);
    temp[3] = (unsigned char)(Xpos&0xFF);
    temp[4] = (unsigned char)(Ypos>>8);
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)(Ypos&0xFF);
    temp[1] = 0x22;
    temp[2] = 0x81;
    temp[3] = 0x02;
    temp[4] = 0xF8;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;
    temp[1] = 0x84;
    temp[2] = 0x18;
    temp[3] = 0xA1;   //��
    temp[4] = 0xF8;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);              
}

//-----------------------------------------------------
//flag =0:  ����     flag=1  :   ����
//��ʾ��Χ
//-----------------------------------------------------
void LCDDisplayLimit(bit flag,unsigned int value)
{        
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    if(flag == UpValue)                   //����
    {
        temp[0] = 0x00;    //x
        temp[1] = 0x2F;
        temp[2] = 0x00;    //y
        temp[3] = 0x00;
        temp[4] = 0xFE;  
        Uart1_Send_String(temp);
        temp[0] = 0xC9;            //��
        temp[1] = 0xCF;
        temp[2] = 0xFE;
        Uart1_Send_String(temp);
    } 
    else if(flag == DownValue)                         //����
    {
        temp[0] = 0x00;    //x
        temp[1] = 0x2F;
        temp[2] = 0x00;    //y
        temp[3] = 0x30;
        temp[4] = 0xFE;  
        Uart1_Send_String(temp);
        temp[0] = 0xCF;              //��
        temp[1] = 0xC2;
        temp[2] = 0xFE;
        Uart1_Send_String(temp);        
    } 
    temp[0] = 0xCF;             //��
    temp[1] = 0xDE;
    temp[2] = 0xA3;            //��
    temp[3] = 0xBA;    
    temp[4] = (unsigned char)((value/1000) + 0x30);
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)(((value%1000)/100) + 0x30);
    temp[1] = (unsigned char)(((value%100)/10) + 0x30);
    temp[2] = (unsigned char)((value%10) + 0x30);
    temp[3] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);
}
//--------------------------------------------------------
//��ʾ����λ��uiSignalZeroPosition
//------------------------------------------------------
void LCDDisplayZeroPosition(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;  
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;       //x
    temp[1] = 0x2F;
    temp[2] = 0x00;      //y
    temp[3] = 0x90;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC1;        //��λ
    temp[1] = 0xE3;
    temp[2] = 0xCE;
    temp[3] = 0xBB;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA3;    //��
    temp[1] = 0xBA;
    temp[2] = (unsigned char)((uiSignalZeroPosition/1000)+0x30);
    temp[3] = (unsigned char)(((uiSignalZeroPosition%1000)/100)+0x30);
    temp[4] = (unsigned char)(((uiSignalZeroPosition%100)/10)+0x30);
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)((uiSignalZeroPosition%10)+0x30);
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);
}
//----------------------------------------------------
//����
//-------------------------------------------------
void LCDDisplayRange(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;  
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;       //x
    temp[1] = 0x2F;
    temp[2] = 0x00;      //y
    temp[3] = 0x60;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC1;        //����
    temp[1] = 0xBF;
    temp[2] = 0xB3;
    temp[3] = 0xCC;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA3;    //��
    temp[1] = 0xBA;
    temp[2] = (unsigned char)((uiSignalRange/1000)+0x30);
    temp[3] = (unsigned char)(((uiSignalRange%1000)/100)+0x30);
    temp[4] = (unsigned char)(((uiSignalRange%100)/10)+0x30);
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)((uiSignalRange%10)+0x30);
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);    
}
//----------------------------------------------------
//�޸�ʱ��
//----------------------------------------------------
void LCDDisplayTimeChange(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;
    temp[1] = 0xCA;        //ʱ��
    temp[2] = 0xB1;
    temp[3] = 0xBC;
    temp[4] = 0xE4;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA3;
    temp[1] = 0xBA;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);          
}
//----------------------------------------------------
//��ʾ�����ԡ������ʱ��
void LCDDisplayPage2(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC0;
    temp[1] = 0xB5;     //����
    temp[2] = 0xF7;
    temp[3] = 0xCA;
    temp[4] = 0xD4;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA1;         //��
    temp[1] = 0xFA;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC3;      //����
    temp[1] = 0xDC;
    temp[2] = 0xC2;
    temp[3] = 0xEB;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA1;
    temp[1] = 0xFA;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xCA;     //ʱ��
    temp[1] = 0xB1;
    temp[2] = 0xBC;
    temp[3] = 0xE4;
    temp[4] = 0xFE;
    Uart1_Send_String(temp); 
    Uart1_Send_String(END_CMD);     
}

//--------------------------------------------------------------
//��ʾ���
//-------------------------------------------------------------
void LCDDisplayCursor(unsigned char CursorEN,unsigned int *Pos)
{
    static unsigned int Xpos = 0;
    static unsigned int Ypos = 0;
    Xpos = Pos[0];
    Ypos = Pos[1];
    temp[0] = 0xAA;
    temp[1] = 0x44;
    temp[2] = CursorEN;
    temp[3] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (unsigned char)(Xpos>>8);
    temp[1] = (unsigned char)(Xpos&0xFF);
    temp[2] = (unsigned char)(Ypos>>8);
    temp[3] = (unsigned char)(Ypos&0xFF);
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x05;
    temp[1] = 0x0D;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);    
}

//---------------------------------------------------
//-----------------------------------------------
//��ʾʱ��
//-------------------------------------------------
void LCDDisplayTime(void)
{    
    //if(PreMinute != Minute)
    {
        PreMinute = Minute;
        temp[0] = 0xAA;
        temp[1] = 0x98;
        temp[2] = 0x00;
        temp[3] = 0xFF;
        temp[4] = 0x00;
        temp[5] = 0xFE;
        Uart1_Send_String(temp);
        temp[0] = 0x03;
        temp[1] = 0x23;
        temp[2] = 0xC1;
        temp[3] = 0x02;
        temp[4] = 0xF8;
        temp[5] = 0xFE;
        Uart1_Send_String(temp);
        temp[0] = 0x00;
        temp[1] = 0x84;
        temp[2] = 0x18;
        temp[3] = 0xFE;
        Uart1_Send_String(temp);
        if((Hour == 0x3F) && (Minute == 0x7F))
        {
            temp[0] = 0x66;
            temp[1] = 0x61;
            temp[2] = 0x75;
            temp[3] = 0x6C;
            temp[4] = 0x74; 
            temp[5] = 0xFE;
            Uart1_Send_String(temp);        
            Uart1_Send_String(END_CMD);   
        }
        else
        {
            temp[0] = (Hour>>4) + 0x30;                 //Hour/10 + 0x30;
            temp[1] = (Hour&0x0F) + 0x30;                //Hour%10 + 0x30;
            temp[2] = 0x3A;
            temp[3] = (Minute>>4) + 0x30;                                //Minute/10 + 0x30;
            temp[4] = (Minute&0x0F) + 0x30;                             //Minute%10 + 0x30;
            temp[5] = 0xFE;
            Uart1_Send_String(temp);        
            Uart1_Send_String(END_CMD);
            LCDDisplayCursor(0x01,CursorPos);
        }                        
    }   
}


//---------------------------------------------------------
//��ʾ�²�
//
//---------------------------------------------------------
void LCDDisplayWENBU(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x60;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);  
    temp[0] = 0xCE;         //��
    temp[1] = 0xC2;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xB2;            //��
    temp[1] = 0xB9;
    temp[2] = 0xA3;            //��
    temp[3] = 0xBA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    //����²�����
    
    Uart1_Send_String(END_CMD);
}
//--------------------------------------------------------
//��ʾ����
//
//---------------------------------------------------------
void LCDDisplayDeviceNumber(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x90;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);  
    temp[0] = 0xBB;             //��
    temp[1] = 0xFA;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xBA;
    temp[1] = 0xC5;
    temp[2] = 0xA3;            //:
    temp[3] = 0xBA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    //��ӻ�������
    Uart1_Send_String(END_CMD);        
}
//-------------------------------------------
//��ʾ����״̬����  �� ��
//
//----------------------------------------------
void LCDDisplayDebug(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC0;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xB5;           //��
    temp[1] = 0xF7;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xCA;
    temp[1] = 0xD4;
    temp[2] = 0xA3;
    temp[3] = 0xBA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    if(FreqLargeRangeSearchONOFF == ON)
    {
        temp[0] = 0xBF;
        temp[1] = 0xAA;    
    }
    else
    {
        temp[0] = 0xB9;
        temp[1] = 0xD8;    
    }
    temp[3] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);
}

//-------------------------------------------------------
//
//��ʾ�������룺0000
//      
// �����������   
//-------------------------------------------------------
void LCDDisplayInputPassWord(void)
{    
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = 0x00;
    temp[3] = 0x00;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x60;
    temp[1] = 0x23;
    temp[2] = 0xC1;
    temp[3] = 0x03;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xF8;
    temp[1] = 0x00;
    temp[2] = 0x84;
    temp[3] = 0x18;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA1;    //��
    temp[1] = 0xF4; 
    temp[2] = 0xC3;   //��
    temp[3] = 0xDC;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC2;    //��
    temp[1] = 0xEB;
    temp[2] = 0xA3;    //��
    temp[3] = 0xBA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (InputPassWord1+0x30);   //0000
    temp[1] = (InputPassWord2+0x30);
    temp[2] = (InputPassWord3+0x30);
    temp[3] = (InputPassWord4+0x30);
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);
}
//-----------------------------------------
//�޸��������
//��ʾ�����룺0000
//----------------------------------------
void LCDDisplayPassWord(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x2F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x30;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC3;    //��
    temp[1] = 0xDC;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC2;     //��
    temp[1] = 0xEB;
    temp[2] = 0xA3;    //��
    temp[3] = 0xBA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (PassWordH>>4)+0x30;       //0000
    temp[1] = (PassWordH&0x0F)+0x30;
    temp[2] = (PassWordL>>4)+0x30;    
    temp[3] = (PassWordL&0x0F)+0x30;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);       
}
//--------------------------------------------
//��ʾ���������
//-----------------------------------------
void LCDDisplayPassWordWrong(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = 0x00;
    temp[3] = 0x00;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x7F;
    temp[1] = 0x23;
    temp[2] = 0xC1;
    temp[3] = 0x03;
    temp[4] = 0xF8;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x00;
    temp[1] = 0x84;
    temp[2] = 0x18;
    temp[3] = 0xC3;    //��
    temp[4] = 0xDC;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC2;     //��
    temp[1] = 0xEB;
    temp[2] = 0xB4;    //��
    temp[3] = 0xED;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xCE;       //��
    temp[1] = 0xF3;
//   temp[2] = 0x30;
//    temp[3] = 0x30;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);        
}
//--------------------------------------------
//TEL:020-32051659
//--------------------------------------------
void LCDDisplayTel(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = 0x00;
    temp[3] = 0x1F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xBF;
    temp[1] = 0x23;
    temp[2] = 0xC1;
    temp[3] = 0x03;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x40;
    temp[1] = 0x00;
    temp[2] = 0x84;
    temp[3] = 0x18;          //��ɫ������
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x54;      //T
    temp[1] = 0x45;     //E
    temp[2] = 0x4C;      //L
    temp[3] = 0x3A;      //:
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x30;      //0
    temp[1] = 0x32;      //2
    temp[2] = 0x30;      //0
    temp[3] = 0x2D;     //-
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x33;
    temp[1] = 0x32;
    temp[2] = 0x30;
    temp[3] = 0x35;
    temp[4] = 0x31;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x36;
    temp[1] = 0x35;
    temp[2] = 0x39;
    temp[3] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);       
}
//---------------------------------------
//����ϵ����˾
//-----------------------------------------
void LCDDisplayContect(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x98;
    temp[2] = 0x00;
    temp[3] = 0x0F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xA0;
    temp[1] = 0x23;
    temp[2] = 0xC1;
    temp[3] = 0x03;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xF8;
    temp[1] = 0x00;
    temp[2] = 0x84;
    temp[3] = 0x18;          //��ɫ������
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xC7;        //��
    temp[1] = 0xEB;
    temp[2] = 0xC1;       //��
    temp[3] = 0xAA;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xCF;        //ϵ
    temp[1] = 0xB5;
    temp[2] = 0xB1;       //��
    temp[3] = 0xBE;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0xB9;         //��        
    temp[1] = 0xAB;
    temp[2] = 0xCB;         //˾
    temp[3] = 0xBE;
    temp[4] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x21;          //��
    temp[1] = 0x20;
    temp[2] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);    
}
//----------------------------------------
//�Ա���ɫ���
//
//----------------------------------------
void FillRect(unsigned int * pRectPlace)
{
	//uchar tmp[15];
	unsigned int LeftTopX,LeftTopY,RightDownX,RightDownY;
	LeftTopX = pRectPlace[0];
	LeftTopY = pRectPlace[1];
	RightDownX = pRectPlace[2];
	RightDownY = pRectPlace[3];

	temp[0] = 0xaa;
	temp[1] = 0x5a;
	temp[2] = (unsigned char)(LeftTopX>>8);	//�ȷ��߰�λ
	temp[3] = (unsigned char)(LeftTopX & 0xff);	//�Ͱ�λ
	temp[4] = (unsigned char)(LeftTopY>>8);
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	temp[0] = (unsigned char)(LeftTopY & 0xff);
	temp[1] = (unsigned char)(RightDownX>>8);	//�ȷ��߰�λ
	temp[2] = (unsigned char)(RightDownX & 0xff);	//�Ͱ�λ
	temp[3] = (unsigned char)(RightDownY>>8);;
	temp[4] = (unsigned char)(RightDownY & 0xff);
	temp[5] = 0xFE;
	Uart1_Send_String(temp);
	Uart1_Send_String(END_CMD);
	
}
//--------------------------------------------
//��ʾDAC
//--------------------------------------------
#if 1
void LCDDisplayDAC(void)
{
    temp[0] = 0xAA;
    temp[1] = 0x55;
    temp[2] = 0x00;
    temp[3] = 0x5F;
    temp[4] = 0x00;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x10;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = 0x44;    //
    temp[1] = 0x41;
    temp[2] = 0x43;
    temp[3] = 0x3A;
    temp[4] = 0xFE;
    
    Uart1_Send_String(temp);
    temp[0] = (DAC0H/10)+0x30;       //0000
    temp[1] = (DAC0H%10)+0x30;
    temp[2] = 0x3A;
    temp[3] = (DAC0L/100)+0x30;
    temp[4] = (DAC0L%100)/10 +0x30;
    temp[5] = 0xFE;
    Uart1_Send_String(temp);
    temp[0] = (DAC0L%10) + 0x30;
    temp[1] = 0xFE;
    Uart1_Send_String(temp);
    Uart1_Send_String(END_CMD);                 
}
#endif
//-----------------------------------------------------------
//�¶�= V*80  ��(2.5/4096)*code*80=code*100/2048
//  80 = 200/2.5
//20160526���£��޸ĸ���������Ĵ��� 100��100.0
//----------------------------------------------------------
void TransformTemprature(void)
{
    Temperature = (TemperatureCode*100.0)/2048.0;       
}


//--------------------------------------------------
//LCDˢ�³���
//���ڣ�2016-5-18
//����KeyFlagȷ��ˢ�����ݻ��߲���
//--------------------------------------------------
void LCDRefresh(void)
{
    static unsigned char TimeCount = 0;
    if(KeyFlag == 0)
    {
        //StartKeyFlag = 0;                                               //��ʾ��������
        switch(DisplayIndex)
        {
            case  DisplayTitle:
                LCDDisplayStringTitle();               
                DisplayIndex = 0;
                break;
            case TempratureTransform:                
                TransformTemprature(); 
                LCDDisplayDAC();               
                DisplayIndex = 0;
                break;    
            case DisplayTemperatureValue:            
                LCDDisplayTemperatureValue();
                DisplayIndex = 0;
                break;
            case DisplaySignalValue:
                LCDDisplaySignalValue();
                DisplayIndex = 0;                
                break;
            case DisplayState:
                LCDDisplayState();
                DisplayIndex = 0;
                break;
            case DisplayWorkState:
                LCDDisplayWorkState();
                DisplayIndex = 0;
                break;
            case DisplayTime:
                LCDDisplayTime();
                DisplayIndex = 0;
                break;
            default:
                break;       
        }
            
    }
    else                                 //����������ʾ
    {
        switch(Activity)
        {
            case 0:
                return;
            case ActivityPassWord:
                //��ʾ��������
                LCDDisplayStringTitle();
                LCDDisplayInputPassWord();
                LCDDisplayTel();
                LCDDisplayContect();
                LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                FillRect(FillPlacePos);                
                break;
            case ActivityPassWordWrong:
                //�������
                LCDDisplayPassWordWrong();
                break;
            case ActivityParameter1:
                LCDDisplayRectangle(RectanglePos[0],RectanglePos[1]);
                //LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                LCDDisplayLimit(UpValue,uiHostUpValue);               
                LCDDisplayLimit(DownValue,uiHostDownValue);
                LCDDisplayWENBU();
                LCDDisplayDeviceNumber();
                LCDDisplayPage2();
                TimeCount++;
                if(TimeCount == 5)
                {
                    LCDDisplayTime();
                    TimeCount = 0;    
                }
                //LCDDisplayTime(); 
                //��������1
                break;
            case ActivityParameter2:
                LCDDisplayTimeChange();
                LCDDisplayPassWord();
                LCDDisplayZeroPosition();
                LCDDisplayRange();
                LCDDisplayDebug();
				TimeCount++;
				if(TimeCount == 5)
                {
					LCDDisplayTime();
					TimeCount = 0;    
                }
                //LCDDisplayTime();
                LCDDisplayRectangle(RectanglePos[0],RectanglePos[1]);
                //FillRect(FillPlacePos);
                //��������2
                break;
            default:
                break;     
        }    
    }        
}























