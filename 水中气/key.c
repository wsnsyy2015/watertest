#include "key.h"
#include <intrins.h>

bit bChangeTime;
bit bChange;
bit KeyConFlag = 0;   //����������־
bit KeyFlag = 0;     //0:û�а���    1:�а���

bit HostUpValueHunderdChangeFlag = 0;    //�޸����ް�λ
bit HostDownValueHunderdChangeFlag = 0;  //�޸����ް�λ
bit ZeroPositionHunderdChangeFlag = 0;  //�޸���λ��λ
bit RangeHunderdChangeFlag = 0;       //�޸����̰�λ
bit PassWordHHChangeFlag = 0;      //4λ�����޸ı�־
bit PassWordHLChangeFlag = 0;
bit PassWordLHChangeFlag = 0;
bit PassWordLLChangeFlag = 0;
bit DebugStateChangeFlag = 0;            //�Ƿ��޸��˵���״̬
 

unsigned char IRTimer = 0;       //��¼IR������
unsigned char IRCount = 0;   //�½��ظ���
unsigned int IRCode = 0;         
unsigned char KEY = 0;
unsigned char KEY1_Count = 0;    //KEY1���´���
unsigned char KEY2_Count = 0;
unsigned char KEY3_Count = 0;
unsigned char KEY4_Count = 0;


unsigned char PassWordH;    //�������λ   ��InputPassWord1��InputPassWord2���   BCD�뱣��
unsigned char PassWordL;    //�������λ

unsigned char InputPassWord1;   //�û������4λ���� 0~9
unsigned char InputPassWord2;
unsigned char InputPassWord3;
unsigned char InputPassWord4;
unsigned char idata KeyParameterTable[15];   //�趨�ĸ������

unsigned char Activity = 0;         //����







//---------------------------------------------------
//�û�ʹ�ð����趨�Ĳ���
//�˳��趨ʱ�������
//------------------------------------------------
void SaveParameters(unsigned char *pTable)
{
    FlashPageErase(0x6A00,SELECT_64K);
    FlashWrite(0x6A00,pTable,15,SELECT_64K);
    WDog_Feed();
}


//----------------------------------------
//������ԭ��IR�������壬ÿ�ΰ�����16λ���ݣ�8λ�Զ�����+8λ��ֵ
//����������Ʋ���������յ������ݣ���8λ��ֵ�����ڸ�8λ��
//��IR���͵������½���֮���ʱ����ȷ��Ϊ1����0����ʱ��Ϊ1ms,��Ϊ0��
//��ʱ��Ϊ2ms����Ϊ1��
// T3��ʱ250us����INT1����/�رն�ʱ��
//----------------------------------------
void Timer3_ISR(void) interrupt 14     
{
    TMR3CN &= 0x7F;                  //TF3����
    IRTimer++;
    if(IRTimer == 150)   //����̧��,�����һ�ΰ���
    {
        KEY = 0; 
        IRTimer = 0;
        IRCount = 0;   
        IRCode = 0;
        EIE2 &= 0xEE;       //���ж�
        TMR3CN &= 0xFB;      //ֹͣ
        P17 = 1;                  //
    }        
}    

