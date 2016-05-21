#include "key.h"
#include <intrins.h>

bit bChangeTime;
bit bChange;
bit KeyConFlag = 0;   //按键长按标志
bit KeyFlag = 0;     //0:没有按键    1:有按键

bit HostUpValueHunderdChangeFlag = 0;    //修改上限百位
bit HostDownValueHunderdChangeFlag = 0;  //修改下限百位
bit ZeroPositionHunderdChangeFlag = 0;  //修改零位百位
bit RangeHunderdChangeFlag = 0;       //修改量程百位
bit PassWordHHChangeFlag = 0;      //4位密码修改标志
bit PassWordHLChangeFlag = 0;
bit PassWordLHChangeFlag = 0;
bit PassWordLLChangeFlag = 0;
bit DebugStateChangeFlag = 0;            //是否修改了调试状态
 

unsigned char IRTimer = 0;       //记录IR脉冲宽度
unsigned char IRCount = 0;   //下降沿个数
unsigned int IRCode = 0;         
unsigned char KEY = 0;
unsigned char KEY1_Count = 0;    //KEY1按下次数
unsigned char KEY2_Count = 0;
unsigned char KEY3_Count = 0;
unsigned char KEY4_Count = 0;


unsigned char PassWordH;    //密码高两位   由InputPassWord1和InputPassWord2组成   BCD码保存
unsigned char PassWordL;    //密码低两位

unsigned char InputPassWord1;   //用户输入的4位密码 0~9
unsigned char InputPassWord2;
unsigned char InputPassWord3;
unsigned char InputPassWord4;
unsigned char idata KeyParameterTable[15];   //设定的各项参数

unsigned char Activity = 0;         //活动编号







//---------------------------------------------------
//用户使用按键设定的参数
//退出设定时保存参数
//------------------------------------------------
void SaveParameters(unsigned char *pTable)
{
    FlashPageErase(0x6A00,SELECT_64K);
    FlashWrite(0x6A00,pTable,15,SELECT_64K);
    WDog_Feed();
}


//----------------------------------------
//读按键原理：IR发射脉冲，每次按键有16位数据，8位自定义码+8位键值
//程序采用右移操作保存接收到的数据，故8位键值保存在高8位。
//由IR发送的两个下降沿之间的时间来确认为1或者0，若时间为1ms,则为0，
//若时间为2ms，则为1。
// T3定时250us，由INT1启动/关闭定时器
//----------------------------------------
void Timer3_ISR(void) interrupt 14     
{
    TMR3CN &= 0x7F;                  //TF3清零
    IRTimer++;
    if(IRTimer == 150)   //按键抬起,即完成一次按键
    {
        KEY = 0; 
        IRTimer = 0;
        IRCount = 0;   
        IRCode = 0;
        EIE2 &= 0xEE;       //关中断
        TMR3CN &= 0xFB;      //停止
        P17 = 1;                  //
    }        
}    

