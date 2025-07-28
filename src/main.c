#include <ADC.h>
#include "LCD.h"
#include <string.h>
#include "PWM.h"
#include "EXTI.h"
#include "delay.h"
#include "Timer.h"
#define IR_SENSOR_PIN GPIO_A,9
static const GpioPin Sensorpin={IR_SENSOR_PIN};


void reverse(char* str, int len);
int itoa_u16(uint16 n, char* str, int min_digits);
void IR_Sensor_Init(void);
void detect_objects(void);
void floatToStr(float val, char *buf, int precision);
void uintToStr(uint32 val, char *buf);
void Enable_Interrupts(void);
void Disable_Interrupts(void);

static uint32 object_count=0;
volatile uint8 emergency_stop_active = 0;
volatile uint32 T1 = 0, T2 = 0;
volatile uint64 delta = 0;
float frequency = 0.0f;


int main(void) {
    Rcc_Init();
    Rcc_Enable(RCC_SYSCFG);
    Rcc_Enable(RCC_GPIOA);
    Rcc_Enable(RCC_GPIOB);
    Rcc_Enable(RCC_GPIOC);
    Rcc_Enable(RCC_TIM2);







    Gpio_Init(GPIO_A,12,GPIO_INPUT,GPIO_PULL_UP);
    Exti_Init(GPIO_A,12,FALLING_EDGE);
    Exti_Enable(12);

    // Second button on PA11 for reset
    Gpio_Init(GPIO_A, 11, GPIO_INPUT, GPIO_PULL_UP);
    Exti_Init(GPIO_A, 11, FALLING_EDGE);
    Exti_Enable(11);

    uint16 adc_value;
    uint16 pwm_duty;
    uint8 speed_percent;


    /*
     * Initialization of ADC,PWM,Lcd,IR
     */
    Timer_Init();
    Timer_Reset();
    Timer_As_Capture_Mode();
    ADC_Init();
    ADC_Channel_Init(0);
    PWM_Init();
    Lcd_Init();
    IR_Sensor_Init();


    /*
     * Initialization of ADC,PWM,Lcd,IR
     */

    Lcd_Clear();
    Lcd_SetCursor(0, 0);
    Lcd_SendString("  Motor Control");
    delay_ms(100);
    Lcd_Clear();

    while (1) {
        if (emergency_stop_active) {
            delay_ms(100);
            continue; // Skip  while emergency active
        }
        else
        {
            __disable_irq();
            // Frequency and RPM calculations

            // uint64 T1, T2;
            volatile uint32 overflow_count = 0;

            Timer2_N->CNT = 0;
            Timer2_N->SR &= ~(1 << 0); // clear overflow flag
            Timer2_N->SR &= ~(1 << 1); // clear capture flag

            T1 = Timer_Capture();
            while (!(Timer2_N->SR & (1 << 1))) {
                if (Timer2_N->SR & (1 << 0)) {
                    overflow_count++;
                    Timer2_N->SR &= ~(1 << 0);  // clear UIF
                }
            }

            T2 = Timer_Capture();
            uint64 delta_ticks;

            if (T2 >= T1) {
                delta_ticks = (uint64_t)overflow_count * ((uint64_t)0xFFFFFFFF + 1) + (T2 - T1);
            } else {
                delta_ticks = (uint64_t)(overflow_count - 1) * ((uint64_t)0xFFFFFFFF + 1) + ((0xFFFFFFFF - T1 + 1) + T2);
            }


            if (T2 >= T1) {
                delta_ticks = (uint64_t)overflow_count * ((uint64_t)0xFFFFFFFF + 1) + (T2 - T1);
            } else {
                delta_ticks = (uint64_t)(overflow_count - 1) * ((uint64_t)0xFFFFFFFF + 1) + ((0xFFFFFFFF - T1 + 1) + T2);
            }


            float time_sec = delta_ticks / 16e6f;
            float frequency = 1.0f / time_sec;
            float rpm = frequency * 60.0f;

            __enable_irq();

            // Object detection
            detect_objects();
            __disable_irq();
            // Read ADC and calculate PWM duty cycle
            adc_value = ADC_Read_Channel(0);
            pwm_duty = (uint16)(((uint32)adc_value * 999) / 4095);
            speed_percent = (uint8)(((uint32)adc_value * 100) / 4095);
            __enable_irq();
            Set_PWM_DutyCycle(pwm_duty);
            // Convert values to strings
            char speed_str[4];
            char rpm_str[17];
            char count_str[4];
            __disable_irq();
            itoa_u16(speed_percent, speed_str, 3);
            floatToStr(rpm, rpm_str, 1);      // 1 decimal place for RPM
            itoa_u16(object_count, count_str, 3);
            __enable_irq();
            // Display on LCD (optimized for 16x2)
            // Line 1: Motor Speed (PWM %) | Object Count
            __disable_irq();
            Lcd_SetCursor(0, 0);
            Lcd_SendString("PWM:");
            Lcd_SendString(speed_str);
            Lcd_SendString("%");

            // Right-align object count
            Lcd_SetCursor(10, 0); // Column 10 on line 1
            Lcd_SendString("CO:");
            Lcd_SendString(count_str);

            // Line 2: Conveyor Speed (RPM)
            Lcd_SetCursor(0, 1);
            Lcd_SendString("S:");
            Lcd_SendString(rpm_str);
            Lcd_SendString(" RPM");
            __enable_irq();

            delay_ms(100); // Refresh rate (adjust as needed)
        }
    }
}


