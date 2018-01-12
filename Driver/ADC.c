
#include "includes.h"

#define ADCFUNC_RD_VOLT      0x41
#define ADCFUNC_PRECISE_VOLT 0x43

#define OVERLOAD        2

#define AUTO_RANGE      0
#define CHAN1_0_3V3     1

#define ADCNUM_ONLY_1 	1

static BOOL ADC_ReadCmd(U8 board_num, U8 func, U8 reg, U8 *ReadStr)
{
	return (MERAK_ReadCmd((U8 *)"ADC", board_num, func, reg, ReadStr));
}

BOOL ADC_GetBoardVolt(U8 board_num, U32 range, U32 * pVolt) // use for several ADC boards
{
    U8 VoltStr[6] = {0};
    if(ADC_ReadCmd(board_num, ADCFUNC_RD_VOLT, range, (U8 * )VoltStr) == TRUE)     //?????
    {
        if(VoltStr[0] == 'O' && VoltStr[1] == 'L')  //Overload
             return(OVERLOAD);
        else
        {
            *pVolt = atoi((char *)VoltStr);     //ASCII???int???
             return(TRUE);
        }
    }
    
    return(FALSE);
}

BOOL ADC_GetPreciseVolt(U8 board_num, U32 * pVolt)
{
    U8 VoltStr[6] = {0};
    if(ADC_ReadCmd(board_num, ADCFUNC_PRECISE_VOLT, CHAN1_0_3V3, (U8 * )VoltStr) == TRUE)     //?????
    {
        if(VoltStr[0] == 'O' && VoltStr[1] == 'L')  //Overload
             return(OVERLOAD);
        else
        {
            *pVolt = atoi((char *)VoltStr);     //ASCII???int???
             return(TRUE);
        }
    }
    
    return(FALSE);
}

BOOL ADC_GetVolt(U32 *pVolt)
{
	U8 voltStr[6]={0};

	if (ADC_ReadCmd(ADCNUM_ONLY_1, ADCFUNC_RD_VOLT, AUTO_RANGE, voltStr)) {
		*pVolt = atoi((char *)voltStr);
		return TRUE;
	}
	return FALSE;
}

