

#include "includes.h"


//Configure USARTs

USART_CONFIG MRK_COMM_setting = {MRK_COMM_PORT, CHRL_8, US_NONE, STOP_1, 115200}; //MERAK
USART_CONFIG GUI_COMM_setting = {GUI_COMM_PORT, CHRL_8, US_NONE, STOP_1, 9600};   //GUI
USART_CONFIG DUT_COMM_setting = {DUT_COMM_PORT, CHRL_8, US_NONE, STOP_1, 115200}; //DUT
USART_CONFIG LED_COMM_setting = {LED_COMM_PORT, CHRL_8, US_NONE, STOP_1, 57600};  //LED
USART_CONFIG CCP_COMM_setting = {CCP_COMM_PORT, CHRL_8, US_NONE, STOP_1, 9600};   //CCP

//Configure RLY boards and IO boards

#define DONE    2

#define RELAY_BOARD_MAX    3
#define IO_BOARD_MAX     	1
#define IO_GROUP_PERBOARD  3

char pass_rpt[] = "\x10<PASS>\r\n";
char fail_rpt[] = "\x10<FAIL>\r\n";
char done_rpt[] = "\x10<DONE>\r\n";
char ready_rpt[] = "\x10[READY]\r\n";

typedef struct {
	char head[48];
	char board_name[48];
	char board_version[48];
	char software_version[48];
	char date[48];
	char MAC_address[48];
	char serial_num[48];
	char port[48];
}EEPINFO;

EEPINFO eepInfo;

#define ASSEMBLY_A001 1
#define ASSEMBLY_A002 2
#define ASSEMBLY_A101 3

#define DUT_ACK_BUFF_SIZE 512

U8 assembly_type;
int input_voltage=0;

void GUI_SendMessage(char *lpszFormat, ...)
{
    char txBuff[100] = {0};
    va_list fmtList;

    va_start( fmtList, lpszFormat );
    vsprintf( (char *)txBuff, lpszFormat, fmtList );
    va_end( fmtList );

	printf( (char *)txBuff);
    //LOGFILE_AddItem(txBuff);
	UART_SendFrame(GUI_COMM_setting.fd, txBuff, strlen(txBuff));
}

void Modules_Config(void)
{
    ComInit(&MRK_COMM_setting);	// fd:3
    ComInit(&GUI_COMM_setting);	// fd:4
	ComInit(&DUT_COMM_setting);	// fd:5
    ComInit(&LED_COMM_setting);
    ComInit(&CCP_COMM_setting);

	RLY_SetAdMode(1);   //Configure RLY board 1 to Common mode
	RLY_SetAdMode(2);   //Configure RLY board 2 to Common mode
    RLY_SetCommonMode(3);   //Configure RLY board 3 to Common mode

    GUI_SendMessage("Fixture is ready\r\n");
}

void Modules_Init(void)
{
	int i;
    RLY_Clear(RELAY_BOARD_MAX);
    for (i=1; i<18; i++) {
    	EXTIO_ConfigureBitDirction(i, IO_INPUT);
    }
	//EXTIO_ConfigureByteDirction(1,  0xff);	//
	//EXTIO_ConfigureByteDirction(9,  0xff);
	//EXTIO_ConfigureByteDirction(17, 0xff);

	//PWR_TurnOffDut();
}

void ReportResult(U8 res)
{
    if(res == PASS)
    	GUI_SendMessage(pass_rpt);
    else if(res == FAIL)
    	GUI_SendMessage(fail_rpt);
    else if(res == DONE)
    	GUI_SendMessage(done_rpt);

	delay_ms(30);
	GUI_SendMessage(ready_rpt);
}

#define VOLT_12V_MAX	12388
#define VOLT_12V_MIN	11507

#define VOLT_5V_MAX		5356
#define VOLT_5V_MIN		4828

#define VOLT_3V3_MAX	3350
#define VOLT_3V3_MIN	3250

#define VOLT_2V6_MAX	2640
#define VOLT_2V6_MIN	2600

#define VOLT_1V3_MAX	1314
#define VOLT_1V3_MIN	1280

#define VOLT_1V05_MAX	1062
#define VOLT_1V05_MIN	1034

#define VOLT_1V8_MAX	1880
#define VOLT_1V8_MIN	1774

#define CURR_TOT_MAX	350	/////////////////////////////
#define CURR_TOT_MIN	10	/////////////////////////////

#define CURR_SRC_45MAX	50
#define CURR_SRC_45MIN	40

#define CURR_SRC_35MAX	38
#define CURR_SRC_35MIN	30

typedef struct {
	U8 title[48];
	U8 tp_name[8];
	U32 lower;
	U32 upper;
	U8 channel;

} VOLT_ITEM_T, *P_VOLT_ITEM_T;

const VOLT_ITEM_T volt_item[] = {
	{"24V Measurement",       "MTP22", 22000, 26000, 2},
	{"3.3V Measurement",      "MTP11", 3250,  3350,  3},
	{"5.0V_PS Measurement",   "MTP12", 4750,  5250,  4},
	{"VDDARM_IN Measurement", "MTP5",  1227,  1270,  9},
	{"VDDSOC_IN Measurement", "MTP8",  1227,  1270,  11},
	{"1.5V Measurement",      "MTP21", 1470,  1530,  7},
	{"1.1V Measurement",      "MTP9",  1070,  1130,  8},
	{"1.8V  Measurement",     "MTP7",  1770,  1830,  5},
	{"5.0V Measurement",      "MTP13", 4750,  5250,  10},
	{"1.35V Measurement",     "MTP6",  1330,  1370,  6},
	{"DDR_VTT Measurement",   "MTP27", 660,   685,   12},
	{"DDR_VREF Measurement",  "MTP26", 665,   685,   13},
};

const VOLT_ITEM_T cvolt_item[] = {
	{"5.0V_USB Measurement",  "MTP14", 4750,  5250,  14},
	{"7.5V Measurement",      "MTP10", 7250,  7750,  15}
};

const VOLT_ITEM_T LED_volt_item[] = {
	{"LED D7",  "", 30,  100,  18},
	{"LED D8",  "", 30,  100,  21},
	{"LED D17", "", 30,  100,  22},
	{"LED D18", "", 30,  100,  23},
	{"LED D19", "", 30,  100,  24},
	{"LED D20", "", 30,  100,  40},
};

