/*
 * test_led.c
 *
 *  Created on: Jun 13, 2017
 *      Author: test
 */
#include "includes.h"

enum led_color {

	LED_COLOR_RED,
	LED_COLOR_GREEN,
	LED_COLOR_BLUE,
	LED_COLOR_AMBER,
	LED_COLOR_YELLOW,
	LED_COLOR_ORANGE,
};

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
	{110, 130},  //green
	{220, 250},  //blue
	{2,   10},   //amber
	{60,  80},   //yellow
	{10,  20},   //orange
};

int intensityLimits[][2] = {
	{10000, 20000},
	{30000, 40000},
};

#define UART_TX_1S_BYTES  3000

typedef void (*LED_FUNC)();

typedef struct {
	U8 title[40];
	U8 sensor_chan;
	U8 color;
	U8 is_flash;
    LED_FUNC LedStartFunc;
    LED_FUNC LedEndFunc;

} LED_ITEM_T, *P_LED_ITEM_T;

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

	{"LED D32",      LED_CH_D32,   LED_COLOR_GREEN,  FALSE, LED_StartFA, LED_EndFA},
	{"LED D31,",     LED_CH_D31,   LED_COLOR_GREEN,  FALSE, LED_StartC1, LED_EndC1},
	{"LED D30,",     LED_CH_D30,   LED_COLOR_GREEN,  FALSE, LED_StartC2, LED_EndC2},
	{"LED D29,",     LED_CH_D29,   LED_COLOR_GREEN,  TRUE,  LED_StartC3, LED_EndC3},
	{"LED D28,",     LED_CH_D28,   LED_COLOR_GREEN,  TRUE,  LED_StartC4, LED_EndC4},
	{"LED D37",      LED_CH_D37,   LED_COLOR_GREEN,  TRUE,  LED_StartCE, LED_NullFunc},
	{"LED D39",      LED_CH_D39,   LED_COLOR_GREEN,  TRUE,  LED_NullFunc,LED_EndCE},
	{"LED D47",      LED_CH_D47,   LED_COLOR_GREEN,  TRUE,  LED_StartFE, LED_NullFunc},
	{"LED D49,",     LED_CH_D49,   LED_COLOR_GREEN,  TRUE,  LED_NullFunc,LED_EndFE},
	{"LED D58,",     LED_CH_D58,   LED_COLOR_GREEN,  TRUE,  LED_StartPM, LED_EndPM},
	{"LED D53",      LED_CH_D53,   LED_COLOR_GREEN,  FALSE, LED_StartPE, LED_NullFunc},
	{"LED D57,",     LED_CH_D57,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,    LED_EndPE},
	{"LED D15,",     LED_CH_D15,   LED_COLOR_GREEN,  FALSE, LED_StartBAT_OK, LED_EndBAT_OK},
	{"LED D14,",     LED_CH_D14,   LED_COLOR_GREEN,  FALSE, LED_StartDC_OK,  LED_EndDC_OK},
	{"LED D73",      LED_CH_D73,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,    LED_NullFunc},
	{"LED D23",      LED_CH_D23,   LED_COLOR_GREEN,  FALSE, LED_NullFunc,    LED_NullFunc},
};

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

	sprintf(CmdBuff, "%s%d", "getshi", led_item->sensor_chan);
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

	line = strtok(ptr, " ");
	if (line == NULL) {
		printf("get saturation failed\n");
		return FAIL;
	}
	saturation = atoi(line);
	ptr += (strlen(line)+1);

	line = strtok(ptr, " ");
	if (line == NULL) {
		printf("get intensity failed\n");
		return FAIL;
	}
	intensity = atoi(line);

	if (hue < hueLimits[p_led_item->color][0] || hue > hueLimits[p_led_item->color][1]) {
		printf("hue out of limit\n");
		return FAIL;
	}

	if (intensity < intensityLimits[p_led_item->is_flash][0] || intensity > intensityLimits[p_led_item->is_flash][1]) {
		printf("intensity out of limit\n");
		return FAIL;
	}

	(p_led_item->LedEndFunc)();

	return PASS;
}

void TestItem_LED(U8 * test_result)
{
    U8 i;
    U8 res;

    *test_result = PASS;

    for(i=0; i<sizeof(led_item)/sizeof(LED_ITEM_T); i++) {

	    (led_item[i].LedStartFunc)();
	    res = TestLED((P_LED_ITEM_T)&led_item[i]);
	    (led_item[i].LedEndFunc)();

		if(res == PASS)
			GUI_SendMessage("Passed\r\n");
		else
			GUI_SendMessage("Failed\r\n");

		*test_result &= res;
    }
}

