#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__



#define FreqStep                                 0x01        //小范围&精确调试步进
#define PreciseFreqRange                         0x0020     //精确搜频范围
#define WashFreqRange                            0x0200 
#define SmallFreqRange                           0x00FC      //main中小范围搜频范围  
#define FXCS                                     10//调试次数 分析次数

#define DisplayTitle                        1
#define TempratureTransform                 2
#define DisplayTemperatureValue             3
#define DisplaySignalValue                  4
#define DisplayState                        5
#define DisplayWorkState                    6
#define DisplayTime                         7



extern bit bTSKG;
extern bit bBeng;
extern bit bSzbd;
extern bit ControlValveFlag;
extern bit bDewaterFlag;
extern bit FreqLargeRangeSearchONOFF;
extern bit FreqSmallRangeSearchONOFF;
extern bit bValveState;
extern unsigned int Signal;
extern unsigned int idata uiPWMCount;
extern unsigned int TemperatureCode;
extern float Temperature;
extern unsigned long idata F_ADC;
extern unsigned char DisplayIndex;
extern unsigned int data F_FIND;
extern unsigned int data F_OUT;
extern unsigned int data F_START;
extern unsigned int data F_END;
extern unsigned int idata uiHostUpValue;                        //上限
extern unsigned int idata uiHostDownValue;                      //下限
extern unsigned int idata uiSignalZeroPosition;      //零位
extern unsigned int idata uiSignalRange;             //量程


//extern void ADC0_ISR (void);
//extern void Timer0_ISR(void);
//extern void ADC0_ISR (void);
//extern void SMBUS_ISR(void);


#endif