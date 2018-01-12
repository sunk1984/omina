
#include "includes.h"

#define MERAK_FRAME_HEAD_LEN (14)   //from start to data.len
#define MERAK_FRAME_MIN (18)
#define MERAK_FRAME_MAX (48)            //

//#define NO_SUBBOARD_DEBUG
#ifdef NO_SUBBOARD_DEBUG
#define OVERTIME (6)            //
#else
#define OVERTIME (600)            //
#endif

MERAK_FRAME MERAK_RxFrame;
MERAK_FRAME MERAK_TxFrame;

const U8 WriteStr_EmptData[] = "";

extern USART_CONFIG MRK_COMM_setting;

/**************************************************************************** 
function:    BcdStr2Hex
description: BCD string to Hex
parameters:  U8 *str
return: Hex
*****************************************************************************/   
static U8 BcdStr2Hex(U8 *str)
{
    return((str[0] - '0') * 10 + str[1] - '0');
} 
/**************************************************************************** 
function:    Bcd2Str
description: BCD to string
parameters:  U8 *str U8 bcd_data
return: void
*****************************************************************************/   
static void Bcd2Str(U8 *str, U8 bcd_data)
{
    str[0] = (bcd_data/10) + '0';
    str[1] = (bcd_data%10) + '0';
} 

/**************************************************************************** 
function:    Hex2Str
description: Hex to string
parameters:  U8 *str U8 hex_data
return: void
*****************************************************************************/   
void Hex2Str(U8 *str, U8 hex_data)
{
    sprintf((char *)str, "%02x", hex_data);
    str[0] = toupper(str[0]);
    str[1] = toupper(str[1]);
} 

/*********************************************************************************
function:    MERAK_SetBuff
description:
parameters:  U8 *buff, U8 *str
return: void
*********************************************************************************/
void MERAK_SetBuff(U8 *buff, U8 *str)
{
	strncpy((char *)buff, (char *)str, strlen((char *)str));
}
/*********************************************************************************
function:    MERAK_GetChk
description:
parameters:  p_tx_frame
return: U8
*********************************************************************************/
static U8 MERAK_GetChk(MERAK_FRAME *p_frame)
{
    U8 i;
    U8 *p;
    U8 chk_len;
    U8 datalen_str[3] = {0,0,0};
    U8 chk_hex = 0;
    
    memcpy((char *)datalen_str,(char *)p_frame->data_len, 2);
    chk_len = BcdStr2Hex(datalen_str) + MERAK_FRAME_HEAD_LEN-1;

    p = p_frame->id;
    for (i = 0; i < chk_len; i++)
        chk_hex = (U8)(chk_hex + *(p+i));

    return(chk_hex);
}

/*********************************************************************************
function:    MERAK_VerifyChk
description:
parameters:  p_tx_frame
return: BOOL
*********************************************************************************/
static BOOL MERAK_VerifyChk(MERAK_FRAME *p_rx_frame)
{
    U8 chk_calc;
    U8 chk_recv;

    chk_calc = MERAK_GetChk(p_rx_frame);
    p_rx_frame->end[0] = 0;
    chk_recv = (U8)strtol((char *)p_rx_frame->chk, NULL, 16);
    
    if(chk_calc == chk_recv)
		return TRUE;
    else
	    return FALSE;
}