const VOLT_ITEM_T Impedance_item[] = {

	{"24V Measurement",       "MTP22", 200, 2000,  2},
	{"3.3V Measurement",      "MTP11", 200, 1000,  3},
	{"5.0V_PS Measurement",   "MTP12", 200, 1000,  4},
	{"VDDARM_IN Measurement", "MTP5",  200, 1000,  9},
	{"VDDSOC_IN Measurement", "MTP8",  200, 1000,  11},
	{"1.5V Measurement",      "MTP21", 200, 1000,  7},
	{"1.1V Measurement",      "MTP9",  200, 1000,  8},
	{"1.8V  Measurement",     "MTP7",  200, 1000,  5},
	{"5.0V Measurement",      "MTP13", 200, 1000,  10},
	{"1.35V Measurement",     "MTP6",  200, 1000,  6},
	{"DDR_VTT Measurement",   "MTP27", 200, 1000,  12},
	{"DDR_VREF Measurement",  "MTP26", 200, 1000,  13},
	{"5.0V_USB Measurement",  "MTP14", 200, 1000,  14},
	//{"7.5V Measurement",      "MTP10", 200, 1000,  15},
};

VOLT_ITEM_T Reset_item = {"Reset switch",  "MTP3", 0,  200,  16};
VOLT_ITEM_T Battery_item = {"Battery",  "MTP28", 3150,  3500,  17};

int getVoltage(int channle)
{
	U32 volt=0;
	RLY_ON(channle);
	delay_ms(30);
	ADC_GetVolt(&volt);
	RLY_OFF(channle);
	if (volt < 1500) {
		volt=volt*99/100;
	}
	return volt;
}

static BOOL TestVoltages(P_VOLT_ITEM_T pitem)
{
	U32 volt;

	volt = getVoltage(pitem->channel);
	GUI_SendMessage("%s\r\n%s:%2d.%03dV\r\n", pitem->title, pitem->tp_name, volt/1000, volt%1000);

	if (volt >= pitem->lower && volt < pitem->upper)
		return PASS;
	else
		return FAIL;
}

static U32 TestCurrent(U8 rly_bef_res, U8 rly_aft_res, int resis)
{
	U32 volt_former=0;
	U32 volt_latter=0;
	U32 volt_diff=0;
	U32 curr;

	volt_former=getVoltage(rly_bef_res);
	Dprintf("former=%2d.%03dV\n", volt_former/1000, volt_former%1000);
	delay_ms(50);

	volt_latter=getVoltage(rly_aft_res);
	Dprintf("latter=%2d.%03dV\n", volt_latter/1000, volt_latter%1000);

    Dprintf("res=%d Ohm\n", resis);

    volt_diff = volt_former - volt_latter;
    curr = volt_diff/resis;
    //GUI_SendMessage("%2d.%03dA\r\n", curr/1000, curr%1000);
    GUI_SendMessage("%dmA\r\n", curr);

	return curr;
}

int parseProgrammingParameter(char *cmd)
{
	char *pStr;
	char *line;
	//int index=0;

	pStr=cmd;
	while( (line=strtok(pStr, " ")) != NULL) {
		switch(*line) {
		case 'i':
			strcpy(eepInfo.head, (line+2));
			break;
		case 'a':
			strcpy(eepInfo.board_name, (line+2));
			break;
		case 'b':
			strcpy(eepInfo.board_version, (line+2));
			break;
		case 'v':
			strcpy(eepInfo.software_version, (line+2));
			break;
		case 'n':
			strcpy(eepInfo.serial_num, (line+2));
			break;
		case 'p':
			strcpy(eepInfo.port, (line+2));
			break;
		case 'd':
			strcpy(eepInfo.date, (line+2));
			break;
		default:
			if ((*line >= '0') && (*line <= '7') ) {
				strcpy(eepInfo.MAC_address, line);
				eepInfo.MAC_address[1]=' ';
			}
			break;
		}
		pStr += strlen(line)+1;
	}
	return TRUE;
}


static BOOL Currentloop_Active_test1(void)
{
	char tx_cmd[] = "echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttymxc4";
    char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";

    tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
    //DUT send test string
    DUT_SendComman(tx_cmd);
    //CCP receive test string
    if (UART_CheckStr(CCP_COMM_setting.fd, test_str, 1)) {
    	GUI_SendMessage("Pass\r\n");
    	return PASS;
    }
    else {
    	GUI_SendMessage("Fail\r\n");
    	return FAIL;
    }
}

static BOOL Currentloop_Active_test2(void)
{
	char tx_cmd[] = "cat < /dev/ttymxc4 &";
	char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
	char send_str[100]={0};
	char buff[1024]={0};
	char cmd[10]={0};
	int i=0;
	int ret;

	sprintf(send_str,"%s%s", test_str, "\r");
	DUT_SendComman("cat < /dev/ttymxc4 &");
	//CCP send test string
	tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
	delay_ms(100);

	/*for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				GUI_SendMessage("Pass\r\n");
				return PASS;
			}
	}

	GUI_SendMessage("Fail\r\n");
	return FAIL;*/
	for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				//GUI_SendMessage("Pass\r\n");
				//return PASS;
				break;
			}
	}
	if (i >= 3) {
		ret = FAIL;
	}
	else {
		ret = PASS;
	}

	DUT_SendComman("fg");
	delay_ms(10);
	cmd[0]=0x03;//ctrl+c
	DUT_SendComman(cmd);
	delay_ms(10);
	return ret;
}

static BOOL Currentloop_Active_test3(void)
{
	char tx_cmd[] = "echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS1";
    char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";

    tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
    //DUT send test string
    DUT_SendComman("echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS1");
    //CCP receive test string
    if (UART_CheckStr(CCP_COMM_setting.fd, test_str, 1)) {
    	GUI_SendMessage("Pass\r\n");
    	return PASS;
    }
    else {
    	GUI_SendMessage("Fail\r\n");
    	return FAIL;
    }
}

static BOOL Currentloop_Active_test4(void)
{
    char tx_cmd[] = "cat < /dev/ttyS1 &";
    char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
	char send_str[100]={0};
	char buff[1024]={0};
	char cmd[10]={0};
	int ret;
	int i=0;

	sprintf(send_str,"%s%s", test_str, "\r");
	DUT_SendComman("cat < /dev/ttyS1 &");
	//CCP send test string
	tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
	delay_ms(100);
	//UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
	//DUT receive test string
	/*for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				GUI_SendMessage("Pass\r\n");
				return PASS;
			}
	}

	GUI_SendMessage("Fail\r\n");
	return FAIL;*/

	for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				//GUI_SendMessage("Pass\r\n");
				//return PASS;
				break;
			}
	}
	if (i >= 3) {
		ret = FAIL;
	}
	else {
		ret = PASS;
	}

	DUT_SendComman("fg");
	delay_ms(10);
	cmd[0]=0x03;//ctrl+c
	DUT_SendComman(cmd);
	delay_ms(10);
	return ret;
}

