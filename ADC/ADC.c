
#include "ADC.h"

#include <delay.h>

#include "Rcc.h"
#include "Gpio.h"
#include "Std_Types.h"

void ADC_Channel_Init(uint32 channel) {
    switch(channel) {
        case 0:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 0, GPIO_ANALOG, 0); break;
        case 1:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 1, GPIO_ANALOG, 0); break;
        case 2:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 2, GPIO_ANALOG, 0); break;
        case 3:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 3, GPIO_ANALOG, 0); break;
        case 4:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 4, GPIO_ANALOG, 0); break;
        case 5:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 5, GPIO_ANALOG, 0); break;
        case 6:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 6, GPIO_ANALOG, 0); break;
        case 7:  Rcc_Enable(RCC_GPIOA); Gpio_Init(GPIO_A, 7, GPIO_ANALOG, 0); break;
        case 8:  Rcc_Enable(RCC_GPIOB); Gpio_Init(GPIO_B, 0, GPIO_ANALOG, 0); break;
        case 9:  Rcc_Enable(RCC_GPIOB); Gpio_Init(GPIO_B, 1, GPIO_ANALOG, 0); break;
        case 10: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 0, GPIO_ANALOG, 0); break;
        case 11: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 1, GPIO_ANALOG, 0); break;
        case 12: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 2, GPIO_ANALOG, 0); break;
        case 13: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 3, GPIO_ANALOG, 0); break;
        case 14: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 4, GPIO_ANALOG, 0); break;
        case 15: Rcc_Enable(RCC_GPIOC); Gpio_Init(GPIO_C, 5, GPIO_ANALOG, 0); break;
        default: break;
    }
}

void ADC_Init(void)
{
    // 1. Enable ADC1 Clock
    Rcc_Init();
    Rcc_Enable(RCC_ADC1);

    // 2. Reset ADC settings
    MY_ADC->CR1 = 0;
    MY_ADC->CR2 = 0;

    // 3. Set prescaler (optional)
    ADC_COMMON->CCR &= ~(3 << 16);  // Clear ADCPRE bits
    ADC_COMMON->CCR |=  (2 << 16);  // ADCPRE = PCLK2 / 6

    // 4. Set resolution = 12-bit default 2^12= 4096 we will use it in the conversion to volts
    MY_ADC->CR1 &= ~(3 << 24);

    // 5. Right alignment default
    MY_ADC->CR2 &= ~(1 << 11);

    // 6. Disable scan and continuous mode
    MY_ADC->CR1 &= ~(1 << 8);   // SCAN = 0
    MY_ADC->CR2 &= ~(1 << 1);   // CONT = 0

    // 7. Set conversion sequence length to 1
    MY_ADC->SQR[0] &= ~(0xF << 20);

    // 8. Enable ADC **at the end**
    MY_ADC->CR2 |= (1 << 0);  // ADON

    delay_ms(10);  // stabilization
}


uint32 ADC_Read_Channel(uint32 channel)
{
    // Configure sampling time
    if (channel <= 9) {
        MY_ADC->SMPR[1] &= ~(7 << (channel * 3));
        MY_ADC->SMPR[1] |=  (4 << (channel * 3)); // 84 cycles
    } else {
        uint8 ch = channel - 10;
        MY_ADC->SMPR[0] &= ~(7 << (ch * 3));
        MY_ADC->SMPR[0] |=  (4 << (ch * 3)); // 84 cycles
    }

    // Set channel in sequence (SQ1)
    MY_ADC->SQR[2] &= ~(0x1F << 0);
    MY_ADC->SQR[2] |= (channel << 0);

    // Start conversion
    MY_ADC->CR2 |= (1 << 30); // SWSTART

    // Wait for EOC
    while (!(MY_ADC->SR & (1 << 1)));

    // Read result
    return MY_ADC->DR;
}


// Optional: Get voltage value (3.3V reference)
float ADC_Get_Voltage(uint32 channel)
{
    uint32 adc_value = ADC_Read_Channel(channel);
    return (float)((adc_value * 3.3f) / 4095.0f);
}

//conversion to string
void float_to_str(float num, char *str, int precision) {
    if (!str) return;

    // 1. Handle negative numbers
    int i = 0;
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }

    // 2. Extract integer part
    uint32 int_part = (uint32)num;
    float frac_part = num - int_part;

    // 3. Convert integer part to string
    char int_str[15];
    int j = 0;
    if (int_part == 0) {
        int_str[j++] = '0';
    } else {
        while (int_part > 0) {
            int_str[j++] = (int_part % 10) + '0';
            int_part /= 10;
        }
    }

    // Reverse integer part
    for (int k = j - 1; k >= 0; k--) {
        str[i++] = int_str[k];
    }

    // 4. Add decimal point
    str[i++] = '.';

    // 5. Convert fractional part
    for (int p = 0; p < precision; p++) {
        frac_part *= 10;
        int digit = (int)frac_part;
        str[i++] = digit + '0';
        frac_part -= digit;
    }

    // 6. Null-terminate
    str[i] = '\0';
}
