/*
 * i2c_pca9537.c
 *
 *  Created on: Jun 13, 2017
 *      Author: test
 */

#include "includes.h"

#define I2C_CHIP_PCA9537_ADDRESS   0x49

#define PCA9537_CMD_READ   			0
#define PCA9537_CMD_WRITE   		1
#define PCA9537_CMD_CONFIG   		3

#define PCA9537_REG_DIR_0011   		3	// bit3~bit0:0011

static BOOL PCA9537_SetDirection(int fd)
{
    U8 cnt;
    U8 wr_data[4];

    wr_data[0] = PCA9537_CMD_CONFIG;
    wr_data[1] = PCA9537_REG_DIR_0011;
	cnt = write(fd, wr_data, 2);

    if(cnt == 2)
    	return TRUE;
    else
        return FALSE;
}

static BOOL PCA9537_ReadPort(int fd, U8 * read_data)
{
    U8 cnt;
    U8 rd_addr[1] = { PCA9537_CMD_READ };

	write(fd, rd_addr, 1);
    cnt = read(fd, read_data, 1);		// *read_data??
    if(cnt == 1)
    	return TRUE;
    else
        return FALSE;
}

static BOOL TestPCA9537ReadPort(int fd)
{
	U8 read_data;

	if(PCA9537_ReadPort(fd, &read_data) == FALSE)
        return FALSE;

	Dprintf("Read port %02x!\n", read_data);
    if((read_data&0x03) != 0x00)	// input ports should be 0
        return FALSE;

    return TRUE;
}

static BOOL PCA9537_WritePort(int fd, U8 write_data)
{
    U8 cnt;
    U8 wr_data[4];

    wr_data[0] = PCA9537_CMD_WRITE;
    wr_data[1] = write_data;

	cnt = write(fd, wr_data, 2);

    if(cnt == 2)
    	return TRUE;
    else
        return FALSE;
}

BOOL TestExtIO(U8 chan, U8 desired_data)
{
    U8 readData;

    EXTIO_ConfigureBitDirction(chan, IO_INPUT);
    EXTIO_ReadBit(chan, &readData);

    if(readData == desired_data)
        return TRUE;
    else
    	return FALSE;
}

static BOOL TestPCA9537WritePort(int fd)
{

	if(PCA9537_WritePort(fd, 0x04) == FALSE)
        return FALSE;

//    if(TestExtIO(IOB_PCA9537_P2_LED, 1) == FALSE)
//        return FALSE;

    if(PCA9537_WritePort(fd, 0x00) == FALSE)
        return FALSE;

//    if(TestExtIO(IOB_PCA9537_P2_LED, 0) == FALSE)
//        return FALSE;

    if(PCA9537_WritePort(fd, 0x08) == FALSE)
        return FALSE;

//    if(TestExtIO(IOB_PCA9537_P3_PCNRST, 1) == FALSE)
//        return FALSE;

    if(PCA9537_WritePort(fd, 0x00) == FALSE)
        return FALSE;

//    if(TestExtIO(IOB_PCA9537_P3_PCNRST, 0) == FALSE)
//        return FALSE;

	return TRUE;
}

static BOOL TEST_I2C_PCA9537(void)
{
	int fd;

	GUI_SendMessage("I2C PCA9537 test start!\r\n");

    fd = open("/dev/i2c-1", O_RDWR);
    if(fd < 0)  {
    	Dprintf("Can't open i2c device!\n");
    	return FAIL;
    }

    if(ioctl(fd, I2C_TENBIT, 0) < 0)  {
    	Dprintf("Can't config i2c to 7 bits!\n");
    	return FAIL;
    }
    if(ioctl(fd, I2C_SLAVE, I2C_CHIP_PCA9537_ADDRESS) < 0)  {
    	Dprintf("Can't config i2c slave address!\n");
    	return FAIL;
    }

    if(PCA9537_SetDirection(fd) == FALSE)
    	return FAIL;

    if(TestPCA9537ReadPort(fd) == FALSE)
    	return FAIL;

    if(TestPCA9537WritePort(fd) == FALSE)
    	return FAIL;

    GUI_SendMessage("I2C test OK!\r\n");
    return PASS;

}

void TestItem_I2C(U8 * test_result)
{
//	RLY_ON(RLY_I2C_PCA9537_SDA);
//	RLY_ON(RLY_I2C_PCA9537_SCL);

	* test_result = TEST_I2C_PCA9537();

//	RLY_OFF(RLY_I2C_PCA9537_SDA);
//	RLY_OFF(RLY_I2C_PCA9537_SCL);
}

