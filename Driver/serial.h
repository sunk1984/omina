
#ifndef _SERIAL_H_
#define _SERIAL_H_

#define MRK_COMM_PORT 	 0
#define GUI_COMM_PORT   1
#define DUT_COMM_PORT   2
#define LED_COMM_PORT   3
#define CCP_COMM_PORT   4

#define CHRL_7   7
#define CHRL_8   8

#define US_NONE  4
#define US_ODD   1
#define US_EVEN  0

#define STOP_1   0
#define STOP_1_5 1
#define STOP_2   2

typedef struct _USART_CONFIG{
	U8  usartport;
	U8  databit;
	U8  parity;
	U8  stopbit;
	U32 baudrate;
	int fd;
}USART_CONFIG;

extern int ComInit(USART_CONFIG *pconfig);
extern int UART_GetFrameByEndByte(int fd, U8 end_char, U8 * RecvBuff, U8 * FrameLen);
extern int UART_GetLine(int fd, U8 * RecvBuff, U8 * FrameLen);
extern void UART_SendFrame(int fd, char *pStr, U32 length);
extern void UART_InitBuff(int fd);
extern int DUT_SendComman(char *cmd);
extern int UART_CheckStr(int fd, char *str, int second);
extern int DUT_GetCommandAck(char *Ack);
#endif

