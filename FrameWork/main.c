
#include "includes.h"

/*

#define PIN_DUTPROB RPI_GPIO_P1_15

BOOL PushDownDut(void)
{
	if (!bcm2835_gpio_lev(PIN_DUTPROB))
		return TRUE;
	else
		return FALSE;
}

void InitDUTProb(void)
{
	if (!bcm2835_init())
		perror();

	bcm2835_gpio_fsel(PIN_DUTPROB, BCM2835_GPIO_FSEL_INPT); // Set RPI pin P1-15 to be an input
	bcm2835_gpio_set_pud(PIN_DUTPROB, BCM2835_GPIO_PUD_UP);	//  with a pullup
}

void Prob_DUT(void)
{
	GUI_SendMessage((U8 *)"Please pull up the handle\n");

	while(1) {
		if(!PushDownDut())
			break;

		delay_ms(100);
	}

	GUI_SendMessage((U8 *)"Please load a DUT\n");

	while(1) {
		if(PushDownDut())
			break;

		delay_ms(100);
	}
}

*/

void delay_ms(U32 ms)
{
	usleep(1000*ms);
}

void Dprintf(char *lpszFormat, ...)
{
    char DisplayBuff[100] = {0};
    va_list fmtList;

    va_start( fmtList, lpszFormat );
    vsprintf( DisplayBuff, lpszFormat, fmtList );
    va_end( fmtList );

	printf(DisplayBuff);
    LOGFILE_AddItem((U8 *)DisplayBuff);
}


int main(int argc, char *argv[])
{
	Modules_Config();
	Modules_Init();

	//dbg();

	while(1) {
		GUI_Process();
		delay_ms(2);
	}

	return 0;
}
