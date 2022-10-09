/*
    
*/

// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>

// usr
#include "main.h"
#include "button.h"

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// Buttons
extern btn_t Tamper;
extern btn_t ModeBtn;
extern btn_t StartBtn;


// Power control
void EnablePower(void);
void DisablePower(void);

// Measurement of voltage and current
uint32_t GetAdjVoltageMv(void);
uint32_t GetExtVoltage1Mv(void);
// 0,47 Ohm current shunt
uint32_t GetHiCurrentMa(void);
// 1k current shunt
uint32_t GetLoCurrentUa(void);
// Adjustable load current
uint32_t GetLoadCurrentMa(void);

// Set output adjustable voltage
void SetAdjVoltage(int16_t voltage_mv);
// Increase/decrease adjustable voltage to one step
// returns false if limit is reached
bool IncAdjVoltage(void);
bool DecAdjVoltage(void);

// Set load current
void IncLoadCurrent(void);

// Thyristors control
void EnableThyristor(int number);
void DisableThyristors(void);

// Reset to start state
void ResetHardwarePi(void);
// Init voltage resistor (set min voltage)
void InitVoltageRes(void);
// Init load resistor (set min load current)
void InitLoadRes(void);



#endif
