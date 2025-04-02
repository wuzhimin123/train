#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MPU6050.h"
#include "DHT11.h"
#include "Serial.h"
#include "Buzzer.h"
#include <stdio.h>
#include "LED.h"
#include "adc.h"


int16_t AX, AY, AZ, GX, GY, GZ, ADC_Value;			//定义用于存放各个数据的变量
uint8_t buffer[5];
float t,current;

#define RX_BUFFER_SIZE 10
#define VREF 3.3f
#define R  0.05f

int main(void)
{
	/*模块初始化*/
	
	MPU6050_Init();		//MPU6050初始化
	
	/*显示ID号*/
	
	Serial_Init();
	LED_Init();
	Buzzer_Init();
	AD_Init();
	AD_Start();
	while (1)
	{
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//获取MPU6050的数据
		if(DHT_ReadData(buffer) == 0)
		{
			t = buffer[2] + buffer[3]/10.0;
		}
		if(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == SET)
		{
			ADC_Value = ADC_GetConversionValue(ADC1);
			current = (float)ADC_Value * VREF / 4096 /R;
		}
		char data_str[100];
		sprintf(data_str,"AX: %d, AY: %d, AZ: %d, GX: %d, GY: %d, GZ: %d, Temp: %.1f,Current:%.3f\r\n", AX, AY, AZ, GX, GY, GZ, t,current);
		Serial_SendByte(0x55);//发送起始帧
		Serial_SendString(data_str);
		
		
		for (volatile uint32_t i = 0; i < 1000000; i++);
	}
}
/*串口中断服务函数*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		uint8_t command = USART_ReceiveData(USART1);
		
		switch(command)
		{
			case 0x51:
				Buzzer_On(); 
				break;
			case 0x50:
				Buzzer_Off();
				break;
			case 0x61:
				LED2_ON();
				break;
			case 0x60:
				LED2_OFF();
				break;
			default:
				break;
		}
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		
	}
}
