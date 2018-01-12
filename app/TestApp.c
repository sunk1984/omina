

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
	char board_name[48];
	char board_version[48];
	char software_version[48];
	char MAC_address[48];
	char serial_num[48];
}EEPINFO;

EEPINFO eepInfo;

#define ASSEMBLY_A001 1
#define ASSEMBLY_A002 2
#define ASSEMBLY_A101 3

U8 assembly_type;
int input_voltage=0;

void GUI_SendMessage(char *lpszFormat, ...)
{
    U8 txBuff[100] = {0};
    va_list fmtList;

    va_start( fmtList, lpszFormat );
    vsprintf( (char *)txBuff, lpszFormat, fmtList );
    va_end( fmtList );

	printf( (char *)txBuff);
    LOGFILE_AddItem(txBuff);
	UART_SendFrame(GUI_COMM_setting.fd, txBuff, strlen((char * )txBuff));
}

void Modules_Config(void)
{
    ComInit(&MRK_COMM_setting);	// fd:3
    ComInit(&GUI_COMM_setting);	// fd:4
	ComInit(&DUT_COMM_setting);	// fd:5
//  ComInit(&LED_COMM_setting);

    //RLY_SetCommonMode(1);   //Configure RLY board 1 to Common mode
    //RLY_SetCommonMode(2);   //Configure RLY board 2 to Common mode
    //RLY_SetCommonMode(3);   //Configure RLY board 3 to Common mode

    GUI_SendMessage("Fixture is ready\r\n");
}

void Modules_Init(void)
{
    RLY_Clear(RELAY_BOARD_MAX);

	EXTIO_ConfigureByteDirction(1, IO_INPUT);	//
	EXTIO_ConfigureByteDirction(9, IO_INPUT);
	EXTIO_ConfigureByteDirction(17, IO_INPUT);

	PWR_TurnOffDut();
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

#define CURR_TOT_MAX	450	/////////////////////////////
#define CURR_TOT_MIN	100	/////////////////////////////

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
	"24V Measurement",       "MTP22", 22000, 26000, 2,
	"3.3V Measurement",      "MTP11", 3250,  3350,  3,
	"5.0V_PS Measurement",   "MTP12", 4750,  5250,  4,
	"VDDARM_IN Measurement", "MTP5",  1227,  1270,  5,
	"VDDSOC_IN Measurement", "MTP8",  1227,  1270,  6,
	"1.5V Measurement",      "MTP21", 1470,  1530,  7,
	"1.1V Measurement",      "MTP9",  1070,  1130,  8,
	"1.8V  Measurement",     "MTP7",  1770,  1830,  9,
	"5.0V Measurement",      "MTP13", 4750,  5250,  10,
	"1.35V Measurement",     "MTP6",  1330,  1370,  11,	
	"DDR_VTT Measurement",   "MTP27", 665,   685,   12,
	"DDR_VREF Measurement",  "MTP26", 665,   685,   13,	
//	"5.0V_USB Measurement",  "MTP14", 4750,  5250,  RLY_1V8C_VOLT,
//	"7.5V Measurement",      "MTP10", 7250,  7750,  RLY_1V8X_VOLT,	
};

const VOLT_ITEM_T Impedance_item[] = {

	"24V Measurement",       "MTP22", 200, 1000,  2,
	"3.3V Measurement",      "MTP11", 200, 1000,  3,
	"5.0V_PS Measurement",   "MTP12", 200, 1000,  4,
	"VDDARM_IN Measurement", "MTP5",  200, 1000,  5,
	"VDDSOC_IN Measurement", "MTP8",  200, 1000,  6,
	"1.5V Measurement",      "MTP21", 200, 1000,  7,
	"1.1V Measurement",      "MTP9",  200, 1000,  8,
	"1.8V  Measurement",     "MTP7",  200, 1000,  9,
	"5.0V Measurement",      "MTP13", 200, 1000,  10,
	"1.35V Measurement",     "MTP6",  200, 1000,  11,
	"DDR_VTT Measurement",   "MTP27", 200, 1000,  12,
	"DDR_VREF Measurement",  "MTP26", 200, 1000,  13,
	"5.0V_USB Measurement",  "MTP14", 200, 1000,  14,
	"7.5V Measurement",      "MTP10", 200, 1000,  15,
};

