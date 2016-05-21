#ifndef __KEY_H__
#define __KEY_H__

#include "global.h"

#define KEY_Start                                  1
#define KEY1                                       2
#define KEY2                                       3
#define KEY3                                       4
#define KEY4                                       5

#define ActivityPassWord                                  1             //界面1，即密码界面
#define ActivityPassWordWrong                             4             //密码错误界面
#define ActivityParameter1                                2            //参数界面第一页
#define ActivityParameter2                                3           //参数界面第二页



extern bit bChangeTime;
extern bit bChange;
//extern bit StartKeyFlag;
//extern bit HostUpValueHunderdChangeFlag;             //百位修改标志
//extern bit HostDownValueHundredChangeFlag;
extern bit KeyFlag;     //0:没有按键    1:有按键
 

extern unsigned char Activity;                               //界面序号
extern unsigned char KEY;
//extern unsigned char KEY1_Count;
//extern unsigned char KEY2_Count;
//extern unsigned char KEY3_Count;
//extern unsigned char KEY4_Count;


extern unsigned char PassWordH;    //密码高两位   由InputPassWord1和InputPassWord2组成
extern unsigned char PassWordL;    //密码低两位

extern unsigned char InputPassWord1;   //用户输入的4位密码
extern unsigned char InputPassWord2;
extern unsigned char InputPassWord3;
extern unsigned char InputPassWord4;


extern void KeyService(void);


#endif