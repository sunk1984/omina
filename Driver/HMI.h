
#ifndef _HMI_H_
#define _HMI_H_

extern BOOL HMI_PassBuzz(void);
extern BOOL HMI_FailBuzz(void);
extern BOOL HMI_LongWarnBuzz(void);
extern BOOL HMI_LongWarnBuzzOff(void);
extern BOOL HMI_ShortWarnBuzz(void);

extern BOOL HMI_OnRunLed(void);
extern BOOL HMI_OffRunLed(void);
extern BOOL HMI_FlashRunLed(void);

extern BOOL HMI_OnFailLed(void);
extern BOOL HMI_OffFailLed(void);

extern BOOL HMI_OnPassLed(void);
extern BOOL HMI_OffPassLed(void);

extern BOOL HMI_PressYesKey(void);
extern BOOL HMI_PressNoKey(void);
extern BOOL HMI_PressFuncKey(void);

extern void HMI_ShowPass(void);
extern void HMI_ShowFail(void);

#define LCD_ALL_LINE	(0)

#define LCD_LINE1		(1)
#define LCD_LINE2		(2)

extern void LCD_DisplayALine(U8 line, U8 *str);
extern void LCD_Display2Line(U8 startline, U8 *str);
extern void LCD_Clear(U8 line);
extern void LCD_DisplayAItem(U8 *str);
extern void LCD_DisplayResult(U32 res);

#endif//_HMI_H_
