// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>

//user
#include "hardware.h"
#include "main.h"
#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "dig_res.h"


// Buttons
btn_t Tamper    = {TAMPER_PIN,      0, 20, false, false, false};
btn_t ModeBtn   = {MODE_SWITCH_PIN, 0, 20, false, false, false};
btn_t StartBtn  = {START_BTN_PIN,   0, 20, false, false, false};
    

// Measure voltage num times and average
static const uint32_t average_num = 100;
// Delay before voltage measurement, ms
static const uint8_t delay_mes_ms = 20;


// Digital resistors
digres_t VoltageRes =   
    {CS_VOLTAGE_RES_PIN,    ADJUST_VOLTAGE_PIN, 0, 0x3F};
digres_t LoadRes =      
    {CS_LOAD_RES_PIN,       ADJUST_LOAD_PIN,    0, 0x3F};

// Power control
void EnablePower(void)
{
    SetPin(ENABLE_POWER_PIN);
}
void DisablePower(void)
{
    ResetPin(ENABLE_POWER_PIN);
    delay_ms(100);
}
// Measurement of voltage and current
uint32_t GetAdjVoltageMv(void)
{
    uint32_t res = 0;

    // Wait for voltage stabilization
    delay_ms(delay_mes_ms);
    
    // Get ADC value
    for (int i = 0; i < average_num; i++)
        res += ReadAdc(MEASURE_VOLTAGE_CHANNEL);
    res /= average_num;
    // Translate ADC value into mV
    res = res * V_REF_MV / ADC_MAX_VALUE;
    // Calculate adjustable voltage value, mV
    res = res * ADJ_VOLTAGE_RATIO / 1000;

    return res;
}
uint32_t GetExtVoltage1Mv(void)
{
    uint32_t res = 0;

    // Wait for voltage stabilization
    delay_ms(delay_mes_ms);
    
    // Get ADC value
    for (int i = 0; i < average_num; i++)
        res += ReadAdc(MEASURE_EXT_VOLTAGE1_CHANNEL);
    res /= average_num;
    // Translate ADC value into mV
    res = res * V_REF_MV / ADC_MAX_VALUE;
    // Calculate adjustable voltage value, mV
    res = res * EXT_VOLTAGE1_RATIO / 1000;

    return res;
}
// 0,47 Ohm current shunt
uint32_t GetHiCurrentMa(void)
{
    uint32_t res = 0;
    
    // Switch on current shunt for high currents (default state)
    SetPin(SWITCH_CURRENT_PROBE_PIN);
    
    // Get ADC value
    for (int i = 0; i < average_num; i++)
        res += ReadAdc(MEASURE_HIGH_CURRENT_CHANNEL);
    res /= average_num;
    // Translate ADC value into mV
    res = res * V_REF_MV / ADC_MAX_VALUE;
    // Calculate current value, mA
    res = res * 1000 / I_AMP_GAIN / HI_I_R;
    
    return res;
}
// 1k current shunt
uint32_t GetLoCurrentUa(void)
{
    uint32_t res = 0;
    
    // Switch off current shunt for high currents
    ResetPin(SWITCH_CURRENT_PROBE_PIN);
    
    // Get ADC value
    for (int i = 0; i < average_num; i++) {
        
        delay_ms(1);
        res += ReadAdc(MEASURE_LOW_CURRENT_CHANNEL);
    }
    res /= average_num;
    // Translate ADC value into mV
    res = res * V_REF_MV / ADC_MAX_VALUE;
    // Calculate current value, mA
    res = res * 1000 / I_AMP_GAIN / LO_I_R;
    
    // Switch on current shunt for high currents (default state)
    SetPin(SWITCH_CURRENT_PROBE_PIN);
    
    return res;
}
// Adjustable load current
uint32_t GetLoadCurrentMa(void)
{
    uint32_t res = 0;
    
    // Delay for load current stabilization
    delay_ms(2);
    
    // Get ADC value
    for (int i = 0; i < average_num; i++)
        res += ReadAdc(MEASURE_LOAD_CURRENT_CHANNEL);
    res /= average_num;
    // Translate ADC value into mV
    res = res * 1000 * V_REF_MV / ADC_MAX_VALUE;
    // Calculate current value, mA
    res = res / LOAD_I_R;
    
    return res;
}

