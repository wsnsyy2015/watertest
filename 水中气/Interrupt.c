#include "global.h"
//#include "interrupt.h"
//#include "lcd.h"

unsigned char idata ucCountForINT = 0;   //进入中断次数
unsigned int idata uiPWMCount;  //PWM次数
unsigned long idata F_ADC;
unsigned int Signal = 0;                       //信号值
unsigned long idata TemperatureTemp = 0;  //ADC累加值
unsigned int TemperatureCode = 0;  //ADC数据
float Temperature = 0; //温度值
unsigned int data F_FIND;
unsigned int data F_OUT;
unsigned int data F_START;
unsigned int data F_END;
unsigned int idata uiHostUpValue = 0;            //上限
unsigned int idata uiHostDownValue = 0;         //下限
unsigned int idata uiSignalZeroPosition = 0;      //信号值零位
unsigned int idata uiSignalRange = 0;             //信号值量程

unsigned char DisplayIndex = 0;
unsigned char MarkTime = 0;
unsigned char ucTempCount = 0;        //温度ADC次数
unsigned char ucSignalCount = 0;
unsigned char ucNoDebugTime = 0;
unsigned char ucControlValveTime = 0;    //延时控制阀
unsigned char ucOpenValveTime = 0;//开阀时间
unsigned char ucPreOpenValveTime = 0;//前一次开阀时间
unsigned int idata uiTemperatureCodeOpenValve = 0xFFFF;   //开阀温度ADC
unsigned int idata uiTemperatureCodeCloseValve;          //关阀温度ADC

bit bTSKG;
bit bBeng;
bit bSzbd;
bit bWash = 0;
bit bValveState = OFF;   //阀门状态
bit bSlaveValveState = ON;
bit bDewaterFlag = 0;   //是否在脱水
bit bStart = 0;
bit ControlValveFlag = 0;
bit bCloseValveTempFall = OFF;//温度下降导致关阀
bit FreqLargeRangeSearchONOFF = ON;//大范围搜索
bit FreqSmallRangeSearchONOFF = OFF;//小范围搜索

//bit SignalChangeFlag = 0;

#if 1

void MarkArray(void);
void ControlValve(void);