static BOOL Currentloop_Active_test5(void)
{
	char tx_cmd[] = "echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS0";
    char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
    int ret;
    int i;
    tcflush(CCP_COMM_setting.fd, TCIOFLUSH);

    //DUT send test string
    /*DUT_SendComman("echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS0");
    //CCP receive test string
    if (UART_CheckStr(CCP_COMM_setting.fd, test_str, 1)) {
    	GUI_SendMessage("Pass\r\n");
    	return PASS;
    }
    else {
    	GUI_SendMessage("Fail\r\n");
    	return FAIL;
    }*/

    for (i=0; i<3; i++) {
    	//DUT send test string
    	   DUT_SendComman("echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS0");
    	   if (UART_CheckStr(CCP_COMM_setting.fd, test_str, 1)) {
    	       	break;
    	      }
    }
    if (i>=3) {
    	ret = FAIL;
    }
    else {
    	ret =PASS;
    }
    return ret;

}

static BOOL Currentloop_Active_test6(void)
{
	char tx_cmd[] = "cat < /dev/ttyS0 &";
	char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
	char send_str[100]={0};
	char cmd[10]={0};
	int ret;
	int i=0;

	sprintf(send_str,"%s%s", test_str, "\r");
	DUT_SendComman("cat < /dev/ttyS0 &");
	//CCP send test string
	tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
	delay_ms(100);
	//UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
	//DUT receive test string
	/*for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				GUI_SendMessage("Pass\r\n");
				return PASS;
			}
	}

	GUI_SendMessage("Fail\r\n");
	return FAIL;*/

	for (i=0; i<3; i++) {
		UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
		if (UART_CheckStr(DUT_COMM_setting.fd, test_str, 1)) {
				//GUI_SendMessage("Pass\r\n");
				//return PASS;
				break;
			}
	}
	if (i >= 3) {
		ret = FAIL;
	}
	else {
		ret = PASS;
	}

	DUT_SendComman("fg");
	delay_ms(10);
	cmd[0]=0x03;//ctrl+c
	DUT_SendComman(cmd);
	delay_ms(10);
	return ret;

}

static BOOL Currentloop_Active_test7(void)
{
	char rbuf[1024]={0};
    const char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
    int ret;
    char cmd[10]={0};

    DUT_SendComman("cat < /dev/ttyS0 &");
    //DUT_GetCommandAck(rbuf);
    DUT_SendComman("echo \"the quick brown fox jumps over the lazy dog 0123456789\" > /dev/ttyS1");
    delay_ms(200);
    DUT_GetCommandAck(rbuf);
    if (strstr(rbuf, test_str)) {
    	//GUI_SendMessage("Pass\r\n");
    	//return PASS;
    	ret = PASS;
    }
    else {
    	//GUI_SendMessage("Fail\r\n");
    	//return FAIL;
    	ret = FAIL;
    }
    DUT_SendComman("fg");
	delay_ms(10);
	cmd[0]=0x03;//ctrl+c
	DUT_SendComman(cmd);
	delay_ms(10);
	return ret;
}

static BOOL Currentloop_Passive_test1(void)
{
	char Dutrbuf[1024]={0};
	char ccprbuf[1024]={0};
    const char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
    char send_str[100]={0};
    int DutRecvlen=0;
    int i=0;
    int ret=FAIL;
    char *str;

    sprintf(send_str,"%s%s", test_str, "\r");
    DUT_SendComman("cat < /dev/ttyS1 &");
    tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
    delay_ms(100);
    DUT_GetCommandAck(Dutrbuf); //clear dut buffer
    UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
    delay_ms(200);
    DutRecvlen=DUT_GetCommandAck(Dutrbuf);

	for (i=0; i<DutRecvlen; i++)  {
		if (Dutrbuf[i] != 0) {
			str = Dutrbuf+i;
			if (strstr(str, test_str)) {
				ret = PASS;
				break;
			}
		}
	}

    if (ret == FAIL) {
    	return FAIL;
    }
    UART_GetReceData(CCP_COMM_setting.fd, ccprbuf);
    if (strstr(ccprbuf, test_str) == NULL) {
		GUI_SendMessage("Fail\r\n");
		return FAIL;
    }
    else {
    	GUI_SendMessage("Pass\r\n");
    	return PASS;
    }
}

static BOOL Currentloop_Passive_test2(void)
{
	char Dutrbuf[1024]={0};
	char ccprbuf[1024]={0};
	const char test_str[] = "the quick brown fox jumps over the lazy dog 0123456789";
	char send_str[100]={0};
	int DutRecvlen=0;
	int i=0;
	int ret=FAIL;
	char *str;

	sprintf(send_str,"%s%s", test_str, "\r");
	DUT_SendComman("cat < /dev/ttyS0 &");
	tcflush(CCP_COMM_setting.fd, TCIOFLUSH);
	delay_ms(100);
	DUT_GetCommandAck(Dutrbuf); //clear dut buffer
	UART_SendFrame(CCP_COMM_setting.fd, send_str, strlen(send_str));
	delay_ms(200);
	DutRecvlen=DUT_GetCommandAck(Dutrbuf);
	for (i=0; i<DutRecvlen; i++)  {
		if (Dutrbuf[i] != 0) {
			str = Dutrbuf+i;
			if (strstr(str, test_str)) {
				ret = PASS;
				break;
			}
		}
	}


	if (ret == FAIL) {
		return FAIL;
	}
	UART_GetReceData(CCP_COMM_setting.fd, ccprbuf);
	if (strstr(ccprbuf, test_str) == NULL) {
		GUI_SendMessage("Fail\r\n");
		return FAIL;
	}
	else {
		GUI_SendMessage("Pass\r\n");
		return PASS;
	}
}

typedef BOOL (*CURRLOOP_FUNC)(void);
typedef struct {
	U8 name[80];

	U32 rly_in_1;
	U32 rly_in_2;
	U32 rs232_rlyin;

	U32 rly_out_1;
	U32 rly_out_2;
	U32 rs232_rlyout;
	CURRLOOP_FUNC Currentloop_Test;

} CURRLOOP_ITEM_T, *P_CURRLOOP_ITEM_T;

