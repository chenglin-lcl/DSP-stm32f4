#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f4xx.h"


void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(void);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);//²»¿ÉÓÃ
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void Write_DataBuffer(void);
void OLED_DrawPoint(uint8_t x,uint8_t y);
void OLED_DrawFw(uint8_t x,uint8_t y);//»­ÆµÆ×
void newCLS(void);

#endif
