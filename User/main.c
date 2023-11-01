#include "main.h"



float32_t InputData[FFT_SIZE*2] = {0};
uint16_t InputCount = 0;
float32_t OutputData[FFT_SIZE] = {0};

int32_t main(void)
{
	uint16_t i;
	float32_t MaxValue = 0;
	uint32_t MaxIndex;
	
	OLED_Init();
	USARTx_Config();		
	Rheostat_Init();
	DAC_Mode_Init();

	OLED_CLS();	
  while (1)
  {
		
		if(InputCount == FFT_SIZE)
		{
			//采样完成之后进来这里
			
			//进行FFT处理
			arm_cfft_f32(&arm_cfft_sR_f32_len256, InputData, 0, 1);
			//求出幅度
			arm_cmplx_mag_f32(InputData, OutputData, FFT_SIZE);
			//丢弃第一个值
			OutputData[0] = 0.0f;
			//先求出输出数据的最大值
			arm_max_f32(OutputData, FFT_SIZE, &MaxValue, &MaxIndex);
			//对幅度进行归一化处理
			if(MaxValue >= 1)
				for(i = 0; i < FFT_SIZE/2; i ++)
				{
					OutputData[i] = OutputData[i]*63/MaxValue;
				}
			//处理完成之后，打开ADC，开始下一次采样
			InputCount = 0;
			ADC_Cmd(RHEOSTAT_ADC, ENABLE); 
			ADC_SoftwareStartConv(RHEOSTAT_ADC);
		}
		//对归一化后的数据进行显示
		//只需要取前128个数据就行，实数信号的FFT具有对称性
		//OLED_DrawFw(0,0);
		for(i = 1; i < FFT_SIZE/2; i ++)
		{
			OLED_DrawFw(i,(uint8_t)OutputData[i]);
		}
		//显示完后清楚，方便下一次显示
		newCLS();
		}
}





