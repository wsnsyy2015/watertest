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
//��ʼ��
//---------------------------------------------------
void SMBus_Init(void){
    SMB0CN = 0x04;           //��Ӧ�������ڷ���ack
    SMB0CR = 0xE7;           //400kHZ 
    SMB0CN |= 0x40;          //ʹ��
    EIE1 |= 0x02;
    SM_BUSY = 0;        
}

//--------------------------------------------------
//I2Cд����
//ChipAddress :     ������ַ
//MemeryAddress:    �Ĵ�����ַ
//DataByte     ��   ����������
//--------------------------------------------------
void SM_Send(unsigned char ChipAddress,unsigned char MemeryAddress,unsigned char DataByte)
{
    while(SM_BUSY);//�ȴ�����
    SMBUS_RW = 0;        
    WriteAdd = (ChipAddress | WRITE);
    MemeryAdd = MemeryAddress;
    TransmitByte = DataByte;
    SM_BUSY = 1;
    STA = 1;
}

//-------------------------------------------------
//I2C������
//ChipAddress:     ������ַ
//MemeryAddress:   �Ĵ�����ַ
//2016-5-20���£�  ������ʱ��������ֹʱ��оƬ����ʱ��������
//-------------------------------------------------
unsigned char SM_Receive(unsigned char ChipAddress,unsigned char MemeryAddress)
{
	static unsigned int delaycount = 0;
    while(SM_BUSY)
	{                         //����PCF8563����ʱ����
		delaycount++;
		if(delaycount == 10000)
		{
			delaycount = 0;
			return 0xFF;
		}
	}
	delaycount = 0;
    SMBUS_RW = 1;
    WriteAdd = (ChipAddress | WRITE);   //д������ַ
    ReadAdd = (ChipAddress | READ);     //��
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
//SMBus�жϷ���
//-----------------------------------------------------------
void SMBUS_ISR (void) interrupt 7
{
	SMB0STA = (SMB0STA & 0X7F);
	switch (SMB0STA){ 					// SMBus ״̬�루SMB0STA �Ĵ�����
// ��������/����������ʼ�����ѷ���
// �ڸ�״̬���͵�COMMAND �ֵ�R/W λ����Ϊ0(W)��
// ��Ϊ���ڶ���д������˵��������д�洢����ַ��
    	case SMB_BUS_ERROR:
    	{
    		STO = 1;
    		break;
    	}
    	case SMB_START:
    	{
    		SMB0DAT = WriteAdd; 		// װ��Ҫ���ʵĴ������ĵ�ַ
    		STA = 0; 							// �ֶ����START λ
    	break;
    	}
    //��������/���������ظ���ʼ�����ѷ��͡�
    // ��״ֻ̬Ӧ�ڶ������ڼ���֣��ڴ洢����ַ�ѷ��Ͳ��õ�ȷ��֮��
    	case SMB_RP_START:
    	{
    		SMB0DAT = ReadAdd; 					// COMMAND ��Ӧ���ִӵ�ַ+ R.
    		STA = 0;
    		break;
    	}
    // �����������ӵ�ַ+ WRITE �ѷ��ͣ��յ�ACK��
    	case SMB_MTADDACK:
    	{
    		SMB0DAT = MemeryAdd; 				// װ���д�洢����ַ�ĸ��ֽ�
    		ByteSend = 1;
    		break;
    	}
    // �����������ӵ�ַ+ WRITE �ѷ��ͣ��յ�NACK��
    // ��������Ӧ�𣬷���STOP + START ����
    	case SMB_MTADDNACK:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // ���������������ֽ��ѷ��ͣ��յ�ACK��
    // ��״̬��д�Ͷ������ж�Ҫ�õ���BYTE_NUMBER ���洢����ַ״̬�C ���
    // ֻ������HIGH_ADD����װ��LOW_ADD�����LOW_ADD �ѷ��ͣ����COMMAND
    // �е�R/W ֵ�Ծ�����һ״̬��
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
    // ���������������ֽ��ѷ��ͣ��յ�NACK��
    // ��������Ӧ�𣬷���STOP + START ����
    	case SMB_MTDBNACK:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // ��������������ʧ��
    // ��Ӧ���֡�������֣����¿�ʼ�������
    	case SMB_MTARBLOST:
    	{
    		STO = 1;
    		STA = 1;
    		break;
    	}
    // �����������ӵ�ַ+ READ �ѷ��͡��յ�ACK��
    // ����Ϊ����һ�δ������NACK����Ϊ�ǽ������һ���ֽڣ�Ψһ����
    	case SMB_MRADDACK:
    	{
    		AA = 0; 						// ��Ӧ������NACK��
    		break;
    	}
    // �����������ӵ�ַ+ READ �ѷ��͡��յ�NACK��
    // ��������Ӧ�𣬷����ظ���ʼ��������
    	case SMB_MRADDNACK:
    	{
    		STO= 1;
    		break;
    	}
    // �յ������ֽڡ�ACK �ѷ��͡�
    // ��״̬��Ӧ���֣���ΪAA ����ǰһ״̬����0��������֣�����ֹͣ������
    	case SMB_MRDBACK:
    	{
    		STA= 1;
    		SM_BUSY = 0;
    		break;
    	}
    // �յ������ֽڡ�NACK �ѷ��͡�
    // ����������ɡ������ݼĴ�������ֹͣ������
    	case SMB_MRDBNACK:
    	{
    		ReceiveByte = SMB0DAT;
    		STO = 1;		
    		AA = 1; 						// ��Ӧ������NACK��
    		SM_BUSY = 0; 					// �ͷ�SMBus
    		break;
    	}
    // �ڱ�Ӧ���У���������״̬��û�����塣ͨ�Ÿ�λ��
    	default:
    	{
    		STO = 1; 						// ͨ�Ÿ�λ��
    		SM_BUSY = 0;
    		break;
    	}
	}
    SI=0; 								// ����жϱ�־
}
