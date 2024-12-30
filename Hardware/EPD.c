#include "stm32f10x.h"
#include "EPD.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "Delay.h"
#include "EPD_Data.h"

/*宏定义*********************/
#define EPD_SCL 	GPIO_Pin_0
#define EPD_SDA 	GPIO_Pin_1
#define EPD_RES 	GPIO_Pin_2
#define EPD_DC  	GPIO_Pin_3
#define EPD_CS  	GPIO_Pin_4
#define EPD_BUSY 	GPIO_Pin_5


/*********************宏定义*/

/*全局变量*********************/

/**
  * EPD显存数组
  * 所有的显示函数，都只是对此显存数组进行读写
  * 随后调用EPD_Update函数或EPD_UpdateArea函数
  * 才会将显存数组的数据发送到EPD硬件，进行显示
  */
uint8_t EPD_DisplayBuf[16][248];

/*********************全局变量*/


/*引脚配置*********************/

/**
  * 函    数：EPD写D0（CLK）高低电平
  * 参    数：要写入D0的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写D0时，此函数会被调用
  *           用户需要根据参数传入的值，将D0置为高电平或者低电平
  *           当参数传入0时，置D0为低电平，当参数传入1时，置D0为高电平
  */
void EPD_W_D0(uint8_t BitValue)
{
	/*根据BitValue的值，将D0置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, EPD_SCL, (BitAction)BitValue);
}

/**
  * 函    数：EPD写D1（MOSI/SDA）高低电平
  * 参    数：要写入D1的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写D1时，此函数会被调用
  *           用户需要根据参数传入的值，将D1置为高电平或者低电平
  *           当参数传入0时，置D1为低电平，当参数传入1时，置D1为高电平
  */
void EPD_W_D1(uint8_t BitValue)
{
	/*根据BitValue的值，将D1置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, EPD_SDA, (BitAction)BitValue);
}

/**
  * 函    数：EPD写RES高低电平
  * 参    数：要写入RES的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写RES时，此函数会被调用
  *           用户需要根据参数传入的值，将RES置为高电平或者低电平
  *           当参数传入0时，置RES为低电平，当参数传入1时，置RES为高电平
  */
void EPD_W_RES(uint8_t BitValue)
{
	/*根据BitValue的值，将RES置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, EPD_RES, (BitAction)BitValue);
}

/**
  * 函    数：EPD写DC高低电平
  * 参    数：要写入DC的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写DC时，此函数会被调用
  *           用户需要根据参数传入的值，将DC置为高电平或者低电平
  *           当参数传入0时，置DC为低电平，当参数传入1时，置DC为高电平
  */
void EPD_W_DC(uint8_t BitValue)
{
	/*根据BitValue的值，将DC置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, EPD_DC, (BitAction)BitValue);
}

/**
  * 函    数：EPD写CS高低电平
  * 参    数：要写入CS的电平值，范围：0/1
  * 返 回 值：无
  * 说    明：当上层函数需要写CS时，此函数会被调用
  *           用户需要根据参数传入的值，将CS置为高电平或者低电平
  *           当参数传入0时，置CS为低电平，当参数传入1时，置CS为高电平
  */
void EPD_W_CS(uint8_t BitValue)
{
	/*根据BitValue的值，将CS置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, EPD_CS, (BitAction)BitValue);
}

/**
  * 函    数：EPD引脚初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：当上层函数需要初始化时，此函数会被调用
  *           用户需要将D0、D1、RES、DC和CS引脚初始化为推挽输出模式
  */
void EPD_GPIO_Init(void)
{
	uint32_t i, j;
	
	/*在初始化前，加入适量延时，待EPD供电稳定*/
	for (i = 0; i < 1000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
	
	/*将D0、D1、RES、DC和CS引脚初始化为推挽输出模式*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = EPD_SCL|EPD_SDA|EPD_RES|EPD_DC|EPD_CS;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = EPD_BUSY;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*置引脚默认电平*/
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0);
}

/*********************引脚配置*/


/*通信协议*********************/

/**
  * 函    数：SPI发送一个字节
  * 参    数：Byte 要发送的一个字节数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void EPD_SPI_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	/*循环8次，主机依次发送数据的每一位*/
	for (i = 0; i < 8; i++)
	{

		/*使用掩码的方式取出Byte的指定一位数据并写入到D1线*/
		/*两个!的作用是，让所有非零的值变为1*/
		EPD_W_D0(0);	//拉低D0，主机开始发送下一位数据
		EPD_W_D1(!!(Byte & (0x80 >> i)));
		EPD_W_D0(1);	//拉高D0，从机在D0上升沿读取SDA
	}

}

/**
  * 函    数：EPD写命令
  * 参    数：Command 要写入的命令值，范围：0x00~0xFF
  * 返 回 值：无
  */
void EPD_WriteCommand(uint8_t Command)
{
	EPD_W_CS(0);					//拉低CS，开始通信
	EPD_W_DC(0);					//拉低DC，表示即将发送命令
	EPD_SPI_SendByte(Command);		//写入指定命令
	EPD_W_DC(1);
	EPD_W_CS(1);					//拉高CS，结束通信
}

/**
  * 函    数：EPD写数据
  * 参    数：Data 要写入的数据
  * 返 回 值：无
  */
void EPD_WriteData(uint8_t Data)
{
	EPD_W_CS(0);					//拉低CS，开始通信
	EPD_W_DC(1);					//拉高DC，表示即将发送数据
	EPD_SPI_SendByte(Data);			//依次发送Data数据
	EPD_W_CS(1);					//拉高CS，结束通信
}


/**
  * 函    数：EPD写数据
  * 参    数：Data 要写入数据的起始地址
  * 参    数：Count 要写入数据的数量
  * 返 回 值：无
  */
void EPD_WriteDatas(uint8_t *Data, uint16_t Count)
{
	uint8_t i;
	
	EPD_W_CS(0);					//拉低CS，开始通信
	EPD_W_DC(1);					//拉高DC，表示即将发送数据
	/*循环Count次，进行连续的数据写入*/
	for (i = 0; i < Count; i++)
	{
		EPD_SPI_SendByte(Data[i]);	//依次发送Data的每一个数据
	}
	EPD_W_CS(1);					//拉高CS，结束通信
}

/*********************通信协议*/

/*busy线*********************/

void EPD_WaitBusy(void)
{
	uint8_t x=1;
	while(1)
	{
		x=GPIO_ReadInputDataBit(GPIOA,EPD_BUSY);
		if(x==0)
		{
			Delay_ms(10);
			break;
		}
	}
}

/*********************busy线*/



/*硬件配置*********************/

/**
  * 函    数：EPD初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：使用前，需要调用此初始化函数
  */
void EPD_Init(void)
{
	EPD_GPIO_Init();
	EPD_W_RES(0);
	Delay_ms(15);
	EPD_W_RES(1);
	Delay_ms(15);

	EPD_WaitBusy();
	EPD_WriteCommand(0x12);				//软件复位EPD
	EPD_WaitBusy();

	EPD_WriteCommand(0x01);				//设置输出尺寸
	EPD_WriteData(0xF7);				//[A7:A0]
	EPD_WriteData(0x00);				//[A8],[A8:A0]为X轴上的输出宽度
	EPD_WriteData(0x00);				//[B2:B0]设置扫描顺序

	EPD_WriteCommand(0x11);				//设置数据输入顺序
	EPD_WriteData(0x04);
	/*
	*/
	EPD_WriteCommand(0x44);				//设置RAM的X上的起始位和终止位
	EPD_WriteData(0x0E);
	EPD_WriteData(0x03);

	EPD_WriteCommand(0x45);				//设置RAM的Y上的起始位和终止位
	EPD_WriteData(0xF7);
	EPD_WriteData(0x00);
	EPD_WriteData(0x00);
	EPD_WriteData(0x00);

	EPD_WriteCommand(0x3C);				//设置边界波形控制
	EPD_WriteData(0xC0);

	EPD_WriteCommand(0x2C);				//设置VCOM值
	EPD_WriteData(0x70);

	EPD_WriteCommand(0x03);				//设置栅极驱动电压
	EPD_WriteData(0x17);

	EPD_WriteCommand(0x04);				//设置源极驱动电压
	EPD_WriteData(0x41);				//VSH1
	EPD_WriteData(0x00);				//VSH2
	EPD_WriteData(0x32);				//VSL
	
	EPD_WriteCommand(0x32);
	uint8_t i;
	for( i=0;i<224;i++)
	{
		EPD_WriteData(0xFF);
	}
	EPD_WaitBusy();

}

void EPD_DisplaySet(uint8_t XStart,uint8_t XStop,uint8_t XCount,
					uint8_t YStart,uint8_t YStop,uint8_t YCount,uint8_t Mode)
{
	EPD_WriteCommand(0x11);				//设置数据输入顺序
	EPD_WriteData(Mode);

	EPD_WriteCommand(0x44);				//设置RAM的X上的起始位和终止位
	EPD_WriteData(XStart);
	EPD_WriteData(XStop);

	EPD_WriteCommand(0x45);				//设置RAM的Y上的起始位和终止位
	EPD_WriteData(YStart);
	EPD_WriteData(0x00);
	EPD_WriteData(YStop);
	EPD_WriteData(0x00);

	EPD_WriteCommand(0x4E);				//X输出计数器
	EPD_WriteData(XCount);

	EPD_WriteCommand(0x4F);				//Y输出计数器
	EPD_WriteData(YCount);
	EPD_WriteData(0x00);

}
/**
  * 函    数：EPD设置显示光标位置
  * 参    数：Page 指定光标所在的页，范围：0~15(实际上第15面只有1/4面)
  * 参    数：X 指定光标所在的X轴坐标，范围：0~250
  * 返 回 值：无
  * 说    明：这里设置EPD为默认的X轴，只能8个Bit为一组写入，即1页等于250个X轴坐标
  */
void EPD_SetCursor(uint16_t X,uint16_t Length)
{	
	/*通过指令设置列地址*/
	EPD_WriteCommand(0x0F);		//起始位
	EPD_WriteData(248-X);
	EPD_WriteCommand(0x01);		//数据长度
	EPD_WriteData(Length);
}

/*********************硬件配置*/

/*工具函数*********************/

/*工具函数仅供内部部分函数使用*/

/**
  * 函    数：次方函数
  * 参    数：X 底数
  * 参    数：Y 指数
  * 返 回 值：等于X的Y次方
  */
uint32_t EPD_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

/**
  * 函    数：判断指定点是否在指定多边形内部
  * 参    数：nvert 多边形的顶点数
  * 参    数：vertx verty 包含多边形顶点的x和y坐标的数组
  * 参    数：testx testy 测试点的X和y坐标
  * 返 回 值：指定点是否在指定多边形内部，1：在内部，0：不在内部
  */
uint8_t EPD_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
	int16_t i, j, c = 0;
	
	/*此算法由W. Randolph Franklin提出*/
	/*参考链接：https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
  * 函    数：判断指定点是否在指定角度内部
  * 参    数：X Y 指定点的坐标
  * 参    数：StartAngle EndAngle 起始角度和终止角度，范围：-180~180
  *           水平向右为0度，水平向左为180度或-180度，下方为正数，上方为负数，顺时针旋转
  * 返 回 值：指定点是否在指定角度内部，1：在内部，0：不在内部
  */
uint8_t EPD_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
	int16_t PointAngle;
	PointAngle = atan2(Y, X) / 3.14 * 180;	//计算指定点的弧度，并转换为角度表示
	if (StartAngle < EndAngle)	//起始角度小于终止角度的情况
	{
		/*如果指定角度在起始终止角度之间，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle && PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	else			//起始角度大于于终止角度的情况
	{
		/*如果指定角度大于起始角度或者小于终止角度，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle || PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	return 0;		//不满足以上条件，则判断判定指定点不在指定角度
}

/*********************工具函数*/

/**
  * 函    数：将EPD显存数组更新到EPD屏幕
  * 参    数：无
  * 返 回 值：无
  * 说    明：所有的显示函数，都只是对EPD显存数组进行读写
  *           随后调用EPD_Update函数或EPD_UpdateArea函数
  *           才会将显存数组的数据发送到EPD硬件，进行显示
  *           故调用显示函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_Update(void)
{
	EPD_DisplaySet(0,15,15,0,247,247,7);
	EPD_WriteCommand(0x24);	//黑白RAM
	uint16_t Page,X;
	for(Page=0;Page<16;Page++)
	{
		for(X=0;X<248;X++)
		{
			EPD_WriteData(EPD_DisplayBuf[Page][X]);
		}
	}
	EPD_WriteCommand(0x22);			//设置更新
	EPD_WriteData(0xF7);			//选择模式一
	EPD_WriteCommand(0x20);
	EPD_WaitBusy();
}

/**
  * 函    数：将EPD显存数组全部清零
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_Clear(void)
{
	uint8_t Page,X;
	for(Page=0;Page<16;Page++)
	{
		for(X=0;X<248;X++)
		{
			EPD_DisplayBuf[Page][X]=0x00;
		}
	}
}
/**
  * 函    数：将EPD显存数组部分清零
  * 参    数：X 指定区域左上角的横坐标，范围：-32768~32767，屏幕区域：0~247
  * 参    数：Y 指定区域左上角的纵坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Width 指定区域的宽度，范围：0~247
  * 参    数：Height 指定区域的高度，范围：0~127
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//遍历指定页
	{
		for (i = X; i < X + Width; i ++)	//遍历指定列
		{
			if (i >= 0 && i <=247 && j >=0 && j <= 127)				//超出屏幕的内容不显示
			{
				EPD_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));	//将显存数组指定数据清零0x01
			}
		}
	}
}

/**
  * 函    数：将EPD显存数组全部取反
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_Reverse(void)
{
	uint8_t i, j;
	for (j = 0; j < 16; j ++)				//遍历8页
	{
		for (i = 0; i < 247; i ++)			//遍历128列
		{
			EPD_DisplayBuf[j][i] ^= 0xFF;	//将显存数组数据全部取反
		}
	}
}
	
/**
  * 函    数：将EPD显存数组部分取反
  * 参    数：X 指定区域左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定区域左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Width 指定区域的宽度，范围：0~128
  * 参    数：Height 指定区域的高度，范围：0~64
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//遍历指定页
	{
		for (i = X; i < X + Width; i ++)	//遍历指定列
		{
			if (i >= 0 && i <= 247 && j >=0 && j <= 127)			//超出屏幕的内容不显示
			{
				EPD_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8);	//将显存数组指定数据取反
			}
		}
	}
}


/**
  * 函    数：EPD显示图像
  * 参    数：X 指定图像左上角的横坐标，范围：-32768~32767，屏幕区域：0~247
  * 参    数：Y 指定图像左上角的纵坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Width 指定图像的宽度，范围：0~247
  * 参    数：Height 指定图像的高度，范围：0~127
  * 参    数：Image 指定要显示的图像
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i = 0, j = 0;
	int16_t Page, Shift;
	X=X;
	Y=128-Y-Height;
	/*将图像所在区域清空*/
	EPD_ClearArea(X, Y, Width, Height);
	
	/*遍历指定图像涉及的相关页*/
	/*(Height - 1) / 8 + 1的目的是Height / 8并向上取整*/
	for (j = 0; j < (Height - 1) / 8 + 1; j ++)
	{
		/*遍历指定图像涉及的相关列*/
		for (i = 0; i < Width; i ++)
		{
			if (X + i >= 0 && X + i <= 248)		//超出屏幕的内容不显示
			{
				/*负数坐标在计算页地址和移位时需要加一个偏移*/
				Page = Y / 8;
				Shift = Y % 8;
				if (Y < 0)
				{
					Page -= 1;
					Shift += 8;
				}
				
				if (Page - j >= 0 && Page - j <= 16)		//超出屏幕的内容不显示
				{
					/*显示图像在当前页的内容*/
					EPD_DisplayBuf[Page - j][X + i] |= Image[j * Width + i] << (Shift);
				}
				
				if (Page - j - 1 >= 0 && Page - j - 1 <= 16)		//超出屏幕的内容不显示
				{					
					/*显示图像在下一页的内容*/
					EPD_DisplayBuf[Page - j - 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
				}
			}
		}
	}
}



/**
  * 函    数：EPD显示一个字符
  * 参    数：X 指定字符左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定字符左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Char 指定要显示的字符，范围：ASCII码可见字符
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize)
{
	if (FontSize == EPD_8X16)		//字体为宽8像素，高16像素
	{
		/*将ASCII字模库EPD_F8x16的指定数据以8*16的图像格式显示*/
		EPD_ShowImage(X, Y, 8, 16, EPD_F8x16[Char - ' ']);
	}
	else if(FontSize == EPD_6X8)	//字体为宽6像素，高8像素
	{
		/*将ASCII字模库EPD_F6x8的指定数据以6*8的图像格式显示*/
		EPD_ShowImage(X, Y, 6, 8, EPD_F6x8[Char - ' ']);
	}
}

/**
  * 函    数：EPD显示字符串
  * 参    数：X 指定字符串左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定字符串左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：String 指定要显示的字符串，范围：ASCII码可见字符组成的字符串
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)		//遍历字符串的每个字符
	{
		/*调用EPD_ShowChar函数，依次显示每个字符*/
		EPD_ShowChar(X + i * FontSize, Y, String[i], FontSize);
	}
}

/**
  * 函    数：EPD显示数字（十进制，正整数）
  * 参    数：X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Number 指定要显示的数字，范围：0~4294967295
  * 参    数：Length 指定数字的长度，范围：0~10
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位							
	{
		/*调用EPD_ShowChar函数，依次显示每个数字*/
		/*Number / EPD_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		EPD_ShowChar(X + i * FontSize, Y, Number / EPD_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * 函    数：EPD显示有符号数字（十进制，整数）
  * 参    数：X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Number 指定要显示的数字，范围：-2147483648~2147483647
  * 参    数：Length 指定数字的长度，范围：0~10
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//数字大于等于0
	{
		EPD_ShowChar(X, Y, '+', FontSize);	//显示+号
		Number1 = Number;					//Number1直接等于Number
	}
	else									//数字小于0
	{
		EPD_ShowChar(X, Y, '-', FontSize);	//显示-号
		Number1 = -Number;					//Number1等于Number取负
	}
	
	for (i = 0; i < Length; i++)			//遍历数字的每一位								
	{
		/*调用EPD_ShowChar函数，依次显示每个数字*/
		/*Number1 / EPD_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		EPD_ShowChar(X + (i + 1) * FontSize, Y, Number1 / EPD_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * 函    数：EPD显示十六进制数字（十六进制，正整数）
  * 参    数：X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * 参    数：Length 指定数字的长度，范围：0~8
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)		//遍历数字的每一位
	{
		/*以十六进制提取数字的每一位*/
		SingleNumber = Number / EPD_Pow(16, Length - i - 1) % 16;
		
		if (SingleNumber < 10)			//单个数字小于10
		{
			/*调用EPD_ShowChar函数，显示此数字*/
			/*+ '0' 可将数字转换为字符格式*/
			EPD_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
		}
		else							//单个数字大于10
		{
			/*调用EPD_ShowChar函数，显示此数字*/
			/*+ 'A' 可将数字转换为从A开始的十六进制字符*/
			EPD_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * 函    数：EPD显示二进制数字（二进制，正整数）
  * 参    数：X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * 参    数：Length 指定数字的长度，范围：0~16
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位	
	{
		/*调用EPD_ShowChar函数，依次显示每个数字*/
		/*Number / EPD_Pow(2, Length - i - 1) % 2 可以二进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		EPD_ShowChar(X + i * FontSize, Y, Number / EPD_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * 函    数：EPD显示浮点数字（十进制，小数）
  * 参    数：X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Number 指定要显示的数字，范围：-4294967295.0~4294967295.0
  * 参    数：IntLength 指定数字的整数位长度，范围：0~10
  * 参    数：FraLength 指定数字的小数位长度，范围：0~9，小数进行四舍五入显示
  * 参    数：FontSize 指定字体大小
  *           范围：EPD_8X16		宽8像素，高16像素
  *                 EPD_6X8		宽6像素，高8像素
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		EPD_ShowChar(X, Y, '+', FontSize);	//显示+号
	}
	else									//数字小于0
	{
		EPD_ShowChar(X, Y, '-', FontSize);	//显示-号
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部分*/
	IntNum = Number;						//直接赋值给整型变量，提取整数
	Number -= IntNum;						//将Number的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	PowNum = EPD_Pow(10, FraLength);		//根据指定小数的位数，确定乘数
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示误差
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	/*显示整数部分*/
	EPD_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	/*显示小数点*/
	EPD_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	/*显示小数部分*/
	EPD_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

/**
  * 函    数：EPD显示汉字串
  * 参    数：X 指定汉字串左上角的横坐标，范围：-32768~32767，屏幕区域：0~127
  * 参    数：Y 指定汉字串左上角的纵坐标，范围：-32768~32767，屏幕区域：0~63
  * 参    数：Chinese 指定要显示的汉字串，范围：必须全部为汉字或者全角字符，不要加入任何半角字符
  *           显示的汉字需要在EPD_Data.c里的EPD_CF16x16数组定义
  *           未找到指定汉字时，会显示默认图形（一个方框，内部一个问号）
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void EPD_ShowChinese(int16_t X, int16_t Y, char *Chinese)
{
	uint8_t pChinese = 0;
	uint8_t pIndex;
	uint8_t i;
	char SingleChinese[EPD_CHN_CHAR_WIDTH + 1] = {0};
	
	for (i = 0; Chinese[i] != '\0'; i ++)		//遍历汉字串
	{
		SingleChinese[pChinese] = Chinese[i];	//提取汉字串数据到单个汉字数组
		pChinese ++;							//计次自增
		
		/*当提取次数到达EPD_CHN_CHAR_WIDTH时，即代表提取到了一个完整的汉字*/
		if (pChinese >= EPD_CHN_CHAR_WIDTH)
		{
			pChinese = 0;		//计次归零
			
			/*遍历整个汉字字模库，寻找匹配的汉字*/
			/*如果找到最后一个汉字（定义为空字符串），则表示汉字未在字模库定义，停止寻找*/
			for (pIndex = 0; strcmp(EPD_CF16x16[pIndex].Index, "") != 0; pIndex ++)
			{
				/*找到匹配的汉字*/
				if (strcmp(EPD_CF16x16[pIndex].Index, SingleChinese) == 0)
				{
					break;		//跳出循环，此时pIndex的值为指定汉字的索引
				}
			}
			
			/*将汉字字模库EPD_CF16x16的指定数据以16*16的图像格式显示*/
			EPD_ShowImage(X + ((i + 1) / EPD_CHN_CHAR_WIDTH - 1) * 16, Y, 16, 16, EPD_CF16x16[pIndex].Data);
		}
	}
}

// /*测试函数*********************/
// const uint8_t EPD_TestArr[]={
// 0xFF,0x80,0x87,0x88,0x92,0xA4,0xA2,0xA0,0xA2,0xA4,0x92,0x88,0x87,0x80,0xFF,
// 0xFE,0x02,0xC2,0x22,0x12,0x8A,0x4A,0x4A,0x4A,0x8A,0x12,0x22,0xC2,0x02,0xFE,
// };

// void EPD_Test(uint16_t Page,uint16_t X,uint8_t Data)
// {
// 	uint16_t i;
// 	for(i=X;i<X+6;i++)
// 	{
// 		EPD_DisplayBuf[Page][i]|=EPD_My6X8[Data-'A'][i-X];
// 		//EPD_DisplayBuf[Page][i]|=EPD_F6x8[Data-' '][i-X];
// 	}
// }

// void EPD_Test_Image(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint8_t *Image)
// {
// 	for(uint8_t i=0;i<Width;i++)
// 	{
// 		EPD_DisplayBuf[Y/8-1][X+i]|=EPD_TestArr[i]<<(Y%8);
// 		EPD_DisplayBuf[Y/8 -1 -1][X+i]|=EPD_TestArr[i]>>(8-Y%8);
// 	}
// 	for(uint8_t i=0;i<Width;i++)
// 	{
// 		EPD_DisplayBuf[Y/8][X+i]|=EPD_TestArr[Width+i]<<(Y%8);
// 		EPD_DisplayBuf[Y/8 -1][X+i]|=EPD_TestArr[Width+i]>>(8-Y%8);
// 	}
// }
/*********************测试函数*/