//---------------------------------------------------------------
//                             ����ֵ
//---------------------------------------------------------------
//         Start    |                0x0247                  |      
//         KEY1     |   0x0047    |    KEY2    |   0x0847    |
//         KEY3     |   0x1047    |    KEY4    |   0x3247    |
//---------------------------------------------------------------
void INT1_ISR(void) interrupt 2
{
   
    unsigned char a = 0;
    IRCount++;
//----------------------------����2016-4-18�޸�---------------------------
	if (IRCount == 1)
	{
	    if(IRTimer != 0)
	    {
	        KeyConFlag = 1;
	        IRTimer = 0;    
	    }
	    else KeyConFlag = 0;		
		IRCode = 0;
					
		EIE2 |= 0X01;		//����ʱ��3�����ж�
		TMR3CN = 0X06;
		EIE2 |= 0X10;		//�����ⲿ�ж�6�����ж�
		PX1 = HIGH;		//�ⲿ�ж�1�жϼ���Ϊ��(û�а���ʱ����Ϊ��) 
		EX1 = 1;
		return ;			
	}
	else	
	{
		IRCode >>= 1;
		a = IRTimer;
		IRTimer = 0;	
		if (a >= 3 && a <=5)
			;
		else if (a >= 7 && a <= 9)//IR_Timer�Լ�8�Σ���2ms������1
			IRCode |= 0x8000;
		else
			IRCount = 0;
		if (IRCount == 17)			//��������  ������17 ��Ϊ��1�͵�2���½���ȷ����1������  
		{						// ��16��17���½���ȷ����16������
			IRCount = 0;
			if(KeyConFlag == 1)
			{
		    return;    
			}
			else
			{
				switch (IRCode)
				{
					case 0x0047 :           //����1	                            		   
						KEY = KEY1;		
						break;
					case 0x0247 :          //����ȷ��    			
        				KEY = KEY_Start;				
        				//KeyFlag = ~KeyFlag;						
        				break;
        			case 0x0847 :
        				//KEY2= 1;			//����2
        				KEY = KEY2;
        				break;
        			case 0x1047 :
        				//KEY3= 1;			//����3
        				KEY = KEY3;
        				break;
        			case 0x3247 :
        				//KEY4= 1;			//����4
        				KEY = KEY4;
        				break;
        			default :
        				break;
    		    }
            }
	    }
	}	
	EX1 = 1;
}
//------------------------------------------------------
//�����������
//ÿһ������Ϊһ��activity,ÿ��������ʾ����������
//���½�������ʱ�������
//----------------------------------------------------
#if 1
void KeyService(void)
{
    static unsigned char NoneKeyCount = 0;
    switch(KEY)       
    { 
        case 0:
            if(KeyFlag == 1)
            {
                NoneKeyCount++;
                if(NoneKeyCount == 80) 
                {
                    KeyFlag =  0;
                    NoneKeyCount = 0;  
                    LCDClear(); 
                    LCDDisplayPic(0x00);
                    Activity = 0;
                    PX1 = LOW;  
                    InputPassWord1 = 0;
                    InputPassWord2 = 0;
                    InputPassWord3 = 0;
                    InputPassWord4 = 0;  
                    PassWordHHChangeFlag = 0;
                    PassWordHLChangeFlag = 0;
                    PassWordLHChangeFlag = 0;
                    PassWordLLChangeFlag = 0; 
                    RectanglePos[0] = 0x0000;
                    RectanglePos[1] = 0x0000; 
                }
            }
            break;            
        case KEY_Start:            //------��ʼ/������
            KeyFlag = ~KeyFlag;	
            KEY = 0;
            NoneKeyCount = 0;
            KEY1_Count = 0;
            KEY2_Count = 0;
            KEY3_Count = 0;
            KEY4_Count = 0;
            if(KeyFlag == 0)           //����
            {               
                LCDClear();
                LCDDisplayPic(0x00);
                Activity = 0;
                PX1 = LOW;  
                InputPassWord1 = 0;
                InputPassWord2 = 0;
                InputPassWord3 = 0;
                InputPassWord4 = 0; 
                HostUpValueHunderdChangeFlag = 0;
                HostDownValueHunderdChangeFlag = 0;
                ZeroPositionHunderdChangeFlag = 0;
                RangeHunderdChangeFlag = 0;
                PassWordHHChangeFlag = 0;
                PassWordHLChangeFlag = 0;
                PassWordLHChangeFlag = 0;
                PassWordLLChangeFlag = 0; 
                RectanglePos[0] = 0x0000;
                RectanglePos[1] = 0x0000; 
                if(bChange)
                {//�������,��������
                    bChange = 0;
                    KeyParameterTable[0] = (unsigned char)(uiHostUpValue>>8);
                    KeyParameterTable[1] = (unsigned char)(uiHostUpValue&0xFF);
                    KeyParameterTable[2] = (unsigned char)(uiHostDownValue>>8);
                    KeyParameterTable[3] = (unsigned char)(uiHostDownValue&0xFF);
                    KeyParameterTable[4] = 0;   //�²�
                    KeyParameterTable[5] = 0;
                    KeyParameterTable[6] = (unsigned char)(FreqLargeRangeSearchONOFF&0x01);
                    KeyParameterTable[7] = (unsigned char)(F_FIND>>8);
                    KeyParameterTable[8] = (unsigned char)(F_FIND&0xFF);
                    KeyParameterTable[9] = PassWordH;
                    KeyParameterTable[10] = PassWordL;
                    KeyParameterTable[11] = (unsigned char)(uiSignalZeroPosition>>8);
                    KeyParameterTable[12] = (unsigned char)(uiSignalZeroPosition&0xFF);
                    KeyParameterTable[13] = (unsigned char)(uiSignalRange>>8);
                    KeyParameterTable[14] = (unsigned char)(uiSignalRange&0xFF);  
                    if((DebugStateChangeFlag == 1)&&(FreqLargeRangeSearchONOFF == ON))
                    {
                        bTSKG = ON;
                        bValveState = OFF;
                        bDewaterFlag = OFF;
                        bSzbd = OFF;
                        FreqSmallRangeSearchONOFF = OFF;
                        F_ADC = 0;
                        DAC0L = 0;
                        DAC0H = 0;
                        F_FIND = 0;
                        uiPWMCount = 0;
                        F_OUT = T0_Set_LargeRange_Start;
                        F_END = T0_Set_LargeRange_End;                        
                        //TH0 = (unsigned char)(F_OUT>>8);
                        //TL0 = (unsigned char)(F_OUT&0xFF);    
                    }                                      
                    SaveParameters(KeyParameterTable);    
                }                         
            }    
            else                              //��ʼ
            {
                Activity = ActivityPassWord;
                //WDog_Feed();
                //LCDClear(); 
                SetMixColor(0x4000,0x8418);
                //KEYLCDBackLight(0x2F);
                TrianglePos[0] = 0x007B;
                TrianglePos[1] = 0x007C;
                FillPlacePos[0] = 0x0000;
                FillPlacePos[1] = 0x005F;
                FillPlacePos[2] = 0x0140;
                FillPlacePos[3] = 0x00F0;
                FillRect(FillPlacePos);  
                FillPlacePos[0] = 0x009B;
                FillPlacePos[1] = 0x007C;
                FillPlacePos[2] = 0x00CB;
                FillPlacePos[3] = 0x009C;                      
            }
            break;

//-------------------------------------------------------------------------            
        case KEY1:                              //����ѡ���
            KEY = 0;
            NoneKeyCount = 0;
            KEY1_Count++;
            KEY4_Count = 0;    //�س�����������
            if(KeyFlag == 0) break;
            else
            {
                if(Activity == ActivityPassWord)             //�������
                {
                    if(KEY1_Count == 4) KEY1_Count = 0;
                    //�޸����������λ��
                    switch(KEY1_Count) 
                    {
                        case 0:
                            TrianglePos[0] = 0x007B;
                            TrianglePos[1] = 0x007C;
                            FillPlacePos[0] = 0x009B;
                            FillPlacePos[1] = 0x007C;
                            FillPlacePos[2] = 0x00CB;
                            FillPlacePos[3] = 0x009C;  
                            FillRect(FillPlacePos);                          
                            break;
                        case 1:
                            TrianglePos[0] = 0x008B;
                            TrianglePos[1] = 0x007C;
                            FillPlacePos[0] = 0x007B;
                            FillPlacePos[1] = 0x007C;
                            FillPlacePos[2] = 0x008D;
                            FillPlacePos[3] = 0x009C;
                            FillRect(FillPlacePos);
                            break;
                        case 2:
                            TrianglePos[0] = 0x009B;
                            TrianglePos[1] = 0x007C;
                            FillPlacePos[0] = 0x008B;
                            FillPlacePos[1] = 0x007C;
                            FillPlacePos[2] = 0x009D;
                            FillPlacePos[3] = 0x009C;
                            FillRect(FillPlacePos);
                            break;
                        case 3:
                            TrianglePos[0] = 0x00AB;
                            TrianglePos[1] = 0x007C;
                            FillPlacePos[0] = 0x009B;
                            FillPlacePos[1] = 0x007C;
                            FillPlacePos[2] = 0x00AD;
                            FillPlacePos[3] = 0x009C;
                            FillRect(FillPlacePos);
                            break;   
                        default:
                            break; 
                    }                           
                }
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))    //������ȷ����ʾ��������1
                {
                    //ѡ�������Ŀ ��λ��
                    if(KEY1_Count == 9)  
                    {
                        KEY1_Count = 0;
                        Activity = ActivityParameter1;
                        LCDClear();
                        ZeroPositionHunderdChangeFlag = 0;                                                
                    }
                    switch(KEY1_Count)
                    {
                        case 0:                                     //����
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x0000;                                                   
                            break;
                        case 1:                               //����
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x002F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x0000;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0015; 
                            FillRect(FillPlacePos);
                            if(HostUpValueHunderdChangeFlag)          //��������޸ı�־
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x001C;
                                FillPlacePos[2] = 0x00AF;
                                FillPlacePos[3] = 0x002F;
                                FillRect(FillPlacePos);  
                                HostUpValueHunderdChangeFlag = 0;       
                            }
                            break;
                        case 2:                                   //�²�
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x005F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x002F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0044;
                            FillRect(FillPlacePos);  
                            if(HostDownValueHunderdChangeFlag)             //��������޸ı�־
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x004C;
                                FillPlacePos[2] = 0x00B3;
                                FillPlacePos[3] = 0x005F;
                                FillRect(FillPlacePos); 
                                HostDownValueHunderdChangeFlag = 0;       
                            }                           
                            break;
                        case 3:                               //����
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x008F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x005F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0074;
                            FillRect(FillPlacePos);                             
                            break;
                        case 4:                                     //���� ��һҳ����������2
                            Activity = ActivityParameter2;
                            LCDClear();
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x00BF;                           
                            break;
                        case 5:                                    //ʱ��
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x0000;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x002F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x00FF;
                            FillRect(FillPlacePos);                             
                            break;
                        case 6:                                       //����
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x002F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x0000;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0015; 
                            FillRect(FillPlacePos);                            
                            break;
                        case 7:                                      //����
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x005F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x002F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0044; 
                            FillRect(FillPlacePos);  
                            PassWordHHChangeFlag = 0;
                            PassWordHLChangeFlag = 0;
                            PassWordLHChangeFlag = 0;
                            PassWordLLChangeFlag = 0; 
                            FillPlacePos[0] = 0x008B;
                            FillPlacePos[1] = 0x004C;
                            FillPlacePos[2] = 0x00CD;
                            FillPlacePos[3] = 0x005F;
                            FillRect(FillPlacePos);                        
                            break;
                        case 8:                                       //��λ
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x008F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x005F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0074;
                            FillRect(FillPlacePos); 
                            if(RangeHunderdChangeFlag)
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x007C;
                                FillPlacePos[2] = 0x00BC;
                                FillPlacePos[3] = 0x008F;
                                FillRect(FillPlacePos); 
                                RangeHunderdChangeFlag = 0;      
                            }                              
                            break;
                        default :
                               break;        
                    }       
                }                    
            }
            break;
