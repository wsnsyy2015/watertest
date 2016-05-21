#ifndef __LCD_H__
#define __LCD_H__
//#include <c8051f020.h>

#define BackLightOn            0x5F
#define BackLightOff           0x5E

#define   UpValue                              0     //пе╨еиооч
#define   DownValue                            1

//#define Select_Max                          0
//#define Select_Min                          1
//#define Select_WENBU                        2
//#define Select_Number                       3
//#define Select_Debug                        4
//#define Select_PassWord                     5



extern unsigned int idata RectanglePos[2];
extern unsigned int idata TrianglePos[2];
extern unsigned int idata FillPlacePos[4];

extern void T1LCDBackLight(unsigned char PWM_T);
extern void LCDClear(void);
extern void LCDDisplayPic(unsigned char PicID);
extern void SetMixColor(unsigned int FColor,unsigned int BColor);   
extern void LCDDisplayTriangle(unsigned int Xpos,unsigned int Ypos);
extern void FillRect(unsigned int * pRectPlace);
extern void LCDRefresh(void);
extern void LCDDisplayTime(void);





#endif