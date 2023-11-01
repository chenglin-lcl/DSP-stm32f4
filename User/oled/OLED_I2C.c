#include "OLED_I2C.h"
#include "bsp_i2c_ee.h"
#include "bsp_sysTick.h"
#include "codetab.h"

uint8_t DataBuffer[8][128];

 /**
  * @brief  WriteCmd，向OLED写入命令
  * @param  I2C_Command：命令代码
  * @retval 无
  */
void WriteCmd(unsigned char I2C_Command)//写命令
{
	I2C_EE_ByteWrite(&I2C_Command, 0x00);
}


 /**
  * @brief  WriteDat，向OLED写入数据
  * @param  I2C_Data：数据
  * @retval 无
  */
void WriteDat(unsigned char I2C_Data)//写数据
{
	I2C_EE_ByteWrite(&I2C_Data, 0x40);
}


 /**
  * @brief  OLED_Init，初始化OLED
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	I2C_EE_Init();				 /*I2C总线的GPIO初始化*/
	Delay_ms(1000);		// 1s,这里的延时很重要,上电后延时，没有错误的冗余设计
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}


 /**
  * @brief  OLED_SetPos，设置光标
  * @param  x,第几页
	*					y,第几列
  * @retval 无
  */
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+x);
	WriteCmd((y&0x0f)|0x00);//LOW
	WriteCmd(((y&0xf0)>>4)|0x10);//HIGHT
}

 /**
  * @brief  OLED_Fill，填充整个屏幕
  * @param  fill_Data:要填充的数据
	* @retval 无
  */
void OLED_Fill(void)//全屏填充
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			DataBuffer[i][j]=0xff;
		}
	}
	Write_DataBuffer();
}

 /**
  * @brief  OLED_CLS，清屏
  * @param  无
	* @retval 无
  */
void OLED_CLS(void)//清屏
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			DataBuffer[i][j]=0x00;
		}
	}
	Write_DataBuffer();
}


 /**
  * @brief  OLED_ON，将OLED从休眠中唤醒
  * @param  无
	* @retval 无
  */
void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}


 /**
  * @brief  OLED_OFF，让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
  * @param  无
	* @retval 无
  */
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}


void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
	// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(y,x);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(y,x);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(y+1,x);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
	// Description    : 显示codetab.h中的汉字,16*16点阵
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(y,x);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(y+1,x);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
	// Description    : 显示BMP位图
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(y,x0);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}


/*0<=x<=127,0<=y<=63屏幕看作一个坐标轴，左下角是原点*/
void OLED_DrawPoint(uint8_t x,uint8_t y)/*这里x是横坐标，y是纵坐标，在（x,y）处画一个点*/
{
	if(x>127||y>63)
		return;
	uint8_t page,move,data;
	
	page=7-y/8;//数据写在第几页
	move=7-y%8;//0x01要移动的位数
	data=0x01<<move;//要写什么数据
	
	OLED_SetPos(page,x);
	WriteDat(data);
}

/*0<=x<=127,0<=y<=63屏幕看作一个坐标轴，左下角是原点*/
void OLED_DrawFw(uint8_t x,uint8_t y)/*这里x是横坐标，y是纵坐标，在（x,y）处画一个点*/
{
	uint16_t i;
	if(x>127||y>63)
		return;
	uint8_t page,move,data, temp;
	
	temp = y/8;
	page=7-temp;//数据写在第几页
	move=7-y%8;//0xff要移动的位数
	data=0xff<<move;//要写什么数据
	
	OLED_SetPos(page,x);
	WriteDat(data);
	for(i = 0; i < temp; i ++)
	{
		OLED_SetPos(7-i,x);
		WriteDat(0xff);
	}
}

void newCLS(void)
{
	uint16_t i, j;
	for(i = 0; i <= 7; i++)
	{
		OLED_SetPos(i,0);
		for(j = 0; j <= 127; j++)
		{
			WriteDat(0x00);
		}
	}
}


/*写 缓存数据*/
void Write_DataBuffer(void)//这个是将DataBuffer数组里面的值，全部写进屏里去
{	
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		OLED_SetPos(i,0); //设置起始点坐标
		for(j=0;j<128;j++)
		{
			WriteDat(DataBuffer[i][j]);//写数据
		}
	}
}