//----------------------------------------------------------------------------            
        case KEY2:                         //��1
            KEY = 0;
            NoneKeyCount = 0;
//            KEY2_Count++;
            if(KeyFlag == 0) break;
            else
            {
                bChange = 1;
                switch(KEY1_Count)
                {
                    case 0:                    //�޸�����
					    if(HostUpValueHunderdChangeFlag == 0)						
					    {                                            //�޸ĸ�λ��                                
                            if(uiHostUpValue>0) uiHostUpValue--;
                            if(uiHostUpValue<uiHostDownValue) uiHostUpValue++;   //���޲���С������
                        }
                        else
                        {
                            if(uiHostUpValue>=100) uiHostUpValue = uiHostUpValue-100;
                            if(uiHostUpValue<uiHostDownValue)  uiHostUpValue = uiHostUpValue+100;               
                        }
                        break;
                    case 1:                               //�޸�����
                        if(HostDownValueHunderdChangeFlag == 0)
                        {                    //�޸ĸ�λ��                                
                            if(uiHostDownValue>0)uiHostDownValue--;
                            //if(uiHostUpValue<uiHostDownValue) uiHostDownValue++;   //���޲���С������
                        }
                        else
                        {                        //�޸İ�λ
                            if(uiHostDownValue>=100)uiHostDownValue = uiHostDownValue-100;
                            //if(uiHostUpValue<uiHostDownValue)  uiHostDownValue = uiHostDownValue+100;  
                         }
                         break;
                     case 6:                                                   //�޸�����
                        if(PassWordHHChangeFlag == 1)
                        {
                            if(PassWordH >= 16)
                            {
                                PassWordH = PassWordH-16;    
                            }
                            else PassWordH = PassWordH + 0x90;    
                        }
                        else if(PassWordHLChangeFlag == 1)
                        {
                            if((PassWordH&0x0F) != 0)
                                PassWordH--; 
                            else PassWordH += 0x09;       
                        }
                        else if(PassWordLHChangeFlag == 1)
                        {
                            if(PassWordL >= 16)
                            {
                                PassWordL = PassWordL - 16;        
                            } 
                            else PassWordL = PassWordL + 0x90;   
                        }
                        else if(PassWordLLChangeFlag == 1)
                        {
                            if((PassWordL&0x0F) != 0)                            
                                PassWordL--;                                   
                            else PassWordL += 0x09;
                        }
                        break;
                     case 7:                           //����
                        if(RangeHunderdChangeFlag ==0)
                        {
                             if(uiSignalRange>0) uiSignalRange--;   
                        }
                        else
                        {
                            if(uiSignalRange>=100) uiSignalRange = uiSignalRange-100;   
                        }
                        break;
                    case 8:                             //��λ
                        if(ZeroPositionHunderdChangeFlag == 0)
                        {
                            if(uiSignalZeroPosition > 0) uiSignalZeroPosition--;
                            //if(uiSignalZeroPosition < uiSignalRange ) uiSignalZeroPosition++;          
                        }
                        else 
                        {
                            if(uiSignalZeroPosition>=100) uiSignalZeroPosition = uiSignalZeroPosition-100;                                
                        }
                        break;
                    default :
                        break;                                                                  
                }                    
            }
            break;
