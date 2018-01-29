/*
 * test_led.c
 *
 *  Created on: Jun 13, 2017
 *      Author: test
 */
#include "includes.h"


enum led_sensor_chan {

	LED_CH_D32,
	LED_CH_D31,
	LED_CH_D30,
	LED_CH_D29,
	LED_CH_D28,
	LED_CH_D37,
	LED_CH_D39,
	LED_CH_D47,
	LED_CH_D49,
	LED_CH_D58,
	LED_CH_D53,
	LED_CH_D57,
	LED_CH_D15,
	LED_CH_D14,
	LED_CH_D73,
	LED_CH_D23,
};

int hueLimits[][2] = {
	{0,   2},    //red
	{100, 130},  //green
	{220, 250},  //blue
	{2,   10},   //amber
	{60,  90},   //yellow
	{10,  20},   //orange
};

int intensityLimits[][2] = {
	{3000, 10000},
	{30000, 40000},
};

#define UART_TX_1S_BYTES  3000

static int sendFeasaCommnd(char *cmd, char *rdBuff, U8 *rcnt)
{
	int timeout=0;
	char txBuff[16]={0};

	sprintf((char *)txBuff, "%s\r\n", cmd);
	UART_SendFrame(LED_COMM_setting.fd, txBuff, sizeof(txBuff));

	while(1) {
		if (timeout++ > 1000)
			break;

		if (UART_GetLine(LED_COMM_setting.fd, (U8 *)rdBuff, rcnt)) {
			return TRUE;
		}
		delay_ms(1);
    }
	return FAIL;
}

static void LED_NullFunc(void)
{
    return;
}