//-------------------------------------------
/*****************ADC0**********************
*取振幅最大时的频率值F_FIND
*非搜频阶段，则取平均值显示
********************************************/
//-------------------------------------------
void ADC0_ISR (void) interrupt 15
{

    AD0INT = DISABLE;
    if(AMX0SL == AIN_Signal)   //ADC采集信号值
    {
        if(bTSKG == ON)
        {
            if(ADC0>F_ADC)
            {
                F_ADC = ADC0;         //找最大值
                F_FIND = F_OUT;
                //SignalChangeFlag = 1;
                //Signal = ADC0;   //显示最大值
            }
            Signal = ADC0;   //实时值
        }
        else if(ucSignalCount == 0)   //非调试阶段
        {
            ucSignalCount++;
            AD0BUSY =1;
            return;   
        }
        else
        {
            ucSignalCount++; 
            if(ucSignalCount == 128)
            {
                ucSignalCount = 0;
                Signal = (F_ADC>>7);
                F_ADC = 0;
                //SignalChangeFlag = 1;
                return;       
            }
            F_ADC += ADC0;
            AD0BUSY = 1;               
        }
    }
    else if(AMX0SL == AIN_Temperature)   //采集温度值
    {
        if(ucTempCount == 0)
        {
            ucTempCount++;
            AD0BUSY = 1;  
            return;  
        }
        else
        {
            ucTempCount++;
            if(ucTempCount == 128)
            {
                ucTempCount = 0;
                TemperatureCode = (TemperatureTemp>>7); 
                TemperatureTemp = 0; 
                return;      
            }
            TemperatureTemp += ADC0;
            AD0BUSY = 1;                            
        }    
    }
     
}
//-------------------------------------------------
/*大范围调试阶段，发送200次翻转*********************/
/*精确调试阶段，发送800次翻转***********************/
// 精确调试完成后标定数据
//-------------------------------------------------
void Timer0_ISR(void) interrupt 1
{
    P35 = ~P35;
    if(bTSKG == ON)
    {   
        TH0 = (unsigned char)(F_OUT>>8);
        TL0 = (unsigned char)(F_OUT&0xFF);
    }
    else
    {
        TH0 = (unsigned char)(F_FIND>>8);
        TL0 = (unsigned char)(F_FIND&0xFF);    
    }
    ucCountForINT++;
    if(ucCountForINT<2)
    {
        if(bBeng) P20 = ~P20;
        return;    
    }
    ucCountForINT = 0;
    if(FreqLargeRangeSearchONOFF == ON)
    {
        if(FreqSmallRangeSearchONOFF == ON)
        {
            switch(uiPWMCount)
            {
                case 402:
                    AMX0SL = AIN_Temperature;
                    break;
                case 404:
                    ucTempCount = 0;
                    AD0BUSY = 1;
                    break;
                case 602:
                    AMX0SL = AIN_Signal;
                    break;
                case 604:
                    ucSignalCount = 0;
                    AD0BUSY =1;
                    break;
                default:
                    break;    
            }
        }
        else 
        {
            switch(uiPWMCount)
            {
                case 2:
                    AMX0SL = AIN_Temperature;
                    break;
                case 4:
                    ucTempCount = 0;
                    AD0BUSY = 1;
                    break;
                case 102:
                    AMX0SL = AIN_Signal;
                    break;
                case 104:
                    ucSignalCount = 0;
                    AD0BUSY = 1;
                    break;
                default :
                    break;    
            }   
        }
    }
    else 
    {
        switch(uiPWMCount)
        {
            case 402:
                AMX0SL = AIN_Temperature;
                break;
            case 404:
                ucTempCount = 0;
                AD0BUSY = 1;
                break;
            case 602:
                AMX0SL = AIN_Signal;
                break;
            case 604:
                ucSignalCount = 0;
                AD0BUSY =1;
                break;
            default:
                break;          
        }    
    }
////////////2016-03-31
                        
    uiPWMCount++;            
    if(FreqLargeRangeSearchONOFF == ON)
    {
        if(FreqSmallRangeSearchONOFF == OFF)
        {
            if(uiPWMCount <200)
            {
                if(bBeng) P20 = ~P20;
                return;    
            }    
        }
        else if(uiPWMCount<800)
        {
            if(bBeng) P20 = ~P20;
            return;        
        }        
    }
    else if(uiPWMCount<800)
    {
        if(bBeng) P20 = ~P20;
        return;        
    }                               
    uiPWMCount = 0;
    if(bTSKG == OFF) 
    {
        if(bBeng) P20 = ~P20;
        return;            
    }
    else 
        F_OUT = F_OUT+FreqStep;
    if(FreqLargeRangeSearchONOFF == ON)
    {
        if(FreqSmallRangeSearchONOFF == OFF)
        {
            F_OUT = F_OUT+0x05;                     //大范围调试步进
            if(F_OUT>F_END)
            {
                F_OUT = F_FIND-PreciseFreqRange;
                F_END = F_FIND+PreciseFreqRange;
                FreqSmallRangeSearchONOFF = ON;  //精确调试开
                if(bBeng) P20 = ~P20;
                return;   
            }
            if(bBeng) P20 = ~P20;
            return;    
        }                              //FreqSmallRangeSearchONOFF == ON
        else if(F_OUT>F_END)                      //精确调试结束
        {
            F_START = F_FIND - SmallFreqRange;
            MarkArray();//标定数据
            if(bBeng) P20 = ~P20; 
            return;  
        }
        if(bBeng) P20 = ~P20;
        return;                                
    }                                 //FreqLargeRangeSearchONOFF == OFF
    else if(bTSKG == OFF)
    {
        if(bBeng) P20 = ~P20;
        return;                                    
    }
    else if(F_OUT>F_END)
    {
        bTSKG = OFF;
        MarkTime++;
        if(bSzbd == ON)
        {
            //添加降温调试功能
            if(bBeng) P20 = ~P20;
            return;    
        }
        else 
        {
            if(MarkTime<FXCS)
            {
                if(bBeng) P20 = ~P20;
                return;        
            }
            MarkTime = 0;
            
            bTSKG = ON;
            FreqLargeRangeSearchONOFF = ON;
            //WASH = ON;
            F_OUT = F_FIND-WashFreqRange;
            F_END = F_FIND+WashFreqRange;
            if(bBeng) P20 = ~P20;
            return;   
        }    
    }    
    if(bBeng) P20 = ~P20;
    return;                 
}                       

/**********************************************
******************时序输出*********************
******************3ms刷新一次*****************
**********************************************/

