// cstd
#include <stdbool.h>

//user
#include "dig_res.h"
#include "gpio.h"
#include "timer.h"


// Increase/decrease value
void ChangeResValue(digres_t* res, int16_t num)
{
    int i;
    bool inc;   // true - increase; false - decrease
    
    if (num == 0)       // no change
        return;
    
    if (num > 0)
        inc = true;     // increase
    else
        inc = false;    // decrease
    
    // Preparing
    if (inc == false) {
        
        num = -num;
        ResetPin(res->ud_port, res->ud_pin);
    }
    else
        SetPin(res->ud_port, res->ud_pin);
    
    // Start transmission
    delay_us(2);
    ResetPin(res->cs_port, res->cs_pin);
    delay_us(2);
        
    for (i = 0; i < num; i++) {
        
        // One step
        TogglePin(res->ud_port, res->ud_pin);
        delay_us(2);
        TogglePin(res->ud_port, res->ud_pin);
        delay_us(2);
        
        if (inc == true)
            res->value ++;
        else
            res->value --;
        if ((res->value == res->max_value) || (res->value == 0))    // limit values
            break;
    }
        
    // End transmission
    SetPin(res->cs_port, res->cs_pin);
    delay_us(2);
}
// Set value
void SetResValue(digres_t* res, uint16_t num)
{
    int16_t dif;
    
    if (num > res->max_value)
        num = res->max_value;
    
    // Number of steps to increase/decrease
    dif = num - res->value;
    
    ChangeResValue(res, dif);
}
// Get current value
uint16_t GetResValue(digres_t* res)
{
    return res->value;
}
// Get max value
uint16_t GetMaxResValue(digres_t* res)
{
    return res->max_value;
}

