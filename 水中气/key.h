#ifndef __KEY_H__
#define __KEY_H__

#include "global.h"

#define KEY_Start                                  1
#define KEY1                                       2
#define KEY2                                       3
#define KEY3                                       4
#define KEY4                                       5

#define ActivityPassWord                                  1             //����1�����������
#define ActivityPassWordWrong                             4             //����������
#define ActivityParameter1                                2            //���������һҳ
#define ActivityParameter2                                3           //��������ڶ�ҳ



extern bit bChangeTime;
extern bit bChange;
//extern bit StartKeyFlag;
//extern bit HostUpValueHunderdChangeFlag;             //��λ�޸ı�־
//extern bit HostDownValueHundredChangeFlag;
extern bit KeyFlag;     //0:û�а���    1:�а���
 

extern unsigned char Activity;                               //�������
extern unsigned char KEY;
//extern unsigned char KEY1_Count;
//extern unsigned char KEY2_Count;
//extern unsigned char KEY3_Count;
//extern unsigned char KEY4_Count;


extern unsigned char PassWordH;    //�������λ   ��InputPassWord1��InputPassWord2���
extern unsigned char PassWordL;    //�������λ

extern unsigned char InputPassWord1;   //�û������4λ����
extern unsigned char InputPassWord2;
extern unsigned char InputPassWord3;
extern unsigned char InputPassWord4;


extern void KeyService(void);


#endif