//----------------------------------------------------------------            
        case KEY3:
            KEY = 0;
            NoneKeyCount = 0;
            KEY3_Count++;
            if(KeyFlag == 0) break;
            else
            {
                if(Activity == ActivityPassWord)                    //��������
                {
                    switch(KEY1_Count)
                    {
                        case 0:
                            InputPassWord1++;
                            if(InputPassWord1 == 10) InputPassWord1 = 0;
                            break;
                        case 1:
                            InputPassWord2++;
                            if(InputPassWord2 == 10) InputPassWord2 = 0;
                            break;
                        case 2:
                            InputPassWord3++;
                            if(InputPassWord3 == 10) InputPassWord3 = 0;
                            break;
                        case 3:
                            InputPassWord4++;
                            if(InputPassWord4 == 10) InputPassWord4 = 0;
                            break;
                        default:
                            break;            
                    }                    
                } 
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))      //�޸Ĳ���
                {
                    bChange = 1;
                    switch(KEY1_Count)
                    {                        
                        case 0:                    //�޸�����
                            if(HostUpValueHunderdChangeFlag == 0)
                            {                                   //�޸ĸ�λ��                                
                                uiHostUpValue++;
                                if(uiHostUpValue >= 4000) uiHostUpValue = 4000;                                    
                            }
                            else
                            {                     //�޸İ�λ
                                uiHostUpValue = uiHostUpValue+100;
                                if(uiHostUpValue >= 4000 ) uiHostUpValue = 4000;                                              
                            }
                            break;
                        case 1:                               //�޸�����
                            if(HostDownValueHunderdChangeFlag == 0)
                            {                     //�޸ĸ�λ��                                
                                uiHostDownValue++;
                                if(uiHostDownValue > uiHostUpValue) uiHostDownValue = uiHostUpValue;   //���޲���С������
                            }
                            else
                            {                      //�޸İ�λ
                                uiHostDownValue = uiHostDownValue+100;
                                if(uiHostDownValue>uiHostUpValue)  uiHostDownValue = uiHostUpValue;  
                            }
                            break;
                        case 4:                                   //��Χ���Կ���
                            FreqLargeRangeSearchONOFF = ~FreqLargeRangeSearchONOFF;
                            DebugStateChangeFlag = ~DebugStateChangeFlag;                            
                            break;
                        case 7:                           //����
                            if(RangeHunderdChangeFlag ==0)
                            {
                                uiSignalRange++;
                                if(uiSignalRange >=4000) uiSignalRange = 4000;       
                            }
                            else
                            {
                                uiSignalRange = uiSignalRange+100;
                                if(uiSignalRange>=4000) uiSignalRange=4000;    
                            }
                            break;
                        case 8:                             //��λ
                            if(ZeroPositionHunderdChangeFlag == 0)
                            {
                                uiSignalZeroPosition++;
                                if(uiSignalZeroPosition>uiSignalRange) uiSignalZeroPosition = uiSignalRange;     
                            }
                            else
                            {
                                uiSignalZeroPosition +=100;
                                if(uiSignalZeroPosition>uiSignalRange) uiSignalZeroPosition = uiSignalRange;       
                            }
                            break;
                        default :
                            break;                                                                                                    
                    }               
                }
            }
            break;
