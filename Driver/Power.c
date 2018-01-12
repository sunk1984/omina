
#include "includes.h"

#define PWRFUNC_SET_VOLT	    0x11//
#define PWRFUNC_ON_OFF_DUT 	    0x12//
#define PWRFUNC_ON_OFF_AUX 	    0x13//
#define PWRFUNC_RD_CURRENT 	    0x14//
#define PWRFUNC_VOL_ADD 	    0x15//
#define PWRFUNC_VOL_SUB 	    0x16//

#define PWRCHAN_DUT	            0x01//DUT
#define PWRCHAN_MEARK	        0x02//meark
#define PWRCHAN_AUX 	        0x01

#define PWRNUM_ONLY_1       1
#define MAX_VOL_DUT        24000    //DUT
const U8 TurnOnData[]  = "01";
const U8 TurnOffData[] = "00";

/**************************************************************************** 

*****************************************************************************/   
static void Hex2Str32(U8 *str, U32 hex_data)
{
    sprintf((char *)str, "%d", hex_data);
} 
/******************************************************************************

******************************************************************************/
static BOOL PWR_WriteCmd(U8 func, U8 reg, U8 *data_str)
{
    return(MERAK_WriteCmd((U8 *)"PWR", PWRNUM_ONLY_1, func, reg, data_str));
}
/******************************************************************************

******************************************************************************/
static BOOL PWR_ReadCmd(U8 func, U8 reg, U8 *ReadStr)
{
    return(MERAK_ReadCmd((U8 *)"PWR", PWRNUM_ONLY_1, func, reg, ReadStr));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_TurnOnDut(void)
{
    return(PWR_WriteCmd(PWRFUNC_ON_OFF_DUT, PWRCHAN_DUT, (U8 * )TurnOnData));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_TurnOffDut(void)
{
    return(PWR_WriteCmd(PWRFUNC_ON_OFF_DUT, PWRCHAN_DUT, (U8 * )TurnOffData));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_TurnOnAux(void)
{
    return(PWR_WriteCmd(PWRFUNC_ON_OFF_AUX, PWRCHAN_AUX, (U8 * )TurnOnData));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_TurnOffAux(void)
{
    return(PWR_WriteCmd(PWRFUNC_ON_OFF_AUX, PWRCHAN_AUX, (U8 * )TurnOffData));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_SetDutVolt(U32 volt)
{
    U8 VoltStr[6];
    //
    if(volt > MAX_VOL_DUT) 
    {
        volt = MAX_VOL_DUT;
    }
    Hex2Str32(VoltStr, volt);//
 
    return(PWR_WriteCmd(PWRFUNC_SET_VOLT, PWRCHAN_DUT, (U8 *)VoltStr));//
}

BOOL PWR_SetAuxVolt(U32 volt)
{
    U8 VoltStr[6];

    Hex2Str32(VoltStr, volt);
    
    return(PWR_WriteCmd(PWRFUNC_SET_VOLT, PWRCHAN_AUX, (U8 * )VoltStr));
}
/******************************************************************************

******************************************************************************/
BOOL PWR_GetDUTCur(U32 *pCur)
{
    U8 VoltStr[6];
    if(PWR_ReadCmd(PWRFUNC_RD_CURRENT, PWRCHAN_DUT, (U8 * )VoltStr)==TRUE)//
    {
        *pCur=atoi((char *)VoltStr);//
         return TRUE;
    }
    else
    {
        *pCur=0;
        return FALSE;
    }
}
/******************************************************************************

******************************************************************************/
BOOL PWR_AddDutVolt(U32 Stepval)
{
    U8 VoltStr[6];
    Hex2Str32(VoltStr, Stepval);//
    return(PWR_WriteCmd(PWRFUNC_VOL_ADD, PWRCHAN_DUT, (U8 *)VoltStr));//
}
/******************************************************************************

******************************************************************************/
BOOL PWR_SubDutVolt(U32 Stepval)
{
    U8 VoltStr[6];
    Hex2Str32(VoltStr, Stepval);//
    return(PWR_WriteCmd(PWRFUNC_VOL_SUB, PWRCHAN_DUT, (U8 *)VoltStr));//
}

