
#include "includes.h"

#define EEPROM_U8_ADDRESS    0x50
#define EEPROM_U18_ADDRESS   0x51

#define EEPROM_PAGE_SIZE   		8
#define EEPROM_PAGE_NUMBER   	32
#define EEPROM_DATA_LENGTH  	(EEPROM_PAGE_SIZE*EEPROM_PAGE_NUMBER)

U8 eep_data[EEPROM_DATA_LENGTH];

#define U8_BIN_FILE	    "/home/pi/gvr/eep/U8.BIN"
#define U18_BIN_FILE	"/home/pi/gvr/eep/U18.BIN"

U16 getFileBytes(FILE *fp)
{
	int currentPos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int lastPos = ftell(fp);
	fseek(fp,currentPos,SEEK_SET);
	return lastPos;
}

BOOL readFile(char * file_name, U8 * destination, U16 max_length, U16 *actual_length )
{
	FILE *pFile;
    U16 length;

    pFile = fopen(file_name, "r");
    if(pFile == NULL)
        return FALSE;

	length = getFileBytes(pFile);

    if(length > max_length)
        return FALSE;

    *actual_length = length;

    if(fread(destination, 1, length, pFile) != length)
        return FALSE;

    fclose(pFile);

    return TRUE;
}

BOOL write_to_eep(int fd)
{
	U8 i;
	U8 j;
	U8 wr_number;
    U8 page_data[EEPROM_PAGE_SIZE+1];

	for(i=0; i<EEPROM_PAGE_NUMBER; i++)	{
		page_data[0] = i*EEPROM_PAGE_SIZE;

		for(j=0;j<EEPROM_PAGE_SIZE;j++)
			page_data[1+j] = eep_data[i*EEPROM_PAGE_SIZE+j];

		wr_number = write(fd, page_data, EEPROM_PAGE_SIZE+1);
		if(wr_number != EEPROM_PAGE_SIZE+1)
			return FALSE;
		delay_ms(5);
	}
	return TRUE;
}

BOOL Burn_I2cEEPROM(void)
{
	/*char cmdBuff[24]={0};

	sprintf(cmdBuff,"%s", "epp i \"NovTech\"");
	writeCmd(cmdBuff);	
	sprintf(cmdBuff,"%s", "eep c "VeriFone"");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "epp a "M15758A00X"");
	writeCmd(cmdBuff);	
	sprintf(cmdBuff,"%s", "eep b "xxxxx"");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "eep v "xxxxx"");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "eep b "xxxxx"");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "eep p 1");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "eep n "xxxxx"");
	writeCmd(cmdBuff);
	sprintf(cmdBuff,"%s", "eep save");
	writeCmd(cmdBuff);*/
	
}

