
#include "includes.h"

/******************************************************************************
    Routine Name    : ComInit
    Parameters      : pconfig
    Return value    : none
    Description     : set nonstandard mode,non-block mode
******************************************************************************/
int ComInit(USART_CONFIG *pconfig)
{
	int fd;
	speed_t speed;
	struct termios options;

	if(pconfig->fd > 0)
		close(pconfig->fd);

	switch(pconfig->usartport) {
		case MRK_COMM_PORT:
			fd=open("/dev/ttyUSBMeark", O_RDWR|O_NOCTTY|O_NDELAY);
			break;
		case DUT_COMM_PORT:
			fd=open("/dev/ttyUSBDut", O_RDWR|O_NOCTTY|O_NDELAY);
			break;
		case GUI_COMM_PORT:
			fd=open("/dev/ttyUSB2", O_RDWR|O_NOCTTY|O_NDELAY);
			break;
		case LED_COMM_PORT:
			fd=open("/dev/ttyUSB3", O_RDWR|O_NOCTTY|O_NDELAY);
			break;
		case CCP_COMM_PORT:
			fd=open("/dev/ttyUSB4", O_RDWR|O_NOCTTY|O_NDELAY);
			break;
		default:
			return FALSE;
	}

	if(fd < 0) {
		printf("open serial err\n");
		return FALSE;
	}

	if(tcgetattr(fd, &options) != 0) {
		printf("setup serial\n");
		return FALSE;
	}
	//-------set databit----------
	options.c_cflag &= ~CSIZE;
	if (pconfig->databit == CHRL_7)
		options.c_cflag |= CS7;
	else
		options.c_cflag |= CS8;

	//-----set baudrate------------------
	printf("Set com %d to %d\n", fd, pconfig->baudrate);
	switch(pconfig->baudrate) {
		case 4800:
			speed = B4800;
			break;
		case 9600:
			speed = B9600;
			break;
		case 19200:
			speed = B19200;
			break;
		case 38400:
			speed = B38400;
			break;
		case 57600:
			speed = B57600;
			break;
		case 115200:
			speed = B115200;
			break;
		default:
			speed = B115200;
			break;
	}

	cfsetispeed(&options,speed);
	cfsetospeed(&options,speed);
   //------------set parity------------
	switch(pconfig->parity) {
		case US_ODD:
			options.c_cflag |= PARENB; //enable par
			options.c_cflag |= PARODD; //enalbe odd par
			options.c_iflag |= INPCK;  //enable input par
			break;
		case US_EVEN:
			options.c_cflag |= PARENB;  //enable par
			options.c_cflag &= ~PARODD; //enalbe even par
			options.c_iflag |= INPCK;   //enable input par
			break;
		default:
			options.c_cflag &= ~PARENB; //disable par
			break;
	}
  //------------set stop bit---------------
	if (pconfig->stopbit == STOP_2)
		options.c_cflag |= CSTOPB;
	else
		options.c_cflag &= ~CSTOPB;

	options.c_oflag &= ~(ONLCR|OCRNL);
	options.c_iflag &= ~(ICRNL|IGNCR|INLCR);
	options.c_lflag &= ~(ECHO|ICANON);
	options.c_cflag |= (CLOCAL|CREAD);

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&options);

	pconfig->fd = fd;
	return TRUE;
}


void UART_SendFrame(int fd, char *pStr, U32 length)
{
	if (write(fd, pStr, length) < 0)
		perror("write error");
}

#define MAX_BUFF_BYTE_NUM   255

U8 rx_cnt = 0;
U8 rx_buf[MAX_BUFF_BYTE_NUM]; // can not be defined in function?

void UART_InitBuff(int fd)
{
    char rbuf[2000];
	read(fd, rbuf, 2000);
}

int UART_GetFrameByEndByte(int fd, U8 end_char, U8 * RecvBuff, U8 * FrameLen)
{
    U8 ch;
    int recvlen;

	recvlen = read(fd, &ch, 1);

    if (recvlen < 0) {
    	return FALSE;
    }

    //if(read(fd, &ch, 1) == 0)
    //	return FALSE;

	if(ch == 0)
    	return FALSE;

	if(ch == end_char)
	{
		memcpy(RecvBuff, rx_buf, rx_cnt);
		memset(rx_buf, 0, MAX_BUFF_BYTE_NUM);
		* FrameLen = rx_cnt;
		rx_cnt = 0;
		return TRUE;
	}
	else if(ch == '\n' )		// If find a 0x0a and end_char is not 0x0a,discard this whole line.
	{
		memset(rx_buf, 0, MAX_BUFF_BYTE_NUM);
		rx_cnt = 0;
		return FALSE;
	}
	else
	{
		rx_buf[rx_cnt++] = ch;

		if(rx_cnt > MAX_BUFF_BYTE_NUM)
		{
			memset(rx_buf, 0, MAX_BUFF_BYTE_NUM);
			rx_cnt = 0;
		}
		return FALSE;
	}
}

int UART_GetLine(int fd, U8 * RecvBuff, U8 * FrameLen)
{
	return(UART_GetFrameByEndByte(fd, '\r', RecvBuff, FrameLen));
}

int UART_CheckStr(int fd, char *str, int second)
{
	char rxBuff[60*1024]={0};
	char rxtmp[2048]={0};
	int timecnt=second*10;
	int time=0;
	int rxlen=0;

	while(1) {
		delay_ms(100);
		rxlen = read(fd, rxtmp, 2048);
		printf("read bytes: %d\n", rxlen);

		if (rxlen > 0) {
			printf(rxtmp);
			strcat(rxBuff, rxtmp);

		}
		if (strstr(rxBuff, str) != NULL ) {
			return TRUE;
		}

		if (time++ > timecnt) {
			return FALSE;
		}
	}
}

int DUT_SendComman(char *cmd)
{
	char cmdBuff[48]={0};

	tcflush(DUT_COMM_setting.fd, TCIOFLUSH);
	sprintf(cmdBuff,"%s%s", cmd, "\n");
	printf(cmdBuff);
	UART_SendFrame(DUT_COMM_setting.fd, cmdBuff, strlen(cmdBuff));

	return TRUE;
}

int DUT_GetCommandAck(char *Ack)
{
	int rxlen=0;

	rxlen = read(DUT_COMM_setting.fd, Ack, 1024);

	printf("read bytes: %d\n", rxlen);

	return rxlen;
}
