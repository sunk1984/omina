

/*****************************************************************************
ʹ��˵��:

1.slave_add ָ ����ExtIO board�ĵ�ַ���е���R22��������һ�飬Ҳ���Թ��档

2.pin��16���Ʊ�ʾPCA9505�����š�
  ���� pin-->0x15 ������� IO 1.5��
  pin-->0x23 �������IO 2.3.

3.bank �У�0��ʾbank 0, 1 ��ʾbank 1���Դ�����

4.direction�����ֱ�ʾ IO_OUTPUT��IO_INPUT��

5.value �����ֱ�ʾ0,1.�����ã�����0��


******************************************************************************/

#ifndef     __EXTIO_H__
#define     __EXTIO_H__

//����IO�ṹ�еķ���
#define IO_OUTPUT 0
#define IO_INPUT  1

#define IO_CHANNEL_TOTAL  24

//EXTIO board�Ľṹ����
struct IO_INFO
{ 
    INT32U Channel;     //���ź�
    INT32U Direction; //����
    INT32U Value;     //output����ʱ��ֵ
};


//����bit����
extern BOOL EXTIO_ConfigureBitDirction(U8 TotalChan, U8 BitDirData);

//����byte�ķ���
extern BOOL EXTIO_ConfigureByteDirction(U8 TotalChan, U8 ByteDirData);

//��дһ��byte�ĺ���
extern BOOL EXTIO_WriteByte(U8 TotalChan, U8 ByteData);
extern BOOL EXTIO_ReadByte(U8 TotalChan, U8 * ReadByte);

//��дĳһ��bit�ĺ���
extern BOOL EXTIO_WriteBit(U8 TotalChan, U8 BitData);
extern BOOL EXTIO_ReadBit(U8 TotalChan, U8 * ReadBit);

//
extern INT32U EXTIO_Init(struct IO_INFO io_info[]);

#endif



