//
// Created by DELL on 6/8/2025.
//
#include "PWM.h"
void PWM_Init(void) {


    Rcc_Init();
    Rcc_Enable(RCC_TIM3);
    Rcc_Enable(RCC_GPIOB);

    Gpio_Init(GPIO_B,5,GPIO_AF,GPIO_NO_PULL_DOWN);
    GpioType *gpioB = (GpioType *)(GPIOB_BASE_ADDR) ;
    gpioB->GPIO_AFRL &= ~(0xF << (5 * 4));
    gpioB->GPIO_AFRL |= (2 << (5 * 4));     // AF2 = TIM3_CH2

    timer3->TIM_PSC = 16 - 1;                         // Prescaler: 16MHz / 15+1 = 1MHz
    timer3->TIM_ARR = 1000 - 1;                        // Period
    timer3->TIM_CCR2 = 500;                           // duty cycle 50%

    timer3->TIM_CCMR1 &= ~(0x07<<12);           //clear
    timer3->TIM_CCMR1 |= (0x06 << 12);         // PWM Mode 1 high then low or toggle on match
    timer3->TIM_CCMR1 |= (0x01<<11);           // Enable the preload for capture/compare reg

    timer3->TIM_CCER |= (0x01 << 4);              // is enabling Channel 2 output  of Timer 3 (TIM3).


    timer3->TIM_CR1 |= (0x01 << 7);           // Enable auto reload preload
    timer3->TIM_EGR |= (0x01 << 0);             // force update to move content from preload to autoreload and capture/compare regs (Event
    timer3->TIM_CR1 |= (0x01 << 0);            // counter Enable
}

// --- Set PWM Duty Cycle (0-999) ---
void Set_PWM_DutyCycle(uint16 duty) {

    if (duty > 999) {
        duty = 999;
    }
    timer3->TIM_CCR2 = duty;
}
