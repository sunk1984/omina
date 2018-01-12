
#include "includes.h"
#include <linux/input.h>

typedef struct {
	char key;
	char ch;
}KEYTOCH;

KEYTOCH keyTable[] = {
		{KEY_1, '1'},
		{KEY_2, '2'},
		{KEY_3, '3'},
		{KEY_4, '4'},
		{KEY_5, '5'},
		{KEY_6, '6'},
		{KEY_7, '7'},
		{KEY_8, '8'},
		{KEY_9, '9'},
		{KEY_0, '0'},
		{KEY_MINUS, '-'},
		{KEY_A, 'A'},
		{KEY_B, 'B'},
		{KEY_C, 'C'},
		{KEY_D, 'D'},
		{KEY_E, 'E'},
		{KEY_F, 'F'},
		{KEY_G, 'G'},
		{KEY_H, 'H'},
		{KEY_I, 'I'},
		{KEY_J, 'J'},
		{KEY_K, 'K'},
		{KEY_L, 'L'},
		{KEY_M, 'M'},
		{KEY_N, 'N'},
		{KEY_O, 'O'},
		{KEY_P, 'P'},
		{KEY_Q, 'Q'},
		{KEY_R, 'R'},
		{KEY_S, 'S'},
		{KEY_T, 'T'},
		{KEY_U, 'U'},
		{KEY_V, 'V'},
		{KEY_W, 'W'},
		{KEY_X, 'X'},
		{KEY_Y, 'Y'},
		{KEY_Z, 'Z'},
};

char keyToCh(char key)
{
	int i;
	for (i = 0; i < sizeof(keyTable)/sizeof(KEYTOCH); i++) {
		if (key == keyTable[i].key) {
			return keyTable[i].ch;
		}
	}
	return 0;
}

int SCANGUN_GetBarCode(char *codeStr, int length)
{
	int i;
	int fd;
	int rd;
	char count=0;
	char keychar;
	char barcode[48]={0};
	struct input_event  ev[64];

	fd = open("/dev/input/event0", O_RDONLY);
	if (fd < 0) {
		printf("Open scangun failed.\n");
		return 0;
	}

	while(1) {
		rd = read(fd, ev, sizeof(ev));
		if (rd < sizeof(struct input_event)) {
			printf("expected %d byte, got %d\n", (int)sizeof(struct input_event), rd);
		}
		for (i = 0; i< rd/sizeof(struct input_event); i++) {
			U32 type, code, value;
			type  = ev[i].type;
			code  = ev[i].code;
			value = ev[i].value;
//			if (type == EV_KEY)
//				printf("type:%d code:%d value:%d\n", type, code, value);
			if (type == EV_KEY && value == 0) {
				keychar = keyToCh(code);
				if (keychar) {
					barcode[count++] = keychar;
				}
			}
		}
		if (count >= length) {
			printf("barcode: %s\n", barcode);
			close(fd);
			strncpy(codeStr, barcode, length);
			return 1;
		}
	}
}