void Timer1_ISR(void) interrupt 3
{
    static unsigned char BackLightFlag = 0;
    static unsigned char T1_Count1 = 0;
//    static unsigned char T1_Count2 = 0;
//    EX1 = DISABLE;          //外部中断
    TH1 = 0x0A;
    TL1 = 0x00;
//    WDog_Feed();
    T1_Count1++;
    if((BackLightFlag == 0) && (T1_Count1 == 8)||(BackLightFlag == 0) && (T1_Count1 == 20))
    {
        if(T1_Count1 == 20)
        {
            BackLightFlag = 1; 
            //SetMixColor(0x4000,0x8418);   
        }
        T1LCDBackLight(0x3F);                  
    }

    switch(T1_Count1)
    {
        case 20:
            DisplayIndex = DisplayTitle;                 //显示标题
            break;
        case 40:
            DisplayIndex = TempratureTransform;
            //LCDDisplayStringItem(GB_TEMP,0x002F,0x005F);          //温度
            break;
        case 60:
            DisplayIndex = DisplayTemperatureValue;
            break;
        case 80:
            DisplayIndex = DisplaySignalValue;
            //LCDDisplayStringItem(GB_SIGNAL,0x002F,0x007F);          //信号值
            //LCDDisplaySignalValue();           
            break;
        case 100:
            DisplayIndex = DisplayState;
            break;
        case 120:
            DisplayIndex = DisplayWorkState;
            //LCDDisplayStringItem(GB_STATE,0x002F,0x009F);           //状态
            break;
        case 140:
            //显示时间
            DisplayIndex = DisplayTime;            
            break;
        case 180:
            //动画
            break;
        case 200:
            if((bSzbd == OFF)&&(bTSKG == OFF))              //大范围调试后bSzbd=ON,需要重启后才为OFF
            {
                ucControlValveTime++;
                if(bValveState == OFF)
                {
                    //-----------------2016-5-24修改------------------------
                    if((ControlValveFlag == 0) && (ucControlValveTime == 9))
                    {
                        ucControlValveTime = 0;  
                        WDog_Feed();
                        ControlValveFlag = 1;
                        ControlValve();//控制阀
                    }
                }
                else 
                {
                    if((ControlValveFlag == 0) && (ucControlValveTime == 100))
                    {
                        ucControlValveTime = 0;  
                        WDog_Feed();
                        ControlValveFlag = 1;
                        ControlValve();//控制阀   
                    }   
                }
                //-----------------------end---------------------------------
             }
		    break;
		case 220:
		    if((FreqLargeRangeSearchONOFF == ON) || (bTSKG == ON) || (bValveState == ON))
		    {
		        ucNoDebugTime = 0; 
		        //ucControlValveTime = 0;
		        ControlValveFlag = 0;      
		    }
		    else
		    {
		        ucNoDebugTime++;
		        if(((bSzbd == ON)&&(ucNoDebugTime >= 2)) || ((bSzbd == OFF) && (ucNoDebugTime >= 15))) 
		        {
		            ucControlValveTime = 0;         
		            ControlValveFlag = 0;     //2016-5-20添加		            
		            ucNoDebugTime = 0;
		            bTSKG = ON;
		            F_ADC = 0; 
		            F_OUT = F_START;
		            uiPWMCount = 0;
		            //------------20160516添加-----------------------//		                   
		        }       
		    }
		    break;
        case 240:
            T1_Count1 = 0;
            break;
        default:
            break;     
    }
}

//------------------------------------------------------//
//--搜频完成后标定数据
//------------------------------------------------------//

void MarkArray(void)
{
    unsigned char ucMarkTmp[15];
    FreqSmallRangeSearchONOFF = OFF;
    bTSKG = OFF;
    FreqLargeRangeSearchONOFF = OFF; 
    MarkTime = 0;           
    bSzbd = ON;
    bTSKG = ON; 
    ucMarkTmp[0] = (unsigned char)(uiHostUpValue>>8);
    ucMarkTmp[1] = (unsigned char)(uiHostUpValue&0xFF);
    ucMarkTmp[2] = (unsigned char)(uiHostDownValue>>8);
    ucMarkTmp[3] = (unsigned char)(uiHostDownValue&0xFF);
    ucMarkTmp[4] = 0;   //后续放温补系数K
    ucMarkTmp[5] = 0; 
    ucMarkTmp[6] = (unsigned char)(FreqLargeRangeSearchONOFF&0x01);
    ucMarkTmp[7] = (unsigned char)(F_FIND>>8);
    ucMarkTmp[8] = (unsigned char)(F_FIND&0xFF);
    ucMarkTmp[9] = PassWordH;         //后续存放密码
    ucMarkTmp[10] = PassWordL; 
    ucMarkTmp[11] = (unsigned char)(uiSignalZeroPosition>>8);    //后续存放零位量程等信息
    ucMarkTmp[12] = (unsigned char)(uiSignalZeroPosition&0xFF);
    ucMarkTmp[13] = (unsigned char)(uiSignalRange>>8);
    ucMarkTmp[14] = (unsigned char)(uiSignalRange&0xFF);
    FlashPageErase(0x6A00,SELECT_64K);
    FlashWrite(0x6A00,ucMarkTmp,15,SELECT_64K);
    WDog_Feed();  
}