const CURRLOOP_ITEM_T Activecurrloop_item[] = {

	// Group 1
	// Current Loop 1
	"from UART5 to CRIND(P300-1,3)",
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_FCT_P302B5_RS232IN,
	Currentloop_Active_test1,

	"from CRIND(P300-1,3 to UART5)",
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_FCT_P302B5_RS232IN,
	Currentloop_Active_test2,

	// Current Loop 2
	"from UART6 to Pump(P300-4,5)",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 	                  RLY_DUMMY, 					RLY_FCT_P302B5_RS232IN,
	Currentloop_Active_test3,

	"from Pump(P300-4,5) to UART6",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 	                  RLY_DUMMY, 					RLY_FCT_P302B5_RS232IN,
	Currentloop_Active_test4,

	 //Current Loop 3
	"from UART7 to PUMP(P303)",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_DUMMY, 	                  RLY_DUMMY, 					RLY_FCT_P302B1_RS232IN,
	Currentloop_Active_test5,

	"from PUMP(P303) to UART7",
	RLY_LP_DUT_P303_1_FCT_P300_5,  RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_DUMMY, 	                   RLY_DUMMY, 					 RLY_FCT_P302B1_RS232IN,
	Currentloop_Active_test6,

	// Current Loop 4
	"from Pump(P300-4,5) to PUMP(P303)",
	RLY_LP_DUT_P300_4_DUT_P303_2,  RLY_LP_DUT_P300_5_DUT_P303_1, RLY_DUMMY,
	RLY_DUMMY, 	                   RLY_DUMMY, 					 RLY_DUMMY,
	Currentloop_Active_test7,
};

const CURRLOOP_ITEM_T Passive_currloop_item[] = {
		// Current Loop 5
   "from Pump(P300-4,5) to PUMP(P303)",
	RLY_LP_DUT_P300_4_FCT_P303_2,  RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_LP_DUT_P303_1_FCT_P300_5,  RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B1_RS232IN,
	Currentloop_Passive_test1,

	"from Pump(P300-4,5) to PUMP(P303)",
	RLY_LP_DUT_P300_4_FCT_P303_2,  RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B5_RS232IN,
	RLY_LP_DUT_P303_1_FCT_P300_5,  RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	Currentloop_Passive_test2,
};

static BOOL TestCurrLoopItem(P_CURRLOOP_ITEM_T currloop)
{
    U8 test_result;

    GUI_SendMessage("%s\r\n", currloop->name);

	RLY_ON(currloop->rly_in_1);
	RLY_ON(currloop->rly_in_2);
	RLY_ON(currloop->rs232_rlyout);

	RLY_ON(currloop->rly_out_1);
	RLY_ON(currloop->rly_out_2);
	RLY_ON(currloop->rs232_rlyin);
	delay_ms(100);

	test_result = currloop->Currentloop_Test();

	RLY_OFF(currloop->rly_in_1);
	RLY_OFF(currloop->rly_in_2);
	RLY_OFF(currloop->rs232_rlyout);

	RLY_OFF(currloop->rly_out_1);
	RLY_OFF(currloop->rly_out_2);
	RLY_OFF(currloop->rs232_rlyin);

	if (test_result == PASS) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
	}

	return test_result;
}


void PowerCycle(void)
{
	PWR_TurnOffDut();
	delay_ms(1000);
	PWR_TurnOnDut();
	delay_ms(200);
}

void TestItem_Impedance(U8 * test_result)
{
	U8 i;
	U8 res;
	int test_num=0;

	*test_result = PASS;
	test_num = sizeof(Impedance_item)/sizeof(VOLT_ITEM_T);

	for (i=0; i<test_num; i++) {
		RLY_ON(24+Impedance_item[i].channel);
		delay_ms(100);
		res = TestVoltages((P_VOLT_ITEM_T)&Impedance_item[i]);
		RLY_OFF(24+Impedance_item[i].channel);

		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		* test_result &= res;
	}
}

void TestItem_PowerOn(U8 * test_result)
{
	U32 curr;

	Modules_Init();
	RLY_ON(RLY_DUT_DEBUG_TX);
	RLY_ON(RLY_DUT_DEBUG_RX);
	PWR_TurnOnDut();
	delay_ms(1000);

    GUI_SendMessage("Test Current\r\n");

	curr = TestCurrent(RLY_24V_PPS, RLY_24V_INPUT, 1);
	if(curr >= CURR_TOT_MIN && curr <= CURR_TOT_MAX) {
		* test_result = PASS;
	}
	else
		* test_result = FAIL;


}

void TestItem_Voltage(U8 * test_result)
{
    U8 i;
    U8 res;
    int test_num=0;

    *test_result = PASS;
    test_num = sizeof(volt_item)/sizeof(VOLT_ITEM_T);

    for(i=0; i<test_num; i++) {
    	res = TestVoltages((P_VOLT_ITEM_T)&volt_item[i]);
		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		*test_result &= res;
    }
}

void TestItem_Reset(U8 * test_result)
{
	int i;

	*test_result = FAIL;
	GUI_SendMessage("Start to test reset switch\r\n");

	for (i=0; i<100; i++) {
		if (TestVoltages(&Reset_item)) {
			*test_result = PASS;
			break;
		}
		delay_ms(100);
	}

	if (*test_result == FAIL) {
		GUI_SendMessage("Failed\r\n");
		return;
	}
	else {
		GUI_SendMessage("Passed\r\n");
	}

	*test_result = Test_ResetLED();
	if (*test_result == PASS) {
		GUI_SendMessage("PASS\r\n");
	}
	else {
		GUI_SendMessage("FAIL\r\n");
	}
}

void TestItem_Battery(U8 * test_result)
{
	*test_result = FAIL;

	RLY_ON(RLY_BATTERY_SWITCH);
	delay_ms(30);

	if (TestVoltages(&Battery_item)) {
		*test_result = PASS;
	}

	RLY_OFF(RLY_BATTERY_SWITCH);
	delay_ms(30);

	if(*test_result == PASS)
		GUI_SendMessage("Passed\r\n");
	else
		GUI_SendMessage("Failed\r\n");
}

void TestItem_UBoot(U8 * test_result)
{
	//int i;
	U8 BootConfig[3]={0};

	GUI_SendMessage("Start to test Boot config\r\n");

	RLY_ON(RLY_RESET); //reset the board
	delay_ms(500);

	EXTIO_ReadByte(1,  &BootConfig[0]);
	EXTIO_ReadByte(9,  &BootConfig[1]);
	EXTIO_ReadByte(17, &BootConfig[2]);

	BootConfig[0] &= 0xff;
	BootConfig[1] &= 0xff;
	BootConfig[2] &= 0x01;

	if ((BootConfig[0]==0x40) && (BootConfig[1]==0x20) && (BootConfig[2]==0x00)) {
		GUI_SendMessage("U-Boot Config passed\r\n");
	}
	else {
		GUI_SendMessage("U-Boot Config failed\r\n");
		*test_result = FAIL;
		return;
	}

	tcflush(DUT_COMM_setting.fd, TCIFLUSH);
	GUI_SendMessage("U-Boot starts\r\n");
	RLY_OFF(RLY_RESET);

	if (UART_CheckStr(DUT_COMM_setting.fd, "autoboot:", 2)) {
		UART_SendFrame(DUT_COMM_setting.fd, "\r", strlen("\r"));
		GUI_SendMessage("at the U-Boot prompt \r\n");
		*test_result = PASS;
	}
	else {
		GUI_SendMessage("Failed\r\n");
		*test_result = FAIL;
	}
}