static void LED_StartFA(void)
{
	char txBuff[]="omnia_led FA 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndFA(void)
{
	char txBuff[]="omnia_led FA 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartC1(void)
{
	char txBuff[]="omnia_led C1 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndC1(void)
{
	char txBuff[]="omnia_led C1 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartC2(void)
{
	char txBuff[]="omnia_led C2 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndC2(void)
{
	char txBuff[]="omnia_led C2 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartC3(void)
{
	char txBuff[]="omnia_led C3 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndC3(void)
{
	char txBuff[]="omnia_led C3 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartC4(void)
{
	char txBuff[]="omnia_led C4 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndC4(void)
{
	char txBuff[]="omnia_led C4 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_StartCE(void)
{
	char txBuff[]="omnia_led CE 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndCE(void)
{
	char txBuff[]="omnia_led CE 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_StartFE(void)
{
	char txBuff[]="omnia_led FE 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndFE(void)
{
	char txBuff[]="omnia_led FE 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartPM(void)
{
	char txBuff[]="omnia_led PM 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}
static void LED_EndPM(void)
{
	char txBuff[]="omnia_led PM 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartPE(void)
{
	char txBuff[]="omnia_led PE 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_EndPE(void)
{
	char txBuff[]="omnia_led PE 0\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartBAT_OK(void)
{
	char txBuff[]="omnia2k82 ledtest 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_EndBAT_OK(void)
{
	char txBuff[]="omnia2k82 ledtest 1\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_StartDC_OK(void)
{
	char txBuff[]="omnia2k82 ledtest 2\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

static void LED_EndDC_OK(void)
{
	char txBuff[]="omnia2k82 ledtest 2\r";

	UART_SendFrame(DUT_COMM_setting.fd, txBuff, strlen((char *)txBuff));
}

const LED_ITEM_T led_item[] = {
	{"LED D22",     14,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D76",     18,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D25",     17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D24",     16,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},

	{"LED D32",      3,   LED_COLOR_RED,  FALSE, LED_StartFA, LED_EndFA},
	{"LED D31,",     2,   LED_COLOR_GREEN,  FALSE, LED_StartC1, LED_EndC1},
	{"LED D30,",     2,   LED_COLOR_GREEN,  FALSE, LED_StartC2, LED_EndC2},
	{"LED D29,",     1,   LED_COLOR_GREEN,  FALSE, LED_StartC3, LED_EndC3},
	{"LED D28,",     1,   LED_COLOR_GREEN,  FALSE, LED_StartC4, LED_EndC4},
	{"LED D37",      4,   LED_COLOR_YELLOW,  FALSE, LED_StartCE, LED_NullFunc},
	{"LED D39",      5,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,LED_EndCE},
	{"LED D47",      6,   LED_COLOR_GREEN,  FALSE, LED_StartFE, LED_NullFunc},
	{"LED D49,",     6,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,LED_EndFE},
	{"LED D58,",     9,   LED_COLOR_YELLOW,  FALSE, LED_StartPM, LED_EndPM},
	{"LED D53",      7,   LED_COLOR_YELLOW,  FALSE, LED_StartPE, LED_NullFunc},
	{"LED D57,",     8,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,LED_EndPE},
	{"LED D15,",     20,  LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D14,",     19,  LED_COLOR_GREEN,  FALSE, LED_NullFunc,  LED_NullFunc},
};

const LED_ITEM_T led_LCD[] = {
	{"LED D27",     11,   LED_COLOR_RED,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D40",     12,   LED_COLOR_RED,  FALSE, LED_NullFunc, LED_NullFunc},
};

const LED_ITEM_T led_regulator[] = {
	{"LED D23",     15,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D73",     10,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
};

const LED_ITEM_T led_volt[] = {
	{"LED D7",      17,   LED_COLOR_GREEN,   FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D8",      17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D17",     17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D18",     17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D20",     17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
	{"LED D19",     17,   LED_COLOR_GREEN,  FALSE, LED_NullFunc, LED_NullFunc},
};

const LED_ITEM_T led_reset =
	{"LED D50",      13,   LED_COLOR_RED,  FALSE, LED_NullFunc, LED_NullFunc};


BOOL TestLED(P_LED_ITEM_T p_led_item)
{
	static char *ptr;
	char *line;
	double hue;
	int saturation;
	int intensity;
	U8 recvBytes=0;
	char recvBuff[32]={0};
	char CmdBuff[16]={0};

    GUI_SendMessage("%s\r\n", p_led_item->title);

	if (!sendFeasaCommnd("capture", recvBuff, &recvBytes)) {
		printf("send capture command failed\n");
		return FAIL;
	}
	if (strcmp(recvBuff, "OK")) {
		printf("get ok failed\n");
		return FAIL;
	}

	sprintf(CmdBuff, "%s%.2d", "gethsi", p_led_item->sensor_chan);
	if (!sendFeasaCommnd(CmdBuff, recvBuff, &recvBytes)) {
		printf("send getshi## command failed\n");
		return FAIL;
	}

	ptr = recvBuff;
	line = strtok(ptr, " ");
	if (line == NULL) {
		printf("get hue failed\n");
		return FAIL;
	}
	hue = atof(line);
	ptr += (strlen(line)+1);
	printf("hue=%f\n", hue);

	line = strtok(ptr, " ");
	if (line == NULL) {
		printf("get saturation failed\n");
		return FAIL;
	}
	saturation = atoi(line);
	ptr += (strlen(line)+1);

	printf("saturation=%d\n", saturation);

	line = strtok(ptr, " ");
	if (line == NULL) {
		printf("get intensity failed\n");
		return FAIL;
	}
	intensity = atoi(line);

	printf("intensity=%d\n", intensity);

	if (hue < hueLimits[p_led_item->color][0] || hue > hueLimits[p_led_item->color][1]) {
		printf("hue out of limit\n");
		return FAIL;
	}

	/*if (intensity < intensityLimits[p_led_item->is_flash][0] || intensity > intensityLimits[p_led_item->is_flash][1]) {
		printf("intensity out of limit\n");
		return FAIL;
	}*/

	(p_led_item->LedEndFunc)();

	return PASS;
}

int Test_LED(void)
{
    U8 i;
    U8 res;
    int test_result = PASS;

    for (i=0; i<sizeof(led_item)/sizeof(LED_ITEM_T); i++) {

	    (led_item[i].LedStartFunc)();
	    delay_ms(200);
	    res = TestLED((P_LED_ITEM_T)&led_item[i]);
	    (led_item[i].LedEndFunc)();

		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		test_result &= res;
    }
    return test_result;
}

int Test_ResetLED(void)
{
	int test_result = TestLED((P_LED_ITEM_T)&led_reset);

	if (test_result == PASS) {
		GUI_SendMessage("Passed\r\n");
	}
	else {
		GUI_SendMessage("Failed\r\n");
	}
	return test_result;
}

int Test_DismountLED(void)
{
	U8 i;
	U8 res;
	int test_result = PASS;
	for (i=0; i<sizeof(led_LCD)/sizeof(LED_ITEM_T); i++) {

		(led_item[i].LedStartFunc)();
		res = TestLED((P_LED_ITEM_T)&led_LCD[i]);
		(led_item[i].LedEndFunc)();

		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		test_result &= res;
	}
	return test_result;
}

int Test_RegulatorLED(void)
{
	U8 i;
	U8 res;
	U8 test_result=PASS;

	for(i=0; i<sizeof(led_regulator)/sizeof(LED_ITEM_T); i++) {

		(led_item[i].LedStartFunc)();
		res = TestLED((P_LED_ITEM_T)&led_regulator[i]);
		(led_item[i].LedEndFunc)();

		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		test_result &= res;
	}
	return test_result;
}