//----------------------------------------------
//阀门控制函数，控制阀门开度和4-20mA电流
//日期   2016.5.13
//-------------------------------------------
void ControlValve1(void)              //KZF1C
{
    bValveState = OFF;
    bDewaterFlag = OFF;
    bBeng = OFF;
    if(uiTemperatureCodeOpenValve < 0xFFF0)
    {
        if(uiTemperatureCodeOpenValve>(uiTemperatureCodeCloseValve+0x50)) //温度下降导致关阀
        {
            bCloseValveTempFall = ON;
            bStart = 1;        
        } 
        else 
        {
            bCloseValveTempFall = OFF;
            bStart = 1;        
        }           
    }
    if(bStart)
    {
        ucPreOpenValveTime = ucOpenValveTime;
        ucOpenValveTime = 0;  
        return;         
    }
    else
    {
        if(bCloseValveTempFall)
        {
            return;    
        } 
        else
        {
            ucPreOpenValveTime = ucOpenValveTime;
            ucOpenValveTime = 0;  
            return;    
        }   
    }    
}
//--------------------------------------------------------

void ControlValve(void)
{
    static unsigned int  DifferValue = 0;   //信号相对于零位的Differ值
    static unsigned int RangeValue = 0;     //量程-零位
    static unsigned int DACValue = 0;
    float temp;
    WDog_Feed();
    if(Temperature>95 || (bWash == 1))
    {
       ControlValve1();
       return;
    }
    else
    {
        if(bSlaveValveState == ON)
        {
            if(Signal >= uiSignalZeroPosition)
            {
                if(Temperature <= 3.7)
                {
                    bBeng = OFF;
                    if(bValveState) 
                    {
                        ucNoDebugTime = 100;
                        ControlValve1();
                        return;       
                    } 
                    else return;  
                }
                else                      //信号值大于零位且温度值大于3.7度，比较是否超量程
                {
                    if(Signal >= uiSignalRange)
                    {
                        bValveState = ON;      //报警信号
                        bBeng = ON;   
                        DAC0L = 0x00;
                        DAC0H = 0x0D;   
                        return;    
                    }  
                    else
                    {
                        MarkTime = 0;              //避免刚关阀就清洗
                        bValveState = ON;
                        bDewaterFlag = ON;                        
                                                                                    
                        if(Signal > uiHostUpValue)
                        {
                            bValveState = ON;
                            bBeng = ON;
                            return;    
                        } 
                        //2016-5-20添加
                        else if(Signal < uiHostDownValue)
                        {
                                                         
		                    bValveState = OFF; 
		                    bBeng = OFF;
		                    ucControlValveTime = 0;         
    		                ControlValveFlag = 0;     //2016-5-20添加		            
    		                ucNoDebugTime = 0;
    		                bTSKG = ON;
    		                F_ADC = 0; 
    		                F_OUT = F_START;
    		                uiPWMCount = 0;
    		                //DAC0L = 0;
    		                //DAC0H = 0;
		                    return;       
                        }
                        //------end-------------
                        else
                        {                                                      //信号值在下限与上限之间时，按比例输出模拟量
                            DifferValue = Signal - uiSignalZeroPosition;
                            RangeValue = uiSignalRange - uiSignalZeroPosition;  
                            temp = (float)DifferValue/RangeValue;                  //除法运算，需要优化
                            DACValue = 0x0D00*temp;
                            DAC0L = (DACValue&0xFF);
                            DAC0H = ((DACValue>>8)&0x0F);
                            bValveState = OFF;
                            bBeng = OFF;
                            return;                       
                        }
                        
                    }       
                }                    
            }
            else             //信号值小于零位
            {
                bBeng = OFF;
                if(bValveState) 
                {
                    ucNoDebugTime = 100;
                    ControlValve1();
                    return;       
                } 
                else return;                    
            }       
        } 
        else
        {
            bBeng = OFF;
            if(bValveState) 
            {
                ucNoDebugTime = 100;
                ControlValve1();
                return;       
            } 
            else return;     
        }   
    }
            
}






#endif