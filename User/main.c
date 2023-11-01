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
			//�������֮���������
			
			//����FFT����
			arm_cfft_f32(&arm_cfft_sR_f32_len256, InputData, 0, 1);
			//�������
			arm_cmplx_mag_f32(InputData, OutputData, FFT_SIZE);
			//������һ��ֵ
			OutputData[0] = 0.0f;
			//�����������ݵ����ֵ
			arm_max_f32(OutputData, FFT_SIZE, &MaxValue, &MaxIndex);
			//�Է��Ƚ��й�һ������
			if(MaxValue >= 1)
				for(i = 0; i < FFT_SIZE/2; i ++)
				{
					OutputData[i] = OutputData[i]*63/MaxValue;
				}
			//�������֮�󣬴�ADC����ʼ��һ�β���
			InputCount = 0;
			ADC_Cmd(RHEOSTAT_ADC, ENABLE); 
			ADC_SoftwareStartConv(RHEOSTAT_ADC);
		}
		//�Թ�һ��������ݽ�����ʾ
		//ֻ��Ҫȡǰ128�����ݾ��У�ʵ���źŵ�FFT���жԳ���
		//OLED_DrawFw(0,0);
		for(i = 1; i < FFT_SIZE/2; i ++)
		{
			OLED_DrawFw(i,(uint8_t)OutputData[i]);
		}
		//��ʾ��������������һ����ʾ
		newCLS();
		}
}





