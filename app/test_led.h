/*
 * test_led.h
 *
 *  Created on: Jan 13, 2018
 *      Author: test
 */

#ifndef APP_TEST_LED_H_
#define APP_TEST_LED_H_

enum led_color {

	LED_COLOR_RED,
	LED_COLOR_GREEN,
	LED_COLOR_BLUE,
	LED_COLOR_AMBER,
	LED_COLOR_YELLOW,
	LED_COLOR_ORANGE,
};


typedef void (*LED_FUNC)();
typedef struct {
	U8 title[40];
	U8 sensor_chan;
	U8 color;
	U8 is_flash;
    LED_FUNC LedStartFunc;
    LED_FUNC LedEndFunc;

} LED_ITEM_T, *P_LED_ITEM_T;

#endif /* APP_TEST_LED_H_ */
