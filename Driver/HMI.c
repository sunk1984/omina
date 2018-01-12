
#include "includes.h"

#define HMINUM_ONLY_1 	    1

#define HMIFUNC_SET_LED 	0x71
#define HMIFUNC_SET_BUZZ 	0x72
#define HMIFUNC_GET_KEY 	0x73

#define HMIREG_SET_BUZZ	    0

#define HMIREG_LED_RUN 	    1
#define HMIREG_LED_PASS 	2
#define HMIREG_LED_FAIL 	3

#define HMIREG_KEY_FUNC 	1
#define HMIREG_KEY_YES 	    2
#define HMIREG_KEY_NO 	    3

#define LCDNUM_ONLY_1 	    1

#define LCDFUNC_WRITE_LCD 	0x61
#define LCDFUNC_CLEAR_LCD	0x62

#define LCD_PERLINE_MAX     24
#define LCD_LINE_MAX        4

const U8 ClearLineData[] = "00";

const U8 BuzzPassData[] = "01";
const U8 BuzzFailData[] = "02";
const U8 BuzzShortWarnData[] = "03";
const U8 BuzzLongWarnData[] = "04";
const U8 BuzzLongWarnDataOff[] = "05";

const U8 LedOffData[] = "00";
const U8 LedOnData[] =  "01";
const U8 LedFlashData[] = "02";

const U8 KeyActiveData[] = "01";
const U8 KeyIdleData[] = "00";

BOOL HMI_WriteCmd(U8 func, U8 reg, U8 *WriteStr)
{
    return(MERAK_WriteCmd((U8 *)"HMI", HMINUM_ONLY_1, func, reg, WriteStr));
}

BOOL HMI_ReadCmd(U8 func, U8 reg, U8 *ReadStr)
{
    return(MERAK_ReadCmd((U8 *)"HMI", HMINUM_ONLY_1, func, reg, ReadStr));
}

BOOL HMI_ShortWarnBuzz(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_BUZZ, HMIREG_SET_BUZZ, (U8 * )BuzzShortWarnData));
}

BOOL HMI_LongWarnBuzz(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_BUZZ, HMIREG_SET_BUZZ, (U8 * )BuzzLongWarnData));
}

BOOL HMI_LongWarnBuzzOff(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_BUZZ, HMIREG_SET_BUZZ, (U8 * )BuzzLongWarnDataOff));
}

BOOL HMI_PassBuzz(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_BUZZ, HMIREG_SET_BUZZ, (U8 * )BuzzPassData));
}

BOOL HMI_FailBuzz(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_BUZZ, HMIREG_SET_BUZZ, (U8 * )BuzzFailData));
}

BOOL HMI_OnPassLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_PASS, (U8 * )LedOnData));
}

BOOL HMI_OffPassLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_PASS, (U8 * )LedOffData));
}

BOOL HMI_OnFailLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_FAIL, (U8 * )LedOnData));
}

BOOL HMI_OffFailLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_FAIL, (U8 * )LedOffData));
}

BOOL HMI_OnRunLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_RUN, (U8 * )LedOnData));
}

BOOL HMI_OffRunLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_RUN, (U8 * )LedOffData));
}

BOOL HMI_FlashRunLed(void)
{
    return(HMI_WriteCmd(HMIFUNC_SET_LED, HMIREG_LED_RUN, (U8 * )LedFlashData));
}

BOOL HMI_KeyPress(U8 key_num)
{
    U8 ReadKeyData[3];

    if (HMI_ReadCmd(HMIFUNC_GET_KEY, key_num, ReadKeyData))
    {
        if(strncmp((char *)ReadKeyData, (char *)KeyActiveData, 2) == 0)
        {
            return(TRUE);
        }
    }
    return(FALSE);
}

BOOL HMI_PressFuncKey(void)
{
    return(HMI_KeyPress(HMIREG_KEY_FUNC));
}

BOOL HMI_PressYesKey(void)
{
    return(HMI_KeyPress(HMIREG_KEY_YES));
}

BOOL HMI_PressNoKey(void)
{
    return(HMI_KeyPress(HMIREG_KEY_NO));
}

BOOL LCD_WriteCmd(U8 func, U8 reg, U8 *WriteStr)
{
    return(MERAK_WriteCmd((U8 *)"LCD", LCDNUM_ONLY_1, func, reg, WriteStr));
}

static void LCD_Printf(U8 *str)
{
    U8 prtStr[50];

    sprintf((char *)prtStr, "%s\r\n", (char *)str);
	Dprintf((char *)prtStr);
}


static char* StrCpy(char* ds, char* ss)
{
	while(*ss)
		*ds++ = *ss++;
	return ds;
}

static void StrnCpy(char* ds, char* ss, int n)
{
    int i;
    for(i=0;i<n;i++)
		*ds++ = *ss++;
}

void LCD_DisplayALine(U8 line, U8 *str)
{
    LCD_WriteCmd(LCDFUNC_WRITE_LCD, line, (U8 *)str);
	LCD_Printf(str);
}

void LCD_Display2Line(U8 startline, U8 * str)
{
    U8 lcdStr1[LCD_PERLINE_MAX+1],lcdStr2[LCD_PERLINE_MAX+1]={0};

    StrnCpy((char *)lcdStr1, (char *)str, LCD_PERLINE_MAX);
    StrCpy((char *)lcdStr2, (char *)(str+LCD_PERLINE_MAX));

    lcdStr1[LCD_PERLINE_MAX] = 0;
    lcdStr2[LCD_PERLINE_MAX] = 0;

    LCD_DisplayALine(startline, lcdStr1);
    LCD_DisplayALine(startline+1, lcdStr2);

	LCD_Printf(lcdStr1);
	LCD_Printf(lcdStr2);
}


void LCD_DisplayAItem(U8 * str)
{
    if(* str == 0)
        return;

	LCD_DisplayALine(LCD_LINE1, str);
	LCD_DisplayALine(LCD_LINE2, (U8 *)"");
}


void LCD_DisplayResult(U32 res)
{
	if(res == PASS)
		LCD_DisplayALine(LCD_LINE2, (U8 *)"PASS!  ");
	else
		LCD_DisplayALine(LCD_LINE2, (U8 *)"FAIL!  ");
}


void HMI_ShowPass(void)
{
	LCD_DisplayALine(LCD_LINE1, (U8 *)"");
	LCD_DisplayALine(LCD_LINE2, (U8 *)"PASS!!!");
	
	HMI_OnPassLed();
    HMI_PassBuzz();
}


void HMI_ShowFail(void)
{
	LCD_DisplayALine(LCD_LINE1, (U8 *)"");
	LCD_DisplayALine(LCD_LINE2, (U8 *)"FAIL!!!");
	
	HMI_OnFailLed();
    HMI_FailBuzz();
}




