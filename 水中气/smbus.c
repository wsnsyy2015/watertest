#include "smbus.h"

unsigned char WriteAdd;
unsigned char ReadAdd;
unsigned char TransmitByte;
unsigned char ReceiveByte;
unsigned char MemeryAdd;

unsigned char timebuf[2];

//bit IsAfterMinute;
bit SM_BUSY;
bit SMBUS_RW;  //W  :0     R    :1
bit ByteSend;

//-------------------------------------------------
//初始化
//---------------------------------------------------
void SMBus_Init(void){
    SMB0CN = 0x04;           //在应答周期内返回ack
    SMB0CR = 0xE7;           //400kHZ 
    SMB0CN |= 0x40;          //使能
    EIE1 |= 0x02;
    SM_BUSY = 0;        
}

//--------------------------------------------------
//I2C写数据
//ChipAddress :     器件地址
//MemeryAddress:    寄存器地址
//DataByte     ：   待发送数据
//--------------------------------------------------
void SM_Send(unsigned char ChipAddress,unsigned char MemeryAddress,unsigned char DataByte)
{
    while(SM_BUSY);//等待空闲
    SMBUS_RW = 0;        
    WriteAdd = (ChipAddress | WRITE);
    MemeryAdd = MemeryAddress;
    TransmitByte = DataByte;
    SM_BUSY = 1;
    STA = 1;
}

//-------------------------------------------------
//I2C读数据
//ChipAddress:     器件地址
//MemeryAddress:   寄存器地址
//2016-5-20更新：  增加延时计数，防止时钟芯片故障时程序死等
//-------------------------------------------------
unsigned char SM_Receive(unsigned char ChipAddress,unsigned char MemeryAddress)
{
	static unsigned int delaycount = 0;
    while(SM_BUSY)
	{                         //避免PCF8563故障时死等
		delaycount++;
		if(delaycount == 10000)
		{
			delaycount = 0;
			return 0xFF;
		}
	}
	delaycount = 0;
    SMBUS_RW = 1;
    WriteAdd = (ChipAddress | WRITE);   //写器件地址
    ReadAdd = (ChipAddress | READ);     //读
    MemeryAdd = MemeryAddress;
    SM_BUSY = 1;
    STA = 1;
    while(SM_BUSY)
	{
		delaycount++;
		if(delaycount == 10000)
		{
			delaycount = 0;
			return 0xFF;
		}
	}
	delaycount = 0;
    return ReceiveByte;            
}


//-----------------------------------------------------------
//SMBus中断服务
//-----------------------------------------------------------
void SMBUS_ISR (void) interrupt 7
{
	SMB0STA = (SMB0STA & 0X7F);
	switch (SMB0STA){ 					// SMBus 状态码（SMB0STA 寄存器）
// 主发送器/接收器：起始条件已发送
// 在该状态发送的COMMAND 字的R/W 位总是为0(W)，
// 因为对于读和写操作来说都必须先写存储器地址。
    	case SMB_BUS_ERROR:
    	{
    		STO = 1;
    		break;
    	}
    	case SMB_START:
    	{
    		SMB0DAT = WriteAdd; 		// 装入要访问的从器件的地址
    		STA = 0; 							// 手动清除START 位
    	break;
    	}
    //主发送器/接收器：重复起始条件已发送。
    // 该状态只应在读操作期间出现，在存储器地址已发送并得到确认之后
    	case SMB_RP_START:
    	{
    		SMB0DAT = ReadAdd; 					// COMMAND 中应保持从地址+ R.
    		STA = 0;
    		break;
    	}
    // 主发送器：从地址+ WRITE 已发送，收到ACK。
    	case SMB_MTADDACK:
    	{
    		SMB0DAT = MemeryAdd; 				// 装入待写存储器地址的高字节
    		ByteSend = 1;
    		break;
    	}
    // 主发送器：从地址+ WRITE 已发送，收到NACK。
    // 从器件不应答，发送STOP + START 重试
    	case SMB_MTADDNACK:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // 主发送器：数据字节已发送，收到ACK。
    // 该状态在写和读操作中都要用到。BYTE_NUMBER 看存储器地址状态– 如果
    // 只发送了HIGH_ADD，则装入LOW_ADD。如果LOW_ADD 已发送，检查COMMAND
    // 中的R/W 值以决定下一状态。
    	case SMB_MTDBACK:
    	{
    		if (ByteSend == 1)
    		{
    			ByteSend = 0;
    			if (SMBUS_RW == 1)
    			{
    				STA = 1;
    				
    			}
    			else
    			{
    				SMB0DAT = TransmitByte;
    				
    			}
    		}
    		else
    		{
    			STO = 1;
    			SM_BUSY = 0;
    		}
    		break;
    	}
    // 主发送器：数据字节已发送，收到NACK。
    // 从器件不应答，发送STOP + START 重试
    	case SMB_MTDBNACK:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // 主发送器：竞争失败
    // 不应出现。如果出现，重新开始传输过程
    	case SMB_MTARBLOST:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // 主接收器：从地址+ READ 已发送。收到ACK。
    // 设置为在下一次传输后发送NACK，因为那将是最后一个字节（唯一）。
    	case SMB_MRADDACK:
    	{
    		AA = 0; 						// 在应答周期NACK。
    		break;
    	}
    // 主接收器：从地址+ READ 已发送。收到NACK。
    // 从器件不应答，发送重复起始条件重试
    	case SMB_MRADDNACK:
    	{
    		STO= 1;
    		break;
    	}
    // 收到数据字节。ACK 已发送。
    // 该状态不应出现，因为AA 已在前一状态被清0。如果出现，发送停止条件。
    	case SMB_MRDBACK:
    	{
    		STA= 1;
    		SM_BUSY = 0;
    		break;
    	}
    // 收到数据字节。NACK 已发送。
    // 读操作已完成。读数据寄存器后发送停止条件。
    	case SMB_MRDBNACK:
    	{
    		ReceiveByte = SMB0DAT;
    		STO = 1;		
    		AA = 1; 						// 在应答周期NACK。
    		SM_BUSY = 0; 					// 释放SMBus
    		break;
    	}
    // 在本应用中，所有其它状态码没有意义。通信复位。
    	default:
    	{
    		STO = 1; 						// 通信复位。
    		SM_BUSY = 0;
    		break;
    	}
	}
    SI=0; 								// 清除中断标志
}