const VOLT_ITEM_T cvolt_item[] = {

	{"5.0V_USB Measurement",  "MTP14", 4750,  5250,  14},
	{"7.5V Measurement",      "MTP10", 7250,  7750,  15}
};

VOLT_ITEM_T Reset_item = {"Reset switch",  "MTP3", 0,  200,  16};
VOLT_ITEM_T Battery_item = {"Battery",  "MTP28", 3200,  3500,  17};

int getVoltage(int channle)
{
	U32 volt=0;
	RLY_ON(channle);
	delay_ms(30);
	ADC_GetVolt(&volt);
	RLY_OFF(channle);

	return volt;
}

static BOOL TestVoltages(P_VOLT_ITEM_T pitem)
{
	U32 volt;

	//RLY_ON(pitem->channel);
	//delay_ms(30);
    //ADC_GetVolt(&volt);
	//RLY_OFF(pitem->channel);
	//delay_ms(30);

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
	//U32 resis;  //resis range: 0.1 OHm to 99 OHm
	U32 curr;

    //RLY_ON(rly_bef_res);
	//delay_ms(50);
    //ADC_GetVolt(&volt_former);
	//RLY_OFF(rly_bef_res);
	volt_former=getVoltage(rly_bef_res);
	Dprintf("former=%2d.%03dV\n", volt_former/1000, volt_former%1000);
	delay_ms(50);

	//RLY_ON(rly_aft_res);
	//delay_ms(50);
    //ADC_GetVolt(&volt_latter);
	//RLY_OFF(rly_aft_res);
	volt_latter=getVoltage(volt_latter);
	Dprintf("latter=%2d.%03dV\n", volt_latter/1000, volt_latter%1000);

    //resis = 1; // resis = 5 Ohm
    Dprintf("res=%d Ohm\n", resis);

    volt_diff = volt_former - volt_latter;
    curr = volt_diff/resis;
    GUI_SendMessage("%2d.%03dA\r\n", curr/1000, curr%1000);

	return curr;
}



int parseProgrammingParameter(char *cmd)
{
	char *pStr;
	char *line;

	pStr=cmd;
	while( (line=strtok(pStr, " ")) != NULL) {
		if (strstr(line, "a=")) {
			strcpy(eepInfo.board_name, (line+2));
		}
		if (strstr(line, "b=")) {
			strcpy(eepInfo.board_version, (line+2));
		}
		if (strstr(line, "v=")) {
			strcpy(eepInfo.software_version, (line+2));
		}
		if (strstr(line, "X=")) {
			strcpy(eepInfo.MAC_address, (line+2));
		}
		if (strstr(line, "n=")) {
			strcpy(eepInfo.serial_num, (line+2));
		}
		pStr += strlen(line)+1;
	}
	return TRUE;
}


static BOOL RS232_loop_test(void)
{
	volatile U8 timeout=0;
    U32 rcnt=0;
    char rbuf[500];
    char tx_str[255];
    const char test_str[] = "The quick brown fox jumps over the lazy dog";

	read(DUT_COMM_setting.fd, rbuf, 500);

	sprintf(tx_str, "%s\r", test_str);
	UART_SendFrame(DUT_COMM_setting.fd, (U8 *)tx_str, strlen((char * )tx_str));
	printf("Put: \"%s\" ", test_str);

    while(1) {
		if(timeout++ > 200)
			break;

		if(UART_GetLine(DUT_COMM_setting.fd, (U8 * )rbuf, (U8 * )&rcnt)) {
			printf("Got \"%s\" \n", test_str);
			if(strstr((char *)rbuf, (char * )test_str)) {
				printf("loop ok\n");
				delay_ms(20);
				return TRUE;
			}
		}
		delay_ms(1);
    }

	return FALSE;
}

