// cstd
#include <stdbool.h>
#include <stdio.h>

//user
#include "verification_pi.h"
#include "hardware.h"
#include "test_pi.h"
#include "timer.h"
#include "display.h"


// Local buffer for sprintf function
char buf[16];


// Verification of device
void VerificationPi()
{
    // Pause
    Message("VERIFICATION...", "");
    delay_ms(1000);
    
    // Verificate V_adjustable = 12V
    EnablePower();   
    SetAdjVoltage(12000);
    delay_ms(1000);
    Message("VERIFICATION", "U = 12V");
    WaitForStartBtn();
    
    // Verificate V_adjustable = 16V
    Message("VERIFICATION...", "");
    SetAdjVoltage(16000);
    delay_ms(1000);
    Message("VERIFICATION", "U = 16V");
    WaitForStartBtn();
    
    // Verificate I_load = 100mA
    Message("VERIFICATION...", "");
    SetAdjVoltage(12000);
    delay_ms(1000);
    // Increase load current (max 64 steps)
    for (int i = 0; i < 64; i++) {
            
        IncLoadCurrent();
        
        // Desired current, mA
        if (GetLoadCurrentMa() > 100)
            break;
    }
    Message("VERIFICATION", "I = 100mA");
    WaitForStartBtn();
    
    // Verificate I ~ mA (depending on used external resistor)
    Message("VERIFICATION...", "");
    ResetHardwarePi();
    EnablePower();
    SetAdjVoltage(12000);
    delay_ms(1000);
    Message("INSERT RESISTOR", "PRESS BUTTON");
    WaitForStartBtn();
    // Measure current
    Message("VERIFICATION...", "");
    delay_ms(1000);
    sprintf(buf, "I = %imA", GetHiCurrentMa());
    Message("VERIFICATION", buf);
    WaitForStartBtn();
    
    // Verificate I ~ uA (depending on used external resistor)
    Message("VERIFICATION...", "");
    SetAdjVoltage(12000);
    delay_ms(1000);
    Message("INSERT RESISTOR", "PRESS BUTTON");
    WaitForStartBtn();
    // Measure current
    Message("VERIFICATION...", "");
    delay_ms(2000);
    for (int i = 0; i < 20; i++)
        GetLoCurrentUa();
    sprintf(buf, "I = %iuA", GetLoCurrentUa());
    Message("VERIFICATION", buf);
    WaitForStartBtn();
    
    // Go to start state
    ResetHardwarePi();
    Message("TEST", "");
    delay_ms(500);
}


