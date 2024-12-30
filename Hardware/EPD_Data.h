#ifndef __EPD_DATA_H
#define __EPD_DATA_H

#include <stdint.h>

/*中文字符字节宽度*/
#define EPD_CHN_CHAR_WIDTH			3		//UTF-8编码格式给3，GB2312编码格式给2

/*字模基本单元*/
typedef struct 
{
	char Index[EPD_CHN_CHAR_WIDTH + 1];	//汉字索引
	uint8_t Data[32];						//字模数据
} ChineseCell_t;

/*ASCII字模数据声明*/
extern const uint8_t EPD_F8x16[][16];
extern const uint8_t EPD_F6x8[][6];
/*汉字字模数据声明*/
extern const ChineseCell_t EPD_CF16x16[];

/*图像数据声明*/
extern const uint8_t Diode[];
extern const uint8_t W_arrow[];
extern const uint8_t S_arrow[];
extern const uint8_t A_arrow[];
extern const uint8_t D_arrow[];
/*按照上面的格式，在这个位置加入新的图像数据声明*/
//...

#endif


/*****************江协科技|版权所有****************/
/*****************jiangxiekeji.com*****************/
