
#include "includes.h"

#define LOG_LEN_1_PCS           (15000)
#define LOG_FILE_CUT_MAX        (400000)
#define LOG_FILE_TOTAL_MAX      (1000000)

#define LOG_FILE	"/home/pi/Phecda/fct_files/TestLog.txt"

static U8 logStr[LOG_LEN_1_PCS] = {0};

void LOGFILE_AddItem(U8 *str)
{
	strcat((char *)logStr, (char *)str);
}

void LOGFILE_Write(void)
{
	U32 pos;
	FILE *fb;

	static char fileStr[LOG_FILE_TOTAL_MAX];

	fb = fopen(LOG_FILE,"a+");
	if(fb)
	{
		fwrite(logStr, 1, strlen((char *)logStr), fb);

		fseek(fb, 0, SEEK_END);
		pos = ftell(fb);
		if(pos > LOG_FILE_TOTAL_MAX-LOG_LEN_1_PCS) {
			fseek(fb, 0, SEEK_SET);
			fread(fileStr, 1, pos, fb);
			fseek(fb, 0, SEEK_SET);
			fwrite(fileStr+LOG_FILE_CUT_MAX, pos-LOG_FILE_CUT_MAX, 1, fb);
			feof(fb);
		}
	    fclose(fb);
    }
	memset((char * )logStr, 0, sizeof(logStr));
}