typedef struct {
	U8 name[80];

	U32 rly_in_1;
	U32 rly_in_2;
	U32 rs232_rlyin;

	U32 rly_out_1;
	U32 rly_out_2;
	U32 rs232_rlyout;

} CURRLOOP_ITEM_T, *P_CURRLOOP_ITEM_T;

const CURRLOOP_ITEM_T currloop_item[] = {

	// Group 1
	// Current Loop 1
	"from UART5 to CRIND(P300-1,3)",
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_DUT_P302A1_RS232IN,

	"from CRIND(P300-1,3 to UART5)",
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_DUT_P302B1_RS232IN,

	// Current Loop 2
	"from UART6 to Pump(P300-4,5)",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUT_LOAD_10R_IN_PUMP, 	RLY_DUMMY, 					RLY_DUT_P302A5_RS232IN,

	"from Pump(P300-4,5) to UART6",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUT_LOAD_10R_IN_PUMP, 	RLY_DUMMY, 					RLY_DUT_P302B5_RS232IN,

	// Current Loop 3
	"from UART7 to PUMP(P303)",
	RLY_LP_DUT_P333_3_FCT_P303_2,  RLY_LP_DUT_P333_4_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUT_LOAD_10R_IN_PUMP, 	RLY_DUMMY, 					RLY_DUT_P302A5_RS232IN,

	"from to UART7",
	RLY_LP_DUT_P333_3_FCT_P303_2,  RLY_LP_DUT_P333_4_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUT_LOAD_10R_IN_PUMP, 	RLY_DUMMY, 					RLY_DUT_P302B5_RS232IN,

	// Current Loop 4
	"from Pump(P300-4,5) to PUMP(P303)",
	RLY_LP_DUT_P333_3_FCT_P303_2,  RLY_LP_DUT_P333_4_FCT_P303_1, RLY_FCT_P302B4_RS232OUT,
	RLY_DUT_LOAD_10R_IN_PUMP, 	RLY_DUMMY, 					RLY_DUT_P302B5_RS232IN,

	// Group 2
	"No jump on J4, from P302A-2 to P300-1&3",
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_DUT_P302A2_RS232OUT,
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B5_RS232IN,

	"No jump on J4, from P302B-2 to P300-1&3",
	RLY_DUMMY, 					RLY_DUMMY,					RLY_DUT_P302B2_RS232OUT,
	RLY_LP_DUT_P300_1_FCT_P303_2, RLY_LP_DUT_P300_3_FCT_P303_1, RLY_FCT_P302B5_RS232IN,

	// Current Loop 5
	"No jump on J4, from P303-1&2 to P300-4&5",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B5_RS232IN,

	// Current Loop 6
	"No jump on J4, from P303-1&2 to P333-3&4",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_LP_DUT_P333_3_FCT_P303_2, RLY_LP_DUT_P333_4_FCT_P303_1, RLY_FCT_P302B5_RS232IN,

	// Current Loop 7
	"No jump on J4, from P303-1&2 to P302A-5",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY,					RLY_DUT_P302A5_RS232IN,

	"No jump on J4, from P303-1&2 to P302B-5",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_DUMMY, 					RLY_DUMMY, 					RLY_DUT_P302B5_RS232IN,

	// Current Loop 8
	"Jump on J4, from P300-1&3 to P302A-1",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B2_RS232OUT,
	RLY_JUMPER_J4_MODE, 			RLY_DUMMY, 					RLY_DUT_P302A1_RS232IN,

	"Jump on J4, from P300-1&3 to P302B-1",
	RLY_LP_DUT_P300_4_FCT_P303_2, RLY_LP_DUT_P300_5_FCT_P303_1, RLY_FCT_P302B2_RS232OUT,
	RLY_JUMPER_J4_MODE, 			RLY_DUMMY, 					RLY_DUT_P302B1_RS232IN,

	// Current Loop 9
	"Jump on J4, from P303-1&2 to P302A-5",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_JUMPER_J4_MODE, 			RLY_DUT_A_SIDE_LINK,			RLY_DUT_P302A5_RS232IN,

	"Jump on J4, from P303-1&2 to P302B-5",
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B2_RS232OUT,
	RLY_JUMPER_J4_MODE, 			RLY_DUT_B_SIDE_LINK, 		RLY_DUT_P302B5_RS232IN,

	// Current Loop 10
	"Jump on J4, from P302A-4 to P303-1&2",
	RLY_JUMPER_J4_MODE, 			RLY_DUMMY, 					RLY_DUT_P302A4_RS232OUT,
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B1_RS232IN,

	"Jump on J4, from P302B-4 to P303-1&2",
	RLY_JUMPER_J4_MODE, 			RLY_DUMMY, 					RLY_DUT_P302B4_RS232OUT,
	RLY_LP_DUT_P303_1_FCT_P300_5, RLY_LP_DUT_P303_2_FCT_P300_4, RLY_FCT_P302B1_RS232IN,
};

