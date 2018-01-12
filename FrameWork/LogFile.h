

#ifndef _LOG_FILE_H_
#define _LOG_FILE_H_

extern void LOGFILE_Write(void);
extern void LOGFILE_AddItem(U8 * str);

extern void Dprintf(char *lpszFormat, ...);
extern void delay_ms(U32 ms);


#endif


