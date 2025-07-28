//
// Created by DELL on 6/8/2025.
//

#ifndef PWM_H
#define PWM_H

#endif //PWM_H
#include "Std_Types.h"
#include "Gpio.h"
#include "Gpio_Private.h"
#include "Rcc.h"

#define TIMER3_BASE_ADDRESS     0x40000400

typedef struct {
    volatile uint32 TIM_CR1;
    volatile uint32 TIM_CR2;
    volatile uint32 TIM_SMCR;
    volatile uint32 TIM_DIER;
    volatile uint32 TIM_SR;
    volatile uint32 TIM_EGR;
    volatile uint32 TIM_CCMR1;
    volatile uint32 TIM_CCMR2;
    volatile uint32 TIM_CCER;
    volatile uint32 TIM_CNT;
    volatile uint32 TIM_PSC;
    volatile uint32 TIM_ARR;
    volatile uint32 TIM_RESERVED_REG1;
    volatile uint32 TIM_CCR1;
    volatile uint32 TIM_CCR2;
    volatile uint32 TIM_CCR3;
    volatile uint32 TIM_CCR4;
    volatile uint32 TIM_RESERVED_REG2;
    volatile uint32 TIM_DCR;
    volatile uint32 TIM_DMAR;


}
TimerType;

#define timer3      ((TimerType *)TIMER3_BASE_ADDRESS)
void PWM_Init(void);
void Set_PWM_DutyCycle(uint16 duty);


