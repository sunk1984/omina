
#include "includes.h"

#define CHANNEL_NUMBER_PER_BOARD    24

#define RLYFUNC_ON_OFF_CHAN  0x21
#define RLYFUNC_OFF_ALLCHAN  0x22
#define RLYFUNC_SCAN_CHAN    0x23
#define RLYFUNC_SET_MODE     0x24

#define RLYREG_SET_BOARD     0

const U8 RlyOnData[] = "01";
const U8 RlyOffData[] = "00";

const U8 ModeCommonlData[] = "01";
const U8 ModeAdData[] = "00";

static BOOL RLY_Chan_Total2Board(U8 *board_num, U8 *board_chan, U32 TotalChan)
{
    if (TotalChan == 0)
        return FALSE;
	
    *board_num = ((TotalChan-1) / CHANNEL_NUMBER_PER_BOARD) + 1;
    *board_chan = TotalChan % CHANNEL_NUMBER_PER_BOARD;
    
    if (*board_chan == 0)
        *board_chan = CHANNEL_NUMBER_PER_BOARD;
    
    return TRUE;
}


static BOOL RLY_WriteCmd(U8 board_num, U8 func, U8 reg, U8 *WriteStr)
{
    return(MERAK_WriteCmd((U8 *)"RLY", board_num, func, reg, WriteStr));
}

/*********************************************************************************
function:    RLY_ON
description: open Relay
parameters:  board_num board_chan
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_ON(U32 TotalChan)
{
	U8 board_num=0;
	U8 board_chan=0;

	if (RLY_Chan_Total2Board(&board_num, &board_chan, TotalChan) == FALSE)
		return FALSE;

    return(RLY_WriteCmd(board_num, RLYFUNC_ON_OFF_CHAN, board_chan, (U8 *)RlyOnData));
}
/*********************************************************************************
function:    RLY_OFF
description:
parameters:  Channelnum
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_OFF(U32 TotalChan)
{
	U8 board_num=0;
	U8 board_chan=0;

	if (RLY_Chan_Total2Board(&board_num, &board_chan, TotalChan) == FALSE)
		return FALSE;

    return(RLY_WriteCmd(board_num, RLYFUNC_ON_OFF_CHAN, board_chan, (U8 *)RlyOffData));
}

/*********************************************************************************
function:    RLY_OffAll
description:
parameters:  board_num
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_OffAll(U8 board_num)
{
    return(RLY_WriteCmd(board_num, RLYFUNC_OFF_ALLCHAN, RLYREG_SET_BOARD, (U8 *)""));
}

/*********************************************************************************
function:    RLY_Clear
description: 
parameters:  boards_sum
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_Clear(U8 boards_sum)
{
    U8 i;

    for(i = 1; i <= boards_sum; i++)
    {
        if(RLY_OffAll(i) == FALSE)
            break;
        delay_ms(2); //delay 2ms
    }
    
    delay_ms(10); //delay 10ms

    if(i == (boards_sum+1))
        return(TRUE);
    else
        return(FALSE);
}


/*********************************************************************************
function:    RLY_Scan
description:
parameters:  board_num
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_Scan(U8 board_num)
{
    return(RLY_WriteCmd(board_num, RLYFUNC_SCAN_CHAN, RLYREG_SET_BOARD, (U8 *)""));
}

/*********************************************************************************
function:    RLY_SetAdMode
description:
parameters:  board_num
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_SetAdMode(U8 board_num)
{
    return(RLY_WriteCmd(board_num, RLYFUNC_SET_MODE, RLYREG_SET_BOARD, (U8 *)ModeAdData));
}

/*********************************************************************************
function:    RLY_SetCommonMode
description:
parameters:  board_num
return: TRUE/FALSE
*********************************************************************************/
BOOL RLY_SetCommonMode(U8 board_num)
{
    return(RLY_WriteCmd(board_num, RLYFUNC_SET_MODE, RLYREG_SET_BOARD, (U8 *)ModeCommonlData));
}




