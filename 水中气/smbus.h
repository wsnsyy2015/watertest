#ifndef __SMBUS_H__
#define __SMBUS_H__

#include "global.h"

#define WRITE                    0x00
#define READ                     0x01

//#define CHIP_Read  0xA3 	// PCF8563 器件读地址
//#define CHIP_Write 0xA2 	// PCF8563 器件读地址

#define SMB_BUS_ERROR           0x00 		// （对所有方式）总线错误
#define SMB_START               0x08 			// (MT & MR)起始条件已发送
#define SMB_RP_START            0x10 		// (MT & MR)重复起始条件
#define SMB_MTADDACK            0x18 		// (MT) 从地址+ W 已发送；收到ACK
#define SMB_MTADDNACK           0x20 		// (MT) 从地址+ W 已发送；收到NACK
#define SMB_MTDBACK             0x28 		// (MT) 数据字节已发送；收到ACK
#define SMB_MTDBNACK            0x30 		// (MT) 数据字节已发送；收到NACK
#define SMB_MTARBLOST           0x38 		// (MT) 竞争失败
#define SMB_MRADDACK            0x40 		// (MR) 从地址+ R 已发送；收到ACK
#define SMB_MRADDNACK           0x48 		// (MR) 从地址+ R 已发送；收到NACK
#define SMB_MRDBACK             0x50 		// (MR) 收到数据字节；ACK 已发送
#define SMB_MRDBNACK            0x58 		// (MR) 收到数据字节；NACK 已发送


//extern bit SM_BUSY;
//extern bit ByteSend;
//extern bit SMBUS_RW;
//extern unsigned char WriteAdd;
//extern unsigned char ReadAdd;
//extern unsigned char TransmitByte;
//extern unsigned char ReceiveByte;
//extern unsigned char MemeryAdd;

extern void SMBus_Init(void);
extern void SM_Send(unsigned char ChipAddress,unsigned char MemeryAddress,unsigned char DataByte);
extern unsigned char SM_Receive(unsigned char ChipAddress,unsigned char MemeryAddress);


#endif