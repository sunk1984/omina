
#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //
#include <string.h>
#include <semaphore.h> //sem lib
#include <pthread.h>  //thread lib
#include <fcntl.h>
#include <termios.h>
#include <stdarg.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <ctype.h>

#include "type.h"
#include "LogFile.h"

#include "Power.h"
#include "RS485.h"
#include "Relay.h"
#include "ExtIO.h"
#include "Audio.h"
#include "HMI.h"
#include "serial.h"
#include "ADC.h"

#include "app.h"
//#include "bcm2835.h"

#endif//_INCLUDES_H_