//---------------------------------------------------------------
//                             按键值
//---------------------------------------------------------------
//         Start    |                0x0247                  |      
//         KEY1     |   0x0047    |    KEY2    |   0x0847    |
//         KEY3     |   0x1047    |    KEY4    |   0x3247    |
//---------------------------------------------------------------
void INT1_ISR(void) interrupt 2
{
   
    unsigned char a = 0;
    IRCount++;
//----------------------------以上2016-4-18修改---------------------------
	if (IRCount == 1)
	{
	    if(IRTimer != 0)
	    {
	        KeyConFlag = 1;
	        IRTimer = 0;    
	    }
	    else KeyConFlag = 0;		
		IRCode = 0;
					
		EIE2 |= 0X01;		//允许定时器3产生中断
		TMR3CN = 0X06;
		EIE2 |= 0X10;		//允许外部中断6产生中断
		PX1 = HIGH;		//外部中断1中断级别为高(没有按键时级别为低) 
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
		else if (a >= 7 && a <= 9)//IR_Timer自减8次，即2ms，代表1
			IRCode |= 0x8000;
		else
			IRCount = 0;
		if (IRCount == 17)			//接收完了  必须是17 因为第1和第2个下降沿确定第1个数据  
		{						// 第16和17个下降沿确定第16个数据
			IRCount = 0;
			if(KeyConFlag == 1)
			{
		    return;    
			}
			else
			{
				switch (IRCode)
				{
					case 0x0047 :           //按键1	                            		   
						KEY = KEY1;		
						break;
					case 0x0247 :          //按键确认    			
        				KEY = KEY_Start;				
        				//KeyFlag = ~KeyFlag;						
        				break;
        			case 0x0847 :
        				//KEY2= 1;			//按键2
        				KEY = KEY2;
        				break;
        			case 0x1047 :
        				//KEY3= 1;			//按键3
        				KEY = KEY3;
        				break;
        			case 0x3247 :
        				//KEY4= 1;			//按键4
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
//按键服务程序
//每一个界面为一个activity,每个界面显示若干条参数
//按下结束按键时保存参数
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
        case KEY_Start:            //------开始/结束键
            KeyFlag = ~KeyFlag;	
            KEY = 0;
            NoneKeyCount = 0;
            KEY1_Count = 0;
            KEY2_Count = 0;
            KEY3_Count = 0;
            KEY4_Count = 0;
            if(KeyFlag == 0)           //结束
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
                {//保存参数,变量清零
                    bChange = 0;
                    KeyParameterTable[0] = (unsigned char)(uiHostUpValue>>8);
                    KeyParameterTable[1] = (unsigned char)(uiHostUpValue&0xFF);
                    KeyParameterTable[2] = (unsigned char)(uiHostDownValue>>8);
                    KeyParameterTable[3] = (unsigned char)(uiHostDownValue&0xFF);
                    KeyParameterTable[4] = 0;   //温补
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
            else                              //开始
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
        case KEY1:                              //上下选择键
            KEY = 0;
            NoneKeyCount = 0;
            KEY1_Count++;
            KEY4_Count = 0;    //回车键次数清零
            if(KeyFlag == 0) break;
            else
            {
                if(Activity == ActivityPassWord)             //密码界面
                {
                    if(KEY1_Count == 4) KEY1_Count = 0;
                    //修改密码输入▲位置
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
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))    //密码正确则显示参数界面1
                {
                    //选择参数项目 ◆位置
                    if(KEY1_Count == 9)  
                    {
                        KEY1_Count = 0;
                        Activity = ActivityParameter1;
                        LCDClear();
                        ZeroPositionHunderdChangeFlag = 0;                                                
                    }
                    switch(KEY1_Count)
                    {
                        case 0:                                     //上限
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x0000;                                                   
                            break;
                        case 1:                               //下限
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x002F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x0000;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0015; 
                            FillRect(FillPlacePos);
                            if(HostUpValueHunderdChangeFlag)          //清除上限修改标志
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x001C;
                                FillPlacePos[2] = 0x00AF;
                                FillPlacePos[3] = 0x002F;
                                FillRect(FillPlacePos);  
                                HostUpValueHunderdChangeFlag = 0;       
                            }
                            break;
                        case 2:                                   //温补
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x005F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x002F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0044;
                            FillRect(FillPlacePos);  
                            if(HostDownValueHunderdChangeFlag)             //清除下限修改标志
                            {
                                FillPlacePos[0] = 0x009B;
                                FillPlacePos[1] = 0x004C;
                                FillPlacePos[2] = 0x00B3;
                                FillPlacePos[3] = 0x005F;
                                FillRect(FillPlacePos); 
                                HostDownValueHunderdChangeFlag = 0;       
                            }                           
                            break;
                        case 3:                               //机号
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x008F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x005F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0074;
                            FillRect(FillPlacePos);                             
                            break;
                        case 4:                                     //调试 下一页，参数界面2
                            Activity = ActivityParameter2;
                            LCDClear();
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x00BF;                           
                            break;
                        case 5:                                    //时间
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x0000;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x002F;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x00FF;
                            FillRect(FillPlacePos);                             
                            break;
                        case 6:                                       //密码
                            RectanglePos[0] = 0x0000;
                            RectanglePos[1] = 0x002F;
                            
                            FillPlacePos[0] = 0x0000;
                            FillPlacePos[1] = 0x0000;
                            FillPlacePos[2] = 0x0015;
                            FillPlacePos[3] = 0x0015; 
                            FillRect(FillPlacePos);                            
                            break;
                        case 7:                                      //量程
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
                        case 8:                                       //零位
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
        case KEY2:                         //减1
            KEY = 0;
            NoneKeyCount = 0;
//            KEY2_Count++;
            if(KeyFlag == 0) break;
            else
            {
                bChange = 1;
                switch(KEY1_Count)
                {
                    case 0:                    //修改上限
					    if(HostUpValueHunderdChangeFlag == 0)						
					    {                                            //修改个位数                                
                            if(uiHostUpValue>0) uiHostUpValue--;
                            if(uiHostUpValue<uiHostDownValue) uiHostUpValue++;   //上限不能小于下限
                        }
                        else
                        {
                            if(uiHostUpValue>=100) uiHostUpValue = uiHostUpValue-100;
                            if(uiHostUpValue<uiHostDownValue)  uiHostUpValue = uiHostUpValue+100;               
                        }
                        break;
                    case 1:                               //修改下限
                        if(HostDownValueHunderdChangeFlag == 0)
                        {                    //修改个位数                                
                            if(uiHostDownValue>0)uiHostDownValue--;
                            //if(uiHostUpValue<uiHostDownValue) uiHostDownValue++;   //上限不能小于下限
                        }
                        else
                        {                        //修改百位
                            if(uiHostDownValue>=100)uiHostDownValue = uiHostDownValue-100;
                            //if(uiHostUpValue<uiHostDownValue)  uiHostDownValue = uiHostDownValue+100;  
                         }
                         break;
                     case 6:                                                   //修改密码
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
                     case 7:                           //量程
                        if(RangeHunderdChangeFlag ==0)
                        {
                             if(uiSignalRange>0) uiSignalRange--;   
                        }
                        else
                        {
                            if(uiSignalRange>=100) uiSignalRange = uiSignalRange-100;   
                        }
                        break;
                    case 8:                             //零位
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
                if(Activity == ActivityPassWord)                    //输入密码
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
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))      //修改参数
                {
                    bChange = 1;
                    switch(KEY1_Count)
                    {                        
                        case 0:                    //修改上限
                            if(HostUpValueHunderdChangeFlag == 0)
                            {                                   //修改个位数                                
                                uiHostUpValue++;
                                if(uiHostUpValue >= 4000) uiHostUpValue = 4000;                                    
                            }
                            else
                            {                     //修改百位
                                uiHostUpValue = uiHostUpValue+100;
                                if(uiHostUpValue >= 4000 ) uiHostUpValue = 4000;                                              
                            }
                            break;
                        case 1:                               //修改下限
                            if(HostDownValueHunderdChangeFlag == 0)
                            {                     //修改个位数                                
                                uiHostDownValue++;
                                if(uiHostDownValue > uiHostUpValue) uiHostDownValue = uiHostUpValue;   //上限不能小于下限
                            }
                            else
                            {                      //修改百位
                                uiHostDownValue = uiHostDownValue+100;
                                if(uiHostDownValue>uiHostUpValue)  uiHostDownValue = uiHostUpValue;  
                            }
                            break;
                        case 4:                                   //大范围调试开关
                            FreqLargeRangeSearchONOFF = ~FreqLargeRangeSearchONOFF;
                            DebugStateChangeFlag = ~DebugStateChangeFlag;                            
                            break;
                        case 7:                           //量程
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
                        case 8:                             //零位
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
                if(Activity == ActivityPassWord)   //密码界面
                {
                                                                        //判断密码是否正确  
                    KEY1_Count = 0;
                    KEY4_Count = 0;
                    if((((InputPassWord1<<4)+InputPassWord2) == PassWordH) &&  (((InputPassWord3<<4)+InputPassWord4) == PassWordL))
                    {
                        Activity = ActivityParameter1;        //密码正确，跳转到参数界面1
                        
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
                        Activity = ActivityPassWordWrong;     //显示密码错误界面 
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
                else if((Activity == ActivityParameter1) || (Activity == ActivityParameter2))   //参数界面
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
                        case 6:                       //修改密码
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
                        case 7:                         //量程
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
                        case 8:                           //零位
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