void IR_Sensor_Init(void) {
    // Configure PA1 as input with pull-up (IR sensor normally HIGH when no object)
    Gpio_Init(Sensorpin.port, Sensorpin.pinNumber, GPIO_INPUT, GPIO_PULL_UP);
}
void detect_objects(void) {
    static uint8 previous_state = 1;  // Initialize to HIGH (assuming no object present)
    __disable_irq();
    GpioType * GpioA=Gpio_GetPort(GPIO_A);
    uint8 current_state = (GpioA->GPIO_IDR & (1 << Sensorpin.pinNumber)) ? 1 :0;

    // Detect falling edge (1→0 transition)
    if (previous_state && !current_state) {
        object_count++;
    }
    previous_state = current_state;  // Always update the previous state
    __enable_irq();
}


// --- Helper Function: Reverse String ---
void reverse(char* str, int len) {
    int i = 0, j = len - 1;
    char temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


int itoa_u16(uint16 n, char* str, int min_digits) {
    int i = 0;
    if (n == 0) {
        str[i++] = '0';
    }
    while (n > 0) {
        str[i++] = (n % 10) + '0';
        n = n / 10;
    }
    // Add leading spaces for padding if needed
    while (i < min_digits) {
        str[i++] = ' '; // Use space for padding
    }
    str[i] = '\0'; // Null terminate
    reverse(str, i);
    return i;
}

// Efficient conversion functions
void uintToStr(uint32_t val, char *buf) {
    char temp[11];
    int i = 0;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (val > 0) {
        temp[i++] = (val % 10) + '0';
        val /= 10;
    }

    int j = 0;
    while (i > 0) {
        buf[j++] = temp[--i];
    }
    buf[j] = '\0';
}

void floatToStr(float val, char *buf, int precision) {
    char temp[20];
    int int_part = (int)val;
    float frac_part = val - int_part;
    int i = 0;

    if (val < 0) {
        buf[i++] = '-';
        int_part = -int_part;
        frac_part = -frac_part;
    }

    uintToStr(int_part, temp);
    strcpy(buf + i, temp);
    i += strlen(temp);

    if (precision > 0) {
        buf[i++] = '.';
        for (int p = 0; p < precision; p++) {
            frac_part *= 10;
            int digit = (int)frac_part;
            buf[i++] = digit + '0';
            frac_part -= digit;
        }
    }
    buf[i] = '\0';
}
void Enable_Interrupts(void)
{
    Exti_Enable(12);
    Exti_Enable(11);
}
void Disable_Interrupts(void)
{
    Exti_Disable(12);
    Exti_Disable(11);


}