typedef struct {
	U8 name[80];

	U32 rly_in_1;
	U32 rly_in_2;
	U32 rs232_rlyin;

	U32 rly_out_1;
	U32 rly_out_2;
	U32 rs232_rlyout;

	U8 type;
	U8 io_ch;

} CPLD_TRUTH_TABLE_ITEM_T, *P_CPLD_TRUTH_TABLE_ITEM_T;

#define CPLD_TYPE_COMMON	0
#define CPLD_TYPE_IS_1		1
#define CPLD_TYPE_MODE		2
#define CPLD_TYPE_GSOM_PUMP	 3

#define PUMP_TO_GSOM_A		0
#define PUMP_TO_GSOM_B	 	1

const CPLD_TRUTH_TABLE_ITEM_T CPLD_truth_table[] = {

};

static BOOL RS232_CPLD_cts_test(U8 io_bit)
{
	return TRUE;
}

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

	test_result = RS232_loop_test();

	RLY_OFF(currloop->rly_in_1);
	RLY_OFF(currloop->rly_in_2);
	RLY_OFF(currloop->rs232_rlyout);

	RLY_OFF(currloop->rly_out_1);
	RLY_OFF(currloop->rly_out_2);
	RLY_OFF(currloop->rs232_rlyin);

	return test_result;
}



#define IO_READ_TIMES	10

static BOOL GetHighLevel(U8 iob_ch)
{
	U8 i;
    U8 readBit = 0;

	EXTIO_ConfigureBitDirction(iob_ch, IO_INPUT);

	for(i=0; i<IO_READ_TIMES; i++) {
		EXTIO_ReadBit(iob_ch, &readBit);
		delay_ms(20);
		if(readBit == 0)
			break;
	}
	if(i == IO_READ_TIMES)
		return TRUE;
	else
		return FALSE;
}

#define IOB_ETHMUX_START		1

void Set_EtherMuxIO(U8 port_src, U8 port_dest)
{
	U32 oe_data;	// OE, bit 0,2,4,6,8..., 20, 22
	U32 sw_data;	// SW, bit 1,3,5,7,9..., 21, 23
	U32 out_data;	// 24 bits

	oe_data = (1 << (port_src<<1)) | (1 << (port_dest<<1));
	oe_data = ~oe_data;

	sw_data = 1 << ((port_src<<1)+1);
	sw_data = ~sw_data;

	out_data = (oe_data & sw_data)&0xFFFFFF;

//    EXTIO_ConfigureByteDirction(IOB_ETHMUX_START+16, IO_OUTPUT);
//    EXTIO_ConfigureByteDirction(IOB_ETHMUX_START+8, IO_OUTPUT);
//	EXTIO_ConfigureByteDirction(IOB_ETHMUX_START, IO_OUTPUT);

//    EXTIO_WriteByte(IOB_ETHMUX_START+16, (U8)(out_data>>16));
	printf("IOB h :%02x\n", (U8)(out_data>>16));
//    EXTIO_WriteByte(IOB_ETHMUX_START+8, (U8)(out_data>>8));
	printf("IOB m :%02x\n", (U8)(out_data>>8));
//    EXTIO_WriteByte(IOB_ETHMUX_START, (U8)(out_data));
	printf("IOB l :%02x\n", (U8)(out_data));
}



