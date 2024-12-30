#ifndef _EPD_H_
#define _EPD_H_

#include <stdint.h>
#include "stm32f10x.h"
/*参数宏定义*********************/

/*FontSize参数取值*/
/*此参数值不仅用于判断，而且用于计算横向字符偏移，默认值为字体像素宽度*/
#define EPD_8X16				8
#define EPD_6X8				    6

/*IsFilled参数数值*/
#define EPD_UNFILLED			0
#define EPD_FILLED				1

/*********************参数宏定义*/


void EPD_Init(void);

void EPD_Update(void);
void EPD_Clear(void);
void EPD_Reverse(void);

void EPD_Test(uint16_t Page,uint16_t X,uint8_t Data);
void EPD_Test_Image(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint8_t *Image);

void EPD_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void EPD_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void EPD_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void EPD_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void EPD_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void EPD_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void EPD_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void EPD_ShowChinese(int16_t X, int16_t Y, char *Chinese);
void EPD_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);

#endif