void TestItem_Program(U8 *test_result)
{
	char cmdBuff[100]={0};
	char AckBuff[1024]={0};
	char verisoncheck[20]={0};
	char portcheck[20]={0};
	char datecheck[20]="2018/01/16";
	char Maccheck[30]={0};

	*test_result = PASS;

	/*sprintf(verisoncheck, "%s%s", "Version: ", eepInfo.head);
	sprintf(cmdBuff, "%s%s", "eep i ", eepInfo.head);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	DUT_SendComman("eep c \"VeriFone\"");
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff, "%s%s", "eep a ", eepInfo.board_name);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep b ", eepInfo.board_version);
	DUT_SendComman(cmdBuff);
	delay_ms(100);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep v ", eepInfo.software_version);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	strcpy(Maccheck, eepInfo.MAC_address+2);
	sprintf(cmdBuff,"%s%s", "eep ", eepInfo.MAC_address);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	datecheck[2]=eepInfo.date[0];
	datecheck[3]=eepInfo.date[1];
	datecheck[5]=eepInfo.date[2];
	datecheck[6]=eepInfo.date[3];
	datecheck[8]=eepInfo.date[4];
	datecheck[9]=eepInfo.date[5];
	sprintf(cmdBuff,"%s%s", "eep d ", eepInfo.date);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	sprintf(portcheck, "%s%s", "#: ", eepInfo.port);
	sprintf(cmdBuff,"%s%s", "eep p ", eepInfo.port);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep n ", eepInfo.serial_num);
	DUT_SendComman(cmdBuff);
	delay_ms(50);

	DUT_SendComman("eep save");

	delay_ms(100);

	DUT_SendComman("reset");
	if (UART_CheckStr(DUT_COMM_setting.fd, "autoboot:", 2)) {
		 UART_SendFrame(DUT_COMM_setting.fd, "\r", strlen("\r"));
		 GUI_SendMessage("at the U-Boot prompt \r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
		*test_result = FAIL;
		return;
	}
	delay_ms(10);
	DUT_SendComman("eep u");
	delay_ms(100);
	DUT_GetCommandAck(AckBuff);

	if (strstr(AckBuff, verisoncheck)==NULL) {
		GUI_SendMessage("Version Failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, "VeriFone")==NULL) {
		GUI_SendMessage("CM Failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, eepInfo.board_name)==NULL) {
		GUI_SendMessage("Board name failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, eepInfo.board_version)==NULL) {
		GUI_SendMessage("Board version failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, eepInfo.software_version)==NULL) {
		GUI_SendMessage("Software version failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, Maccheck)==NULL) {
		GUI_SendMessage("MAC address Failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, portcheck)==NULL) {
		GUI_SendMessage("port Failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, eepInfo.serial_num)==NULL) {
		GUI_SendMessage("Serial number failed\r\n");
		*test_result = FAIL;
	}
	if (strstr(AckBuff, datecheck)==NULL) {
		GUI_SendMessage("Date failed\r\n");
		*test_result = FAIL;
	}*/
	*test_result = DONE;
}
int getDigitFromString(char *str, char *prefix, int *digit)
{
	char *startPos;
	char digit_str[64]={0};
	int digitcnt=0;
	startPos=strstr(str, prefix);
	if (startPos == NULL) {
		return FALSE;
	}
	startPos+=strlen(prefix)-1;
	while(*startPos++) {
		if (*startPos>='0'&&*startPos<='9') {
			digit_str[digitcnt++]=*startPos;
		}
		else {
			if (digitcnt > 0) {
				*digit=atoi(digit_str);
				return TRUE;
			}
		}
	}
	return FALSE;
}

typedef struct {
	char title[48];
	char command[48];
	char verifystr[24];
}COMMAND_ITEM_T;

COMMAND_ITEM_T KL82Volt[] = {
		"vin 0", "omnia2k82 vin 0", "VIN = ",
		"vin 1", "omnia2k82 vin 1", "VIN = ",
};

void TestItem_KL82(U8 * test_result)
{
	int i;
	char cmdBuff[DUT_ACK_BUFF_SIZE]={0};
	int voltage=0;
	int input_voltage=0;
	int voltageMax=0;
	int voltageMin=0;

	*test_result=PASS;
	GUI_SendMessage("Start to test KL82\r\n");

	input_voltage=getVoltage(RLY_24V_INPUT); //

	voltageMax = input_voltage*105/100;
	voltageMin = input_voltage*95/100;
	DUT_SendComman("omnia2k82 vin 0");
	DUT_SendComman("omnia2k82 vin 0");
	delay_ms(100);

	for (i=0; i<2; i++) {

		GUI_SendMessage(KL82Volt[i].title);

		DUT_SendComman(KL82Volt[i].command);
		delay_ms(200);
		DUT_GetCommandAck(cmdBuff);

		if (!getDigitFromString(cmdBuff, "VIN = ", &voltage)) {
				GUI_SendMessage("FAIL\r\n");
		}
		GUI_SendMessage("vin=%dmV\r\n",voltage);
		if (voltage>voltageMin && voltage<voltageMax) {
			GUI_SendMessage("PASS\r\n");
		}
		else {
			*test_result=FAIL;
			GUI_SendMessage("FAIL\r\n");
		}
		memset(cmdBuff, 0, DUT_ACK_BUFF_SIZE);
	}

	/*GUI_SendMessage("vin 0\r\n");
	DUT_SendComman("omnia2k82 vin 0");
	delay_ms(200);
	DUT_GetCommandAck(cmdBuff);
	if (!getDigitFromString(cmdBuff, "VIN = ", &voltage)) {
		GUI_SendMessage("FAIL\r\n");
	}
	GUI_SendMessage("vin=%dmV\r\n",voltage);

	if (voltage>voltageMin && voltage<voltageMax) {
		GUI_SendMessage("PASS\r\n");
	}

	DUT_SendComman("omnia2k82 vin 1");
	delay_ms(200);
	DUT_GetCommandAck(cmdBuff);

	if (!getDigitFromString(cmdBuff, "VIN = ", &voltage)) {
			*test_result = PASS;
			GUI_SendMessage("FAIL\r\n");
	}
	GUI_SendMessage("vin=%dmV\r\n",voltage);
	if (voltage>voltageMin && voltage<voltageMax) {
			*test_result = PASS;
			GUI_SendMessage("PASS\r\n");
	}*/
}