//--------------------------------------------------------------------            
        case KEY4:
            KEY = 0;
            NoneKeyCount = 0;
            KEY4_Count++;
            if(KeyFlag == 0) break;
            else
            {
                if(Activity == ActivityPassWord)   //�������
                {
                                                                        //�ж������Ƿ���ȷ  
                    KEY1_Count = 0;
                    KEY4_Count = 0;
                    if((((InputPassWord1<<4)+InputPassWord2) == PassWordH) &&  (((InputPassWord3<<4)+InputPassWord4) == PassWordL))
                    {
                        Activity = ActivityParameter1;        //������ȷ����ת����������1
                        
                        LCDClear();
                        InputPassWord1 = 0;
                        InputPassWord2 = 0;
                        InputPassWord3 = 0;
                        InputPassWord4 = 0;
                        TrianglePos[0] = 0x00BB;
                        TrianglePos[1] = 0x001C;
                        return;
                    }
                    else 
                    {
                        Activity = ActivityPassWordWrong;     //��ʾ���������� 
                        LCDClear();
                        InputPassWord1 = 0;
                        InputPassWord2 = 0;
                        InputPassWord3 = 0;
                        InputPassWord4 = 0;
                        return; 
                    }
                }
                else if(Activity == ActivityPassWordWrong)
                {
                    LCDClear();
                    Activity = ActivityPassWord;
                    return;        
                } 
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))   //��������
                {
                    switch(KEY1_Count)
                    {
                        case 0:
                            HostUpValueHunderdChangeFlag = ~HostUpValueHunderdChangeFlag;
                            if(HostUpValueHunderdChangeFlag == 1)
                            {
                                TrianglePos[0] = 0x009B;
                                TrianglePos[1] = 0x001C;
                                LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                            }  
                            else  
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x001C;
                                FillPlacePos[2] = 0x00AF;
                                FillPlacePos[3] = 0x002F;
                                FillRect(FillPlacePos);                                    
                            }                                                     
                            break;
                        case 1:
                            HostDownValueHunderdChangeFlag = ~HostDownValueHunderdChangeFlag;
                            if(HostDownValueHunderdChangeFlag == 1)
                            {
                                TrianglePos[0] = 0x009B;
                                TrianglePos[1] = 0x004C;
                                LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                            }  
                            else
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x004C;
                                FillPlacePos[2] = 0x00B3;
                                FillPlacePos[3] = 0x005F;
                                FillRect(FillPlacePos);       
                            }                          
                            break;
                        case 6:                       //�޸�����
                            if(KEY4_Count == 5) KEY4_Count = 1;
                            switch(KEY4_Count) 
                            {
                                case 1:
                                    PassWordHHChangeFlag = 1;
                                    PassWordHLChangeFlag = 0;
                                    PassWordLHChangeFlag = 0;
                                    PassWordLLChangeFlag = 0;
                                    TrianglePos[0] = 0x008B;
                                    TrianglePos[1] = 0x004C;
                                    LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                                    FillPlacePos[0] = 0x00AB;
                                    FillPlacePos[1] = 0x004C;
                                    FillPlacePos[2] = 0x00CD;
                                    FillPlacePos[3] = 0x005F;
                                    FillRect(FillPlacePos);
                                    break;
                                case 2:
                                    PassWordHHChangeFlag = 0;
                                    PassWordHLChangeFlag = 1;
                                    PassWordLHChangeFlag = 0;
                                    PassWordLLChangeFlag = 0;
                                    TrianglePos[0] = 0x009B;
                                    TrianglePos[1] = 0x004C;
                                    LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                                    FillPlacePos[0] = 0x008B;
                                    FillPlacePos[1] = 0x004C;
                                    FillPlacePos[2] = 0x009D;
                                    FillPlacePos[3] = 0x005F;
                                    FillRect(FillPlacePos);
                                    break;
                                case 3:
                                    PassWordHHChangeFlag = 0;
                                    PassWordHLChangeFlag = 0;
                                    PassWordLHChangeFlag = 1;
                                    PassWordLLChangeFlag = 0;
                                    TrianglePos[0] = 0x00AB;
                                    TrianglePos[1] = 0x004C;
                                    LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                                    FillPlacePos[0] = 0x009B;
                                    FillPlacePos[1] = 0x004C;
                                    FillPlacePos[2] = 0x00AD;
                                    FillPlacePos[3] = 0x005F;
                                    FillRect(FillPlacePos);
                                    break;
                                case 4:
                                    PassWordHHChangeFlag = 0;
                                    PassWordHLChangeFlag = 0;
                                    PassWordLHChangeFlag = 0;
                                    PassWordLLChangeFlag = 1;
                                    TrianglePos[0] = 0x00BB;
                                    TrianglePos[1] = 0x004C;
                                    LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);
                                    FillPlacePos[0] = 0x00AB;
                                    FillPlacePos[1] = 0x004C;
                                    FillPlacePos[2] = 0x00BD;
                                    FillPlacePos[3] = 0x005F;
                                    FillRect(FillPlacePos);
                                    break;
                                default:
                                    break;        
                            }  
                            break;
                        case 7:                         //����
                            RangeHunderdChangeFlag = ~RangeHunderdChangeFlag;
                            if(RangeHunderdChangeFlag == 1)
                            {
                                TrianglePos[0] = 0x009B;
                                TrianglePos[1] = 0x007C;
                                LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);       
                            }
                            else 
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x007C;
                                FillPlacePos[2] = 0x00BC;
                                FillPlacePos[3] = 0x008F;
                                FillRect(FillPlacePos);        
                            }
                            break;
                        case 8:                           //��λ
                            ZeroPositionHunderdChangeFlag = ~ZeroPositionHunderdChangeFlag;
                            if(ZeroPositionHunderdChangeFlag == 1)
                            {
                                TrianglePos[0] = 0x009B;
                                TrianglePos[1] = 0x00AC;
                                LCDDisplayTriangle(TrianglePos[0],TrianglePos[1]);      
                            }
                            else
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x00AC;
                                FillPlacePos[2] = 0x00BC;
                                FillPlacePos[3] = 0x00BF;
                                FillRect(FillPlacePos);                                    
                            }
                            break;  
                        default:
                            break;                               
                    }                
                }                
            }
            break;            
        default:
            break;                             
    }    
}

#endif

