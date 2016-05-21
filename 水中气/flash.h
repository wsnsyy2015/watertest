#ifndef __FLASH_H__
#define __FLASH_H__

#include "global.h"

#define SELECT_128BYTE               1
#define SELECT_64K                   0

extern void FlashWrite(unsigned int FlashAdd,unsigned char *Data,unsigned char len,bit SFLE);
extern unsigned char FlashReadByte(unsigned int FlashAdd,bit SFLE);
extern void FlashPageErase(unsigned int FlashAdd,bit SFLE);

#endif