void TestItem_LED(U8 *test_result)
{
	U8 i;
	U8 res;
	int test_num=0;

	*test_result = PASS;
	GUI_SendMessage("Start to test LED\r\n");
	test_num = sizeof(LED_volt_item)/sizeof(VOLT_ITEM_T);

	for (i=0; i<test_num; i++) {
		res = TestVoltages((P_VOLT_ITEM_T)&LED_volt_item[i]);
		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		*test_result &= res;
	}
	if (*test_result == FAIL) {
		return;
	}
	*test_result=Test_LED();
	if (*test_result == PASS) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
	}
}

void TestItem_Contrlled(U8 *test_result)
{
	int i;
	int res;
	*test_result = PASS;

	GUI_SendMessage("Start to test regulator\r\n");

	DUT_SendComman("omnia_power");

	if (!UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1)) {
		GUI_SendMessage("omnia_power err\r\n");
	}

	for (i=0; i<sizeof(cvolt_item)/sizeof(VOLT_ITEM_T); i++) {
		res = TestVoltages((P_VOLT_ITEM_T)&cvolt_item[i]);
		if (res == PASS) {
			GUI_SendMessage("Passed\r\n");
		}
		else {
			GUI_SendMessage("Failed\r\n");
			*test_result = FAIL;
		}
	}

	if (*test_result == FAIL) {
		return;
	}
	*test_result=Test_RegulatorLED();
	if (*test_result == PASS) {
		GUI_SendMessage("PASS\r\n");
	}
	else {
		GUI_SendMessage("Regulator LED failed\r\n");
	}
}

void TestItem_CurrSrc(U8 * test_result)
{
	U32 curr=0;

    *test_result = PASS;
    GUI_SendMessage("Set to 35mA Current Source\r\n");

    RLY_ON(RLY_PUMP_LOAD_SWITCH);
    delay_ms(100);

    DUT_SendComman("omnia_current1");
    if(!UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1)) {
    	GUI_SendMessage("omnia_current1 err\r\n");
    }
	curr = TestCurrent(RLY_PUMP2_VOLT, RLY_PUMP1_VOLT, 10);

	if (curr >= CURR_SRC_35MIN && curr <= CURR_SRC_35MAX) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
		*test_result = FAIL;
	}

    GUI_SendMessage("Set to 45mA Current Source\r\n");

    DUT_SendComman("omnia_current2");
    if(!UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1)) {
        GUI_SendMessage("omnia_current1 err\r\n");
    }
    curr = TestCurrent(RLY_PUMP2_VOLT, RLY_PUMP1_VOLT, 10);

	RLY_OFF(RLY_PUMP_LOAD_SWITCH);

	if (curr >= CURR_SRC_45MIN && curr <= CURR_SRC_45MAX) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
		*test_result = FAIL;
	}
}

void TestItem_LCDDismount(U8 * test_result)
{
	char cmdBuff[DUT_ACK_BUFF_SIZE]={0};
	*test_result = PASS;
	GUI_SendMessage("Start to test LCD Dismount\r\n");

	GUI_SendMessage("P1&P2 connect with a loopback\r\n");
	RLY_ON(RLY_P1_LOOPBACK); //P1&P2 connect pin4 to GND.
	RLY_ON(RLY_P2_LOOPBACK);
	RLY_ON(RLY_BATTERY_SWITCH);
	delay_ms(20);
	DUT_SendComman("omnia_tamper clear");
	DUT_SendComman("omnia_tamper clear");
	DUT_SendComman("omnia_tamper");
	delay_ms(100);
	DUT_GetCommandAck(cmdBuff);

	if (strstr(cmdBuff, "Tamper Side A: Clear")) {
		GUI_SendMessage("Side A Clear\r\n");
	}
	else {
		GUI_SendMessage("Side A fail\r\n");
		*test_result = FAIL;
		return;
	}

	if (strstr(cmdBuff, "Tamper Side B: Clear")) {
			GUI_SendMessage("Side B Clear\r\n\r\n");
	}
	else {
		GUI_SendMessage("Side B fail\r\n");
		*test_result = FAIL;
		return;
	}

	GUI_SendMessage("Power cycle, wait for 10 seconds\r\n");
	PWR_TurnOffDut(); //power cycle
	sleep(11);
	tcflush(DUT_COMM_setting.fd, TCIFLUSH);
	PWR_TurnOnDut();

	if (UART_CheckStr(DUT_COMM_setting.fd, "autoboot:", 1)) {
		 UART_SendFrame(DUT_COMM_setting.fd, "\r", sizeof("\r"));
	}
	memset(cmdBuff, 0, DUT_ACK_BUFF_SIZE);
	DUT_SendComman("omnia_tamper");
	DUT_SendComman("omnia_tamper");
	delay_ms(100);
	DUT_GetCommandAck(cmdBuff);

	if (strstr(cmdBuff, "Tamper Side A: Clear")) {
		GUI_SendMessage("Side A Clear\r\n");
	}
	else {
		GUI_SendMessage("Side A fail\r\n");
		*test_result = FAIL;
		return;
	}

	if (strstr(cmdBuff, "Tamper Side B: Clear")) {
		GUI_SendMessage("Side B Clear\r\n\r\n");
	}
	else {
		GUI_SendMessage("Side B fail\r\n");
		*test_result = FAIL;
		return;
	}

	GUI_SendMessage("Remove the lookback\r\n");
	RLY_OFF(RLY_P1_LOOPBACK); //
	RLY_OFF(RLY_P2_LOOPBACK);
	delay_ms(300);

	memset(cmdBuff, 0, DUT_ACK_BUFF_SIZE);
	DUT_SendComman("omnia_tamper");
	delay_ms(100);
	DUT_GetCommandAck(cmdBuff);

	if (strstr(cmdBuff, "Tamper Side A: Set")) {
		GUI_SendMessage("Side A Set\r\n");
	}
	else {
		GUI_SendMessage("Side A fail\r\n");
		*test_result = FAIL;
		return;
	}

	if (strstr(cmdBuff, "Tamper Side B: Set")) {
		GUI_SendMessage("Side B Set\r\n\r\n");
	}
	else {
		GUI_SendMessage("Side B fail\r\n");
		*test_result = FAIL;
		return;
	}
}