void PowerCycle(void)
{
	PWR_TurnOffDut();
	delay_ms(1000);
	PWR_TurnOnDut();
	delay_ms(200);
}


#define LEVEL_HIGH	1
#define LEVEL_LOW	0

static BOOL Test_SerialPort(U8 rly_loop, U8 rly_in, U8 rly_out)
{
    U8 res;

	RLY_ON(rly_loop);
	RLY_ON(rly_in);
	RLY_ON(rly_out);

	res = RS232_loop_test();

	RLY_OFF(rly_loop);
	RLY_OFF(rly_in);
	RLY_OFF(rly_out);

	if(res == TRUE)
		GUI_SendMessage("Passed\r\n");
	else
		GUI_SendMessage("Failed\r\n");

	return res;
}
U8 board_version = 0x0F;


void TestItem_Impedance(U8 * test_result)
{
	U8 i;
	U8 res;

	*test_result = PASS;

	for (i=0; i<sizeof(Impedance_item)/sizeof(VOLT_ITEM_T); i++) {
		RLY_ON(26+i);
		delay_ms(50);
		res = TestVoltages((P_VOLT_ITEM_T)&Impedance_item[i]);
		RLY_OFF(26+i);

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
	PWR_TurnOnDut();
	delay_ms(100);
    GUI_SendMessage("Test Current\r\n");
	curr = TestCurrent(1, 2, 1);

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

    * test_result = PASS;

    for(i=0; i<sizeof(volt_item)/sizeof(VOLT_ITEM_T); i++) {
    	res = TestVoltages((P_VOLT_ITEM_T)&volt_item[i]);
		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		* test_result &= res;
    }
}

void TestItem_Reset(U8 * test_result)
{
	int i;
	*test_result = FAIL;

	for (i=0; i<100; i++) {
		if (TestVoltages(&Reset_item)) {
			*test_result = PASS;
			break;
		}
		delay_ms(100);
	}

	if(*test_result == PASS)
		GUI_SendMessage("Passed\r\n");
	else
		GUI_SendMessage("Failed\r\n");
}

void TestItem_Battery(U8 * test_result)
{
	*test_result = FAIL;

	RLY_ON(71);
	delay_ms(30);

	if (TestVoltages(&Battery_item)) {
		*test_result = PASS;
	}

	RLY_OFF(71);
	delay_ms(30);

	if(*test_result == PASS)
		GUI_SendMessage("Passed\r\n");
	else
		GUI_SendMessage("Failed\r\n");
}

void TestItem_UBoot(U8 * test_result)
{
	/*int i;
	char BootConfig[3]={0};

	 *test_result=FAIL;
	 for (i=0; i<3; i++) {
		 EXTIO_ReadByte(1+8*i, BootConfig+i);
	 }
	 BootConfig[0] &= 0xff;
	 BootConfig[1] &= 0xff;
	 BootConfig[2] &= 0x01;

	 if ((BootConfig[0]==0x20) && (BootConfig[1]==0x20) && BootConfig[2]==0x01) {
		 //*test_result = PASS;
		 //GUI_SendMessage("Passed\r\n");
	 }
	 else {
		 *test_result = FAIL;
		 //GUI_SendMessage("Failed\r\n");
	 }
	 RLY_ON(49);
	 RLY_ON(50);
	 tcflush(DUT_COMM_setting.fd, TCIFLUSH);
	 PowerCycle();*/

		 UART_SendFrame(DUT_COMM_setting.fd, "\r", sizeof("\r"));
		 *test_result = PASS;
	 //}
	 *test_result = FAIL;
}

void TestItem_Program(U8 *test_result)
{
	char cmdBuff[100]={0};

	DUT_SendComman("eep i \"NovTech\"");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	DUT_SendComman("eep c \"VeriFone\"");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	sprintf(cmdBuff, "%s%s", "eep a ", eepInfo.board_name);
	DUT_SendComman(cmdBuff);
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep b ", eepInfo.board_version);
	DUT_SendComman(cmdBuff);
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep v ", eepInfo.software_version);
	DUT_SendComman(cmdBuff);
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep X ", eepInfo.MAC_address);
	DUT_SendComman(cmdBuff);
	//DUT_SendComman("eep X \"01:02:03:04:05:06\"");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	DUT_SendComman("eep p 1");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	memset(cmdBuff, 0, 100);
	sprintf(cmdBuff,"%s%s", "eep n ", eepInfo.serial_num);
	DUT_SendComman(cmdBuff);
	//DUT_SendComman("eep n \"1N72800008\"");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	DUT_SendComman("eep save");
	UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1);

	delay_ms(500);

	/*PowerCycle();

	 if (UART_CheckStr(DUT_COMM_setting.fd, "atuoboot:", 3)) {
		 UART_SendFrame(DUT_COMM_setting.fd, "\r\n", sizeof("\r\n"));
		 *test_result = PASS;
	 }

	// DUT_SendComman("eep u");
	 //delay_ms(500);

	 //DUT_GetCommandAck(cmdBuff);*/
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
	char cmdBuff[256]={0};
	int voltage=0;
	int input_voltage=0;
	int voltageMax=0;
	int voltageMin=0;

	GUI_SendMessage("Start to test KL82\r\n");

	input_voltage=getVoltage(2);

	voltageMax = input_voltage*105/100;
	voltageMin = input_voltage*95/100;

	GUI_SendMessage("vin 0\r\n");
	DUT_SendComman("omnia2k82 vin 0");
	delay_ms(200);
	DUT_GetCommandAck(cmdBuff);
	if (!getDigitFromString(cmdBuff, "VIN = ", &voltage)) {
		//*test_result = PASS;
		GUI_SendMessage("FAIL\r\n");
		//return;
	}
	GUI_SendMessage("vin=%dmV\r\n",voltage);

	if (voltage>voltageMin && voltage<voltageMax) {
		*test_result = PASS;
		GUI_SendMessage("PASS\r\n");
	}

	DUT_SendComman("omnia2k82 vin 1");
	delay_ms(200);
	DUT_GetCommandAck(cmdBuff);

	if (!getDigitFromString(cmdBuff, "VIN = ", &voltage)) {
			*test_result = PASS;
			GUI_SendMessage("FAIL\r\n");
			//return;
	}
	GUI_SendMessage("vin=%dmV\r\n",voltage);
	if (voltage>voltageMin && voltage<voltageMax) {
			*test_result = PASS;
			GUI_SendMessage("PASS\r\n");
	}
}

