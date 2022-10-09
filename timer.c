// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>
#include <stdint.h>

//user
#include "main.h"
#include "timer.h"
#include "hardware.h"

volatile uint32_t sys_tick = 0;

// SysTick timer interrupt
void SysTick_Handler(void)
{  
    sys_tick++;             // every 1ms
    
    CheckBtn(&Tamper);
    CheckBtn(&ModeBtn);
    CheckBtn(&StartBtn);
    
}
void InitSysTick(void)
{
    SysTick->LOAD = SYS_TIMER_TICK;
    SysTick->VAL = 0;
    SysTick->CTRL =	SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |    // interrupt enable
                    SysTick_CTRL_ENABLE_Msk;
}


// Delay TIM4
void delay_ms(int time)
{
    TIM4->CR1 = 0;
    TIM4->SR = 0;
    TIM4->PSC = 35999;                  // 2kHz
    TIM4->ARR = time * 2 - 1;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 |= TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF))
        ;
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
void delay_us(int time)
{
    TIM4->CR1 = 0;
    TIM4->SR = 0;
    TIM4->PSC = 35;                     // 2MHz
    TIM4->ARR = time * 2 - 1;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 |= TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF))
        ;
    TIM4->CR1 &= ~TIM_CR1_CEN;
}
