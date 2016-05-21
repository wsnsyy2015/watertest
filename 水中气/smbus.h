#ifndef __SMBUS_H__
#define __SMBUS_H__

#include "global.h"

#define WRITE                    0x00
#define READ                     0x01

//#define CHIP_Read  0xA3 	// PCF8563 ��������ַ
//#define CHIP_Write 0xA2 	// PCF8563 ��������ַ

#define SMB_BUS_ERROR           0x00 		// �������з�ʽ�����ߴ���
#define SMB_START               0x08 			// (MT & MR)��ʼ�����ѷ���
#define SMB_RP_START            0x10 		// (MT & MR)�ظ���ʼ����
#define SMB_MTADDACK            0x18 		// (MT) �ӵ�ַ+ W �ѷ��ͣ��յ�ACK
#define SMB_MTADDNACK           0x20 		// (MT) �ӵ�ַ+ W �ѷ��ͣ��յ�NACK
#define SMB_MTDBACK             0x28 		// (MT) �����ֽ��ѷ��ͣ��յ�ACK
#define SMB_MTDBNACK            0x30 		// (MT) �����ֽ��ѷ��ͣ��յ�NACK
#define SMB_MTARBLOST           0x38 		// (MT) ����ʧ��
#define SMB_MRADDACK            0x40 		// (MR) �ӵ�ַ+ R �ѷ��ͣ��յ�ACK
#define SMB_MRADDNACK           0x48 		// (MR) �ӵ�ַ+ R �ѷ��ͣ��յ�NACK
#define SMB_MRDBACK             0x50 		// (MR) �յ������ֽڣ�ACK �ѷ���
#define SMB_MRDBNACK            0x58 		// (MR) �յ������ֽڣ�NACK �ѷ���


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