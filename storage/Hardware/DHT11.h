#ifndef __DHT11_H__
#define __DHT11_H__
void DHT_Init_InPut(void);
void DHT_Init_OutPut(void);
void DHT_Start(void);
uint16_t DHT_Scan(void);
uint16_t DHT_ReadBit(void);
uint16_t DHT_ReadByte(void);
uint16_t DHT_ReadData(uint8_t buffer[5]);

#endif