// Set output adjustable voltage
void SetAdjVoltage(int16_t desired_voltage_mv)
{
    
    
    // Value of digital resistor which corresponds to the desired Vadj value
    int16_t new_res_value;
    // Current voltage value
    int32_t cur_voltage_mv = GetAdjVoltageMv();
    
    int cnt = 0;
    
    // Difference between current V and desired should be <= 100 mV
    while (((cur_voltage_mv - desired_voltage_mv) > 30) ||
           ((desired_voltage_mv - cur_voltage_mv) > 30)) {
           
        // The one unit of resistor value less then 100 mV
        // Depends on exact full resistance value wich is +-20%
        // The less resistor value the greater voltage
        new_res_value = GetResValue(&VoltageRes) + 
            (cur_voltage_mv - desired_voltage_mv) / 80;
        
        // Control limits
        if (new_res_value < 0)
            new_res_value = 0;
        if (new_res_value > GetMaxResValue(&VoltageRes))
            new_res_value = GetMaxResValue(&VoltageRes);
               
        // Set voltage
        SetResValue(&VoltageRes, new_res_value);
        
        // Can't reach desired voltage
        if ((new_res_value == 0) || 
            (new_res_value == GetMaxResValue(&VoltageRes)))
            break;
                
        cur_voltage_mv = GetAdjVoltageMv();
        
        // To avoid infinite loop
        cnt++;
        if (cnt > 5)
            break;
    }
    
}

// Increase/decrease adjustable voltage to one step
// returns false if limit is reached
bool IncAdjVoltage(void)
{ 
   // Check if limit reached
   if (GetResValue(&VoltageRes) == 0)
       return false;
   else
       ChangeResValue(&VoltageRes, -1);
       
   return true;
}
bool DecAdjVoltage(void)
{
    // Check if limit reached
   if (GetResValue(&VoltageRes) >= GetMaxResValue(&VoltageRes))
       return false;
   else
       ChangeResValue(&VoltageRes, 1);
       
   return true;
}

// Set load current
void IncLoadCurrent(void)
{
    ChangeResValue(&LoadRes, -1);
}

// Init voltage res (set min voltage)
void InitVoltageRes(void)
{
    SetResValue(&VoltageRes, 63);
}
// Init load resistor (set min load current)
void InitLoadRes(void)
{
    SetResValue(&LoadRes, 63);
}

// Reset to start state
void ResetHardwarePi(void)
{
    // Switch voltage off
    DisablePower();
    // Set minimal voltage
    InitVoltageRes();
    // Set minimum load current
    InitLoadRes();
    // Enable all thyristors
    EnableThyristor(3);
}

// Thyristors control
void EnableThyristor(int number)
{
    switch (number)
    {
    case 0:
        ResetPin(ENABLE_THYRISTOR1_PIN);
        SetPin(ENABLE_THYRISTOR2_PIN);
        SetPin(ENABLE_THYRISTOR3_PIN);
        break;
    case 1:
        SetPin(ENABLE_THYRISTOR1_PIN);
        ResetPin(ENABLE_THYRISTOR2_PIN);
        SetPin(ENABLE_THYRISTOR3_PIN);
        break;
    case 2:
        SetPin(ENABLE_THYRISTOR1_PIN);
        SetPin(ENABLE_THYRISTOR2_PIN);
        ResetPin(ENABLE_THYRISTOR3_PIN);
        break;
    default:    // enable all thyristors
        ResetPin(ENABLE_THYRISTOR1_PIN);
        ResetPin(ENABLE_THYRISTOR2_PIN);
        ResetPin(ENABLE_THYRISTOR3_PIN);
    }
}
void DisableThyristors(void)
{
    SetPin(ENABLE_THYRISTOR1_PIN);
    SetPin(ENABLE_THYRISTOR2_PIN);
    SetPin(ENABLE_THYRISTOR3_PIN);
}