void TestItem_Contrlled(U8 * test_result)
{
	int i;
	int res;

	GUI_SendMessage("Start to test regulaor\r\n");
	DUT_SendComman("omnia_power");
	delay_ms(100);
	if(!UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1)) {
		GUI_SendMessage("omnia_power err\r\n");
	}

	for (i=0; i<sizeof(cvolt_item)/sizeof(VOLT_ITEM_T); i++) {

		res = TestVoltages((P_VOLT_ITEM_T)&cvolt_item[i]);
		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		* test_result &= res;
	}
}

void TestItem_CurrSrc(U8 * test_result)
{
	U32 curr=0;

    *test_result = PASS;
    GUI_SendMessage("Set to 35mA Current Source\r\n");

    RLY_ON(72);
    delay_ms(100);

    DUT_SendComman("omnia_current1");
    if(!UART_CheckStr(DUT_COMM_setting.fd, "OMNIA_MFG", 1)) {
    	GUI_SendMessage("omnia_current1 err\r\n");
    }
	curr = TestCurrent(19, 20, 10);

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
	curr = TestCurrent(19, 20, 10);

	RLY_OFF(72);

	if (curr >= CURR_SRC_45MIN && curr <= CURR_SRC_45MAX) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
		*test_result = FAIL;
	}
}