void TestItem_LCDVideo(U8 * test_result)
{
	RLY_ON(RLY_3V3_LCD);
	*test_result = PASS;
}

void TestItem_Storage(U8 * test_result)
{

	char AckBuff[DUT_ACK_BUFF_SIZE]={0};

	*test_result = PASS;
	GUI_SendMessage("Start to test storage\r\n");

	DUT_SendComman("omnia_storage");
	delay_ms(500);
	DUT_GetCommandAck(AckBuff);
	/*if (strstr(AckBuff, "SPI-NOR OK") != NULL) {
		GUI_SendMessage("SPI NOR PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("SPI NOR FAIL\r\n");
	}*/

	if (strstr(AckBuff, "eMMC OK") != NULL) {
			GUI_SendMessage("eMMC PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("eMMC FAIL\r\n");
	}

	if (strstr(AckBuff, "SRAM STORE OK") != NULL) {
			GUI_SendMessage("SRAM STORE PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("SRAM STORE FAIL\r\n");
	}

	memset(AckBuff, 0, DUT_ACK_BUFF_SIZE);
	DUT_SendComman("omnia_storage");
	delay_ms(500);
	DUT_GetCommandAck(AckBuff);

	if (strstr(AckBuff, "SRAM - PERSIST OK") != NULL) {
		GUI_SendMessage("SRAM PERSIST PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("SRAM STORE FAIL\r\n");
	}
}

COMMAND_ITEM_T temp_sensor[] = {
		"DTT1", "omnia_temp", "DTT1:",
		"DTT2", "omnia_temp", "DTT2:",
		"DTT3", "omnia_temp", "DTT3:",
};

void TestItem_TemperatureSensor(U8 * test_result)
{
	int i=0;
	int dtt[3]={0};
	char AckBuff[512]={0};

	*test_result = PASS;
	GUI_SendMessage("Start to test temperature sensor\r\n");
	DUT_SendComman("omnia_temp");
	DUT_SendComman("omnia_temp");
	delay_ms(200);
	DUT_GetCommandAck(AckBuff);

	for (i=0; i< 3; i++) {
		if (!getDigitFromString(AckBuff, temp_sensor[i].verifystr, &dtt[i])) {
			GUI_SendMessage("FAIL\r\n");
		}

		GUI_SendMessage("%s=%d C\r\n",temp_sensor[i].title,  dtt[i]);
		if (dtt[i] >= 15 && dtt[i]<=35) {
			GUI_SendMessage("PASS\r\n");
		}
		else {
			*test_result = FAIL;
			GUI_SendMessage("FAIL\r\n");
		}
	}
}

typedef struct {
	char title[24];
	int channel;
}ETH_ITEM_T;

ETH_ITEM_T Ethernet_Item[8] = {
		{"P304A", 41},
		{"J6101", 42},
		{"J6102", 43},
		{"J6103", 44},
		{"J6104", 45},
		{"J6201", 46},
		{"J6202", 47},
		{"J6203", 48},
};

void TestItem_Ethernet(U8 * test_result)
{
	int i;

	*test_result = PASS;
	GUI_SendMessage("Start to test Ethernet\r\n");
	DUT_SendComman("setenv ipaddr 192.168.0.254");

	for (i=1; i<8; i++) {
		GUI_SendMessage("Start to test %s\r\n", Ethernet_Item[i].title);

		RLY_ON(Ethernet_Item[i].channel);
		delay_ms(100);

		DUT_SendComman("ping 192.168.0.133");
		if (UART_CheckStr(DUT_COMM_setting.fd, "host 192.168.0.133 is alive", 15)) {
			GUI_SendMessage("%s test successful\r\n", Ethernet_Item[i].title);
		}
		else {
			GUI_SendMessage("%s test fail\r\n", Ethernet_Item[i].title);
			*test_result = FAIL;
			return;
		}
		RLY_OFF(Ethernet_Item[i].channel);
	}
	GUI_SendMessage("Ethernet test PASS\r\n");
}

void TestItem_LinuxBoot(U8 * test_result)
{
	GUI_SendMessage("Start to linux boot\r\n");
	DUT_SendComman("boot");

	if (!UART_CheckStr(DUT_COMM_setting.fd, "Omnia login:", 12)) {
		//*test_result = FAIL;
		GUI_SendMessage("Can't check login\r\n");
	}

	DUT_SendComman("root");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "#", 1)) {
		GUI_SendMessage("Can't login\r\n");
		*test_result = FAIL;
		return;
	}

	DUT_SendComman("cd board_test/");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "#", 1)) {
		GUI_SendMessage("Can't enter board_test\r\n");
		*test_result = FAIL;
		return;
	}

	*test_result = PASS;
}

void TestItem_RTC(U8 *test_result)
{
	GUI_SendMessage("Start to test RTC\r\n");

	RLY_ON(RLY_BATTERY_SWITCH);
	DUT_SendComman("./rtc.sh");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "MMDDhhmm[[YY]YY][.ss]", 5)) {
		GUI_SendMessage("Can't exec rtc.sh\r\n");
		*test_result = FAIL;
		return;
	}

	DUT_SendComman("091207372017.25");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "hwclock is\r\nTue Sep 12 07:37:25 2017", 3)) {
		GUI_SendMessage("RTC test failed\r\n");
		*test_result = FAIL;
		return;
	}
	RLY_OFF(RLY_BATTERY_SWITCH);
	GUI_SendMessage("RTC test PASS\r\n");
	*test_result = PASS;

}

void TestItem_USB(U8 *test_result)
{
	GUI_SendMessage("Start to test USB\r\n");

	DUT_SendComman("./usb.sh");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "press any key to continue", 10)) {
		GUI_SendMessage("Can't exec usb.sh\r\n");
		*test_result = FAIL;
		return;
	}

	DUT_SendComman("");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "USB test successful!", 3)) {
		GUI_SendMessage("USB test failed\r\n");
		*test_result = FAIL;
		return;
	}

	*test_result = PASS;
}