/*********************************************************************************
function:    MERAK_GetFrame
description:
parameters:  rx_data
return:      PASS or FAIL
*********************************************************************************/
static BOOL MERAK_GetFrame(MERAK_FRAME *p_rx_frame)
{
    U32 i;
    U32 timeout=0;
    U8 rcnt=0;
    U32 frame_len=0;
    U8 rbuf[255];

    while(1) {
		if (timeout++ > OVERTIME)
			return FALSE;

		if (UART_GetLine(MRK_COMM_setting.fd, rbuf, (U8 * )&rcnt))  //Received a frame by '\n'.
			break;

		delay_ms(1);
    }
	
	for (i = 0; i < rcnt; i++) {
        if (rbuf[i] == '^') {   //detect start falg
            frame_len = rcnt - i; //cal frame length
            break;
        }
	}
	
	if(i == rcnt)
		return FALSE;

	if (frame_len > MERAK_FRAME_MAX || frame_len < MERAK_FRAME_MIN) //Buffer received error, it has been too long or too short.
		return FALSE;

	memcpy(p_rx_frame, &rbuf[i], frame_len-2);
	memcpy(p_rx_frame->chk, &rbuf[i+frame_len-2], 2);

	p_rx_frame->data_data[frame_len-16] = 0;

//	printf("Get485:%s\n", p_rx_frame);

    return TRUE;
}

/*********************************************************************************
function:    MERAK_GetAck
description:
parameters:  p_tx_frame
return:      TRUE/FALSE
*********************************************************************************/
static BOOL MERAK_GetAck(MERAK_FRAME *p_tx_frame, U8 *rx_data)
{
    U32 data_len;
    
    MERAK_FRAME *p_rx_frame = &MERAK_RxFrame;

    if (!MERAK_GetFrame(p_rx_frame))
        return(FALSE);
    
    if (!MERAK_VerifyChk(p_rx_frame))
        return(FALSE);
    
    if (strncmp((char *)p_rx_frame->id, (char *)p_tx_frame->id, 3) != 0)
        return(FALSE);

    if (p_rx_frame->func[0] != p_tx_frame->func[0])
    	return(FALSE);

    if (p_rx_frame->func[1] == p_tx_frame->func[1] || p_rx_frame->func[1] == '0') {
    	data_len = BcdStr2Hex(p_rx_frame->data_len);
    	if (data_len < 26) {
    		memcpy((char *)rx_data, (char *)p_rx_frame->data_data, data_len);
    		printf("Got RS485 ack OK\n");
    		return TRUE;
    	}
    }

    return(FALSE);
}

/*********************************************************************************
function:
description:
parameters:  p_tx_frame
return: void
*********************************************************************************/
static void MERAK_SendFrame(MERAK_FRAME *p_tx_frame)
{
    U8 chk;
    U8 tx_len;
    U8 datalen_str[3] = {0};
    U8 tx_frame[MERAK_FRAME_MAX];

    chk = MERAK_GetChk(p_tx_frame); //
    Hex2Str(p_tx_frame->chk, chk);
	memcpy((char *)datalen_str, (char *)p_tx_frame->data_len, 2);//
	tx_len = BcdStr2Hex(datalen_str) + MERAK_FRAME_HEAD_LEN;  //
    
	memcpy(tx_frame, p_tx_frame, tx_len);        // Copy the strings to tx buffer from start to data
	memcpy(tx_frame+tx_len, p_tx_frame->chk, 2); // Copy chk string to tx buffer
	memcpy(tx_frame+tx_len+2, "\r\n", 2);        // Copy end string to tx buffer
	tx_len+=4;
	printf("Put RS485 Command\n");
	UART_SendFrame(MRK_COMM_setting.fd, tx_frame, tx_len);
}

/*********************************************************************************
function:    MERAK_BuildFrame
description:
parameters:  p_tx_frame,
return: void
*********************************************************************************/
static void MERAK_BuildFrame(MERAK_FRAME *p_tx_frame, U8 * board_id, U8 board_num, U8 func, U8 reg, U8 *data_str)
{
    U8 boardnum_str[4];
    U8 func_str[3];
    U8 reg_str[3];
    U8 datalen_str[3];

    Hex2Str(boardnum_str, board_num);
    Hex2Str(func_str, func);
    Bcd2Str(reg_str, reg);
    Bcd2Str(datalen_str, strlen((char *)data_str));

    MERAK_SetBuff(p_tx_frame->start, (U8 *)"~");   //
    MERAK_SetBuff(p_tx_frame->id, board_id); //
    MERAK_SetBuff(p_tx_frame->num, boardnum_str);//
    MERAK_SetBuff(p_tx_frame->func, func_str);   //
    MERAK_SetBuff(p_tx_frame->reg, reg_str);     //
    MERAK_SetBuff(p_tx_frame->data_prompt, (U8 *)"**");    //
    MERAK_SetBuff(p_tx_frame->data_len, datalen_str);//
    MERAK_SetBuff(p_tx_frame->data_data, data_str);  //
}

/*********************************************************************************                        
function: MERAK_CMD
description: MERAK
parameters: void
return: void
*********************************************************************************/
static BOOL MERAK_CMD(U8 *board_id, U8 board_num, U8 func, U8 reg, U8 *tx_data, U8 *rx_data)
{
	MERAK_FRAME *p_tx_frame;

	p_tx_frame = &MERAK_TxFrame;
	MERAK_BuildFrame(p_tx_frame, board_id, board_num, func, reg, tx_data);
    MERAK_SendFrame(p_tx_frame);
    if (!MERAK_GetAck(p_tx_frame, rx_data))
    	return FALSE;

    return TRUE;
}

/*********************************************************************************                        
function: MERAK_WriteCmd
description: MERAK
parameters: void
return: void
*********************************************************************************/
BOOL MERAK_WriteCmd(U8 *board_id, U8 board_num, U8 func, U8 reg, U8 *write_str)
{
    U8 read_data[10] ={0};
	
    if(MERAK_CMD(board_id, board_num, func, reg, write_str, read_data) == FALSE)
    {
        if(MERAK_CMD(board_id, board_num, func, reg, write_str, read_data) == FALSE)
            return FALSE;
    }
    
    if(strncmp((char * )read_data , "OK", 2) == 0)
        return TRUE;

    if(strncmp((char * )read_data , "PASS", 4) == 0)
        return TRUE;
    
    return FALSE;
}

/*********************************************************************************                        
function: MERAK_ReadCmd
description: MERAK
parameters: BOOL
return: TRUE/FALSE
*********************************************************************************/
BOOL MERAK_ReadCmd(U8 *board_id, U8 board_num, U8 func, U8 reg, U8 *read_str)
{
    if(MERAK_CMD(board_id, board_num, func, reg, (U8 *)WriteStr_EmptData, read_str) == TRUE)
        return TRUE;

    if(MERAK_CMD(board_id, board_num, func, reg, (U8 *)WriteStr_EmptData, read_str) == TRUE)
        return TRUE;

    return FALSE;
}

#if 0

void RS485_Test()
{
    while(1)
    {
        for(int i=1;i<=24;i++)
        {
            EXTIO_ConfigureBitDirction(i,0);
           // EXTIO_ReadBit(i,&Port);
            OS_Delay(2);
            EXTIO_WriteBit(i,0);
            OS_Delay(2);
            EXTIO_WriteBit(i,1);
            OS_Delay(2);
        }
        
        for(int i=0;i<3;i++)
        {
            EXTIO_ConfigureByteDirction(1+ i*8,0);         
            EXTIO_WriteByte(1+ i*8,0);
            OS_Delay(2);
            EXTIO_WriteByte(1+ i*8,0xff);
            OS_Delay(2);
        }

        EXTIO_ConfigureByteDirction(1,0);
        EXTIO_ConfigureByteDirction(9,0);
        EXTIO_ConfigureByteDirction(17,0);
        EXTIO_WriteByte(1,0);
        EXTIO_WriteByte(9,0);
        EXTIO_WriteByte(17,0);
        EXTIO_ConfigureByteDirction(1,1);
        EXTIO_ConfigureByteDirction(9,1);
        EXTIO_ConfigureByteDirction(17,1); 
        EXTIO_ReadByte(1,&Port);
        EXTIO_ReadByte(9,&Port);
        EXTIO_ReadByte(17,&Port);
    }
}

#endif    


