#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H
#include "Std_Types.h"

typedef struct {
    volatile uint32 SR;
    volatile uint32 CR1;
    volatile uint32 CR2;
    volatile uint32 SMPR[2];
    volatile uint32 JOFR[4];
    volatile uint32 HTR;
    volatile uint32 LTR;
    volatile uint32 SQR[3];
    volatile uint32 JSQR;
    volatile uint32 JDR[4];
    volatile uint32 DR;

}ADC_REGISTERS;

typedef struct {
    volatile uint32 CSR;   // Offset 0x00, ADC Common Status Register
    volatile uint32 CCR;   // Offset 0x04, ADC Common Control Register
} ADC_COMMON_REGISTERS;

#define MY_ADC          ((ADC_REGISTERS*)0x40012000)
#define ADC_COMMON    ((ADC_COMMON_REGISTERS*)0x40012300) //this address is from common ADC common block


void ADC_Init(void);
void ADC_Channel_Init(uint32 channel);
uint32 ADC_Read_Channel(uint32 channel);
float ADC_Get_Voltage(uint32 channel);
void float_to_str(float num, char *str, int precision);

#endif
