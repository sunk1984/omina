
#ifndef _RS485_H_
#define _RS485_H_

#include "includes.h"

#pragma pack(1)

typedef struct
{
	U8 prompt[2];
	U8 len[2];
	U8 data[26];

} MERAK_DATA_T, *P_MERAK_DATA_T;

typedef struct
{
    U8 	start[1];    //
    U8 	id[3];       //
    U8 	num[2];      //
    U8 	func[2];     //
    U8 	reg[2];     //
	U8  data_prompt[2];
	U8  data_len[2];
	U8  data_data[26];
    U8 	chk[2];     //
    U8 	end[2];     //
  
} MERAK_FRAME;

#pragma pack()


extern void Hex2Str(U8 * str, U8 hex_data);
extern BOOL MERAK_WriteCmd(U8 * board_id, U8 board_num, U8 func, U8 reg, U8 * write_str);
extern BOOL MERAK_ReadCmd(U8 * board_id, U8 board_num, U8 func, U8 reg, U8 * read_str);
extern void MERAK_ResetALL(void);

#endif


