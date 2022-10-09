// STM32F103
#include "stm32f10x.h"

// user
#include "init.h"
#include "main.h"
#include "gpio.h"

void InitRcc(void)
{
    // port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // port B
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // port C
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    // ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // TIM2
    //RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // TIM1
    //RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    // USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // AFIO
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // I2C1
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
}
void InitInterrupt(void)
{
    // USART1
    NVIC_EnableIRQ(USART1_IRQn);
    // I2C1
    NVIC_EnableIRQ(I2C1_EV_IRQn);
}
void InitGpio(void)
{   
    // Remap I2C1
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    
    // Remap A15, B3, B4
    AFIO->MAPR |=   AFIO_MAPR_SWJ_CFG_2 * 0 |
                    AFIO_MAPR_SWJ_CFG_1 * 1 |
                    AFIO_MAPR_SWJ_CFG_0 * 0;
    
    
    // I2C1 for display
    SetGpioMode(I2C_SCL_PIN, PIN_MODE_2MHZ_AFOD);
    SetGpioMode(I2C_SDA_PIN, PIN_MODE_2MHZ_AFOD);

    // Buttons
    SetGpioMode(TAMPER_PIN, PIN_MODE_IFLOAT);
    SetGpioMode(MODE_SWITCH_PIN, PIN_MODE_IFLOAT);
    SetGpioMode(START_BTN_PIN, PIN_MODE_IFLOAT);

    // Measurement (for ADC)
    SetGpioMode(MEASURE_VOLTAGE_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_LOW_CURRENT_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_HIGH_CURRENT_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_LOAD_CURRENT_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_EXT_VOLTAGE1_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_EXT_VOLTAGE2_PIN, PIN_MODE_IANALOG);
    SetGpioMode(MEASURE_EXT_VOLTAGE3_PIN, PIN_MODE_IANALOG);

    // Controls
    SetGpioMode(ENABLE_POWER_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(SWITCH_CURRENT_PROBE_PIN, PIN_MODE_2MHZ_OPP);
    ResetPin(ENABLE_POWER_PIN);         // power disabled
    // Connected current shunt for high currents
    SetPin(SWITCH_CURRENT_PROBE_PIN);

    // Digital resistors control pins
    SetGpioMode(CS_LOAD_RES_PIN, PIN_MODE_10MHZ_OOD);
    SetGpioMode(ADJUST_LOAD_PIN, PIN_MODE_10MHZ_OOD);
    SetGpioMode(CS_VOLTAGE_RES_PIN, PIN_MODE_10MHZ_OOD);
    SetGpioMode(ADJUST_VOLTAGE_PIN, PIN_MODE_10MHZ_OOD);
    SetPin(ADJUST_LOAD_PIN);
    SetPin(ADJUST_VOLTAGE_PIN);

    // PI board controls
    SetGpioMode(ENABLE_THYRISTOR1_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(ENABLE_THYRISTOR2_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(ENABLE_THYRISTOR3_PIN, PIN_MODE_2MHZ_OPP);
    ResetPin(ENABLE_THYRISTOR1_PIN);
    ResetPin(ENABLE_THYRISTOR2_PIN);
    ResetPin(ENABLE_THYRISTOR3_PIN);

    // SHS board controls
    SetGpioMode(SWITCH_IN_POWER_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(CURRENT_KEY_IN_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(CURRENT_KEY_GND_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(CURRENT_KEY_OUT_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(SHORT_OUT_PIN, PIN_MODE_2MHZ_OPP);

    // Onboard LED
    SetGpioMode(LED_PIN, PIN_MODE_2MHZ_OPP);
    SetPin(LED_PIN);

    // USART
    SetGpioMode(USART_TX_PIN, PIN_MODE_2MHZ_OPP);
    SetGpioMode(USART_RX_PIN, PIN_MODE_IFLOAT);
    
}