void TestItem_Audio(U8 * test_result)
{
	int ret;
	char cmd[10]={0};
	*test_result = PASS;
	GUI_SendMessage("Start to test Audio\r\n");
	DUT_SendComman("./audio.sh");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "press return to continue", 1)) {

		GUI_SendMessage("Can't exec audio2.sh\r\n");
		*test_result = FAIL;
		return;
	}
	DUT_SendComman("");
	GUI_SendMessage("P6901 test\r\n");
	RLY_ON(RLY_DUT_SPEAKKE_P6901_1);
	RLY_ON(RLY_DUT_SPEAKKE_P6901_2);
	delay_ms(100);
	ret=Audio_Test_WhiteNoise(30, 70);
	RLY_OFF(RLY_DUT_SPEAKKE_P6901_1);
	RLY_OFF(RLY_DUT_SPEAKKE_P6901_2);
	if (ret == PASS) {
		GUI_SendMessage("PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("FAIL\r\n");
	}
	GUI_SendMessage("P6902 test\r\n");
	RLY_ON(RLY_DUT_SPEAKKE_P6902_1);
	RLY_ON(RLY_DUT_SPEAKKE_P6902_2);
	delay_ms(100);
	ret=Audio_Test_WhiteNoise(30, 70);
	RLY_OFF(RLY_DUT_SPEAKKE_P6902_1);
	RLY_OFF(RLY_DUT_SPEAKKE_P6902_2);
	if (ret == PASS) {
		GUI_SendMessage("PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("FAIL\r\n");
	}
	cmd[0]=0x03;//ctrl+c
	DUT_SendComman(cmd);
}

BOOL TestActiveModeCurrectLoop(void)
{
	int i=0;
	int res;
	int ret=PASS;

	GUI_SendMessage("Active mode\r\n");

	DUT_SendComman("memtool 20A0000=7800FA0F");

	for (i=0; i<sizeof(Activecurrloop_item)/sizeof(CURRLOOP_ITEM_T); i++) {
		res = TestCurrLoopItem((P_CURRLOOP_ITEM_T)&Activecurrloop_item[i]);
		ret &= res;
    }
	return ret;
}

BOOL TestPassiveModeCurrectLoop(void)
{
	int i=0;
	int res;
	int ret=PASS;

	GUI_SendMessage("Passive mode\r\n");

	DUT_SendComman("memtool 20A0000=7000FB0F");
	//delay_ms(500);
	for (i=0; i<sizeof(Passive_currloop_item)/sizeof(CURRLOOP_ITEM_T); i++) {
		res = TestCurrLoopItem((P_CURRLOOP_ITEM_T)&Passive_currloop_item[i]);
		ret &= res;
	}
	return ret;
}

void TestItem_CurrLoop(U8 * test_result)
{
    GUI_SendMessage("Start to test UART/current loop\r\n");

    *test_result = TestActiveModeCurrectLoop();
    if (*test_result == FAIL) {
    	return;
    }
    *test_result = TestPassiveModeCurrectLoop();
}



void TestItem_PowerOff(U8 * test_result)
{
    PWR_TurnOffDut();
    //LOGFILE_Write();
    Modules_Init();
    * test_result = DONE;
}

void TestItem_AutoPowerOff(U8 * test_result)
{
	* test_result = DONE;
}

char Assm[5];

void TestItem_Assembly(U8 * test_result)
{
	if(strstr(Assm, (char * )"A001"))
		assembly_type = ASSEMBLY_A001;
	else if(strstr(Assm, (char * )"A002"))
		assembly_type = ASSEMBLY_A002;
	else if(strstr(Assm, (char * )"A101"))
		assembly_type = ASSEMBLY_A101;
	else
		printf("error\r\n");

	printf("%s\r\n",Assm);
    * test_result = DONE;
}

typedef void (*TEST_FUNC)(U8 * test_result);

typedef struct
{
	char TestIdChar;
	TEST_FUNC TestFunc;

} TEST_ID, *P_TEST_ID;

const TEST_ID TestIdTab[] =
{
	{'I',  TestItem_Impedance},
	{'N',  TestItem_PowerOn},
	{'V',  TestItem_Voltage},
	{'W',  TestItem_Reset},
	{'B',  TestItem_Battery},
	{'H',  TestItem_UBoot},
	{'P',  TestItem_Program},
	{'K',  TestItem_KL82},
	{'O',  TestItem_Contrlled},
	{'L',  TestItem_LED},
	{'D',  TestItem_LCDDismount},
	{'Q',  TestItem_LCDVideo},
	{'T',  TestItem_CurrSrc},
	{'S',  TestItem_Storage},
	{'M',  TestItem_TemperatureSensor},
	{'E',  TestItem_Ethernet},
	{'X',  TestItem_LinuxBoot},
	{'R',  TestItem_RTC},
	{'U',  TestItem_USB},
	{'A',  TestItem_Audio},
	{'C',  TestItem_CurrLoop},
	{'F',  TestItem_PowerOff},	// Phred
	{'Y',  TestItem_AutoPowerOff},	// Phred
};

#define TEST_TIMES	20

void dbg(void)
{
	U8 i;
	U8 test_result;
	U8 id_sum = sizeof(TestIdTab)/sizeof(TEST_ID);

	for (i=0;i<id_sum;i++) {
		(TestIdTab[i].TestFunc)(&test_result);     	// Testing.
		ReportResult(test_result);
		delay_ms(500);
	}
}

#define GUI_CMD_FRAME_MAX 100

void GUI_Process(void)
{
	U8 i=0;
	U8 len = 0;
	U8 test_result;
	U8 rcnt=0;
	U8 rbuf[255] = {0};
	U8 cmd[GUI_CMD_FRAME_MAX]={0};
	int is_upp;

	U8 id_sum = sizeof(TestIdTab)/sizeof(TEST_ID);

	if (UART_GetLine(GUI_COMM_setting.fd, rbuf, (U8 *)&rcnt) == FALSE) {
		return;
	}

	printf("Got GUI '%s'\n", rbuf);

	for (i = 0; i < rcnt; i++) {
		is_upp = isupper(rbuf[i]);
//    	printf("buff%d=%d,is %d\n", rcnt-i, rbuf[rcnt-i], is_upp);

        if (is_upp) {
            len = rcnt-i;
            break;
        }
	}

	if (len == 0 || len > GUI_CMD_FRAME_MAX) { //Buffer received error, it has been too long
		printf("Command is error\n");
		return;
	}
	//strcpy(cmd,(rbuf+i));
	memcpy(cmd, &rbuf[i], len);

	for (i=0; i<id_sum; i++)
	{
		if (TestIdTab[i].TestIdChar == cmd[0])    // Get the sum of testing ID from the TestIdTable.
		{
			if (cmd[0] == 'P') {
				parseProgrammingParameter((char *)cmd);
			}

			(TestIdTab[i].TestFunc)(&test_result);     	// Testing.

			ReportResult(test_result);

			return;
		}
	}

	printf("Command is error\n");
}