void TestItem_Storage(U8 * test_result)
{
	int i=0;
	char AckBuff[500]={0};

	*test_result = PASS;
	GUI_SendMessage("Start to test storage\r\n");

	for (i=0; i<2; i++) {

		DUT_SendComman("omnia_storage");
		delay_ms(500);
		DUT_GetCommandAck(AckBuff);

		if (strstr(AckBuff, "Reboot and re-run to check SRAM") != NULL) {
			if (strstr(AckBuff, "SRAM STORE OK") != NULL) {
				GUI_SendMessage("SRAM STORE PASS\r\n");

				if (strstr(AckBuff, "SPI-NOR OK") != NULL) {
					GUI_SendMessage("SPI NOR PASS\r\n");
				}
				else {
					*test_result = FAIL;
					GUI_SendMessage("SPI NOR FAIL\r\n");
				}

				if (strstr(AckBuff, "eMMC OK") != NULL) {
						GUI_SendMessage("eMMC PASS\r\n");
				}
				else {
					*test_result = FAIL;
					GUI_SendMessage("eMMC FAIL\r\n");
				}
			}
			else {
				*test_result = FAIL;
				GUI_SendMessage("SRAM STORE FAIL\r\n");
			}
		}
		else {
			if (strstr(AckBuff, "SRAM - PERSIST OK") != NULL) {
				GUI_SendMessage("SRAM PERSIST PASS\r\n");
			}
			else {
				*test_result = FAIL;
				GUI_SendMessage("SRAM STORE FAIL\r\n");
			}
		}
	}

	/*DUT_SendComman("omnia_storage");
	delay_ms(500);
	DUT_GetCommandAck(AckBuff);
	if (strstr(AckBuff, "SPI-NOR OK") != NULL) {
		GUI_SendMessage("SPI NOR PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("SPI NOR FAIL\r\n");
	}

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

	DUT_SendComman("omnia_storage");
	delay_ms(500);
	DUT_GetCommandAck(AckBuff);

	if (strstr(AckBuff, "SRAM - PERSIST OK") != NULL) {
		GUI_SendMessage("SRAM PERSIST PASS\r\n");
	}
	else {
		*test_result = FAIL;
		GUI_SendMessage("SRAM STORE FAIL\r\n");
	}*/
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
	delay_ms(200);
	DUT_GetCommandAck(AckBuff);

	for (i=0; i< 3; i++) {
		if (!getDigitFromString(AckBuff, temp_sensor[i].verifystr, &dtt[i])) {
			GUI_SendMessage("FAIL\r\n");
		}

		GUI_SendMessage("%s=%d C\r\n",temp_sensor[i].title,  dtt[i]);
		if (dtt[i] > 20 && dtt[i]<30) {
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
	for (i=0; i<8; i++) {
		GUI_SendMessage("Start to test %s\r\n", Ethernet_Item[i].title);

		RLY_ON(Ethernet_Item[i].channel);
		delay_ms(100);

		DUT_SendComman("ping 170.20.100.253");
		if (UART_CheckStr(DUT_COMM_setting.fd, "alive", 3)) {
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

	DUT_SendComman("boot");

	if (!UART_CheckStr(DUT_COMM_setting.fd, "omnia login:", 10)) {
		*test_result = FAIL;
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


void TestItem_RTC(U8 * test_result)
{
	GUI_SendMessage("Start to test RTC\r\n");

	DUT_SendComman("./rtc.sh");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "MMDDhhmm[[YY]YY][.ss]", 3)) {
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

	GUI_SendMessage("RTC test PASS\r\n");
	*test_result = PASS;

}

void TestItem_USB(U8 * test_result)
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
	GUI_SendMessage("Start to test Audio\r\n");
	DUT_SendComman("./audio2.sh");
	if (!UART_CheckStr(DUT_COMM_setting.fd, "press return to continue", 1)) {
		GUI_SendMessage("Can't exec audio2.sh\r\n");
		*test_result = FAIL;
		return;
	}
	DUT_SendComman("");
	Audio_WhiteNoiseTest();
	DUT_SendComman("");
	Audio_WhiteNoiseTest();
	DUT_SendComman("");
}

void TestItem_CurrLoop(U8 * test_result)
{

    U8 i;
    //U8 res;

    //* test_result = PASS;
   /* GUI_SendMessage("Start to test UART/current loop\r\n");
	GUI_SendMessage("Set the i.MX6x Active mode\r\n");
	DUT_SendComman("./memtool 20A0000=7800FA0F");

    for(i=0; i<sizeof(currloop_item)/sizeof(CURRLOOP_ITEM_T); i++) {
    	res = TestCurrLoopItem((P_CURRLOOP_ITEM_T)&currloop_item[i]);
		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		* test_result &= res;
   }

    GUI_SendMessage("Set the i.MX6x Passive mode\r\n");
    DUT_SendComman("./memtool 20A0000=7800FB0F");*/
}



void TestItem_PowerOff(U8 * test_result)
{
    PWR_TurnOffDut();
    //LOGFILE_Write();
    Modules_Init();
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
	//{'A',  TestItem_Assembly},
	//{'I',  TestItem_Impedance},
	//{'N',  TestItem_PowerOn},
	//{'V',  TestItem_Voltage},
	//{'W',  TestItem_Reset},
	//{'B',  TestItem_Battery},
	//{'H',  TestItem_UBoot},
	{'P',  TestItem_Program},
	//{'L',  TestItem_LED},
	{'K',  TestItem_KL82},
	{'O',  TestItem_Contrlled},
	{'T',  TestItem_CurrSrc},
	{'S',  TestItem_Storage},
	{'M',  TestItem_TemperatureSensor},
	//{'E',  TestItem_Ethernet},
	{'X',  TestItem_LinuxBoot},
	{'R',  TestItem_RTC},
	{'U',  TestItem_USB},
	{'A',  TestItem_Audio},
	{'C',  TestItem_CurrLoop},
	{'F',  TestItem_PowerOff},	// Phred
};

#define TEST_TIMES	20

void dbg(void)
{
	U8 i;
	U8 test_result;
	//int test_num=TestIdTab/sizeof(TEST_ID);

	for (i=0;i<TEST_TIMES;i++) {
		//TestItem_Program(&test_result);

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

	if(UART_GetLine(GUI_COMM_setting.fd, rbuf, (U8 * )&rcnt) == FALSE)
		return;

	printf("Got GUI '%s'\n", rbuf);

	/*for (i = 1; i <= rcnt; i++) {
		is_upp = isupper(rbuf[rcnt-i]);
//    	printf("buff%d=%d,is %d\n", rcnt-i, rbuf[rcnt-i], is_upp);

        if(is_upp) {
            len = i;
            break;
        }
	}*/

	len = rcnt;
	if (len == 0 || len > GUI_CMD_FRAME_MAX) {//Buffer received error, it has been too long
		printf("Command is error\n");
		return;
	}
	strcpy(cmd,rbuf);
	//memcpy(cmd, &rbuf[rcnt-i], len);

	for(i=0; i<id_sum; i++)
	{
		if(TestIdTab[i].TestIdChar == cmd[0])    // Get the sum of testing ID from the TestIdTable.
		{
			if(cmd[0] == 'P')
				parseProgrammingParameter(cmd);

			(TestIdTab[i].TestFunc)(&test_result);     	// Testing.

			ReportResult(test_result);

			return;
		}
	}

	printf("Command is error\n");
}

