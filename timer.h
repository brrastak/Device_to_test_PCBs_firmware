// STM32F103
#include "stm32f10x.h"

#ifndef __TIMER_H__
#define __TIMER_H__

// CPU and SysTick timer frequency
#define F_CPU 		            72000000UL      // 72MHz
// every 0.5 ms
#define SYS_TIMER_TICK  	    (F_CPU/1000-1)  // 1kHz
// 8 bit per audio sample x 16kHz
#define BIT_DEPTH               127             // (2^N)/2 - 1
#define SAMPLING_RATE           32000L          // 32kHz

// Ticks
extern volatile uint32_t sys_tick;

// Init SysTick timer
void InitSysTick(void);
// Init TIM2
void InitTim2(void);
// TIM2 PWM duty time
void SetTim2DutyTime(int);
// Init TIM1
void InitTim1(void);
// Set TIM1 PWM (2 channels) duty time [-128; 127] -> [0%; 100%]
void SetTim1DutyTime(uint16_t duty);
// Delay TIM4
void delay_ms(int time);    // 32000ms max
void delay_us(int time);    // 32000us max
// TIM2 on/off
void EnableTim2(void);
void DisableTim2(void);

// SysTick timer interrupt
void SysTick_Handler(void);

#endif