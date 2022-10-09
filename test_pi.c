// cstd
#include <stdbool.h>

//user
#include "test_pi.h"
#include "hardware.h"
#include "button.h"
#include "timer.h"
#include "display.h"


// Test limits
const uint8_t max_zener_leakage_ua = 100;       // Vin = 12V
const uint8_t min_zener_protection_ma = 130;    // Vin = 16V
// If max load current > threshold value then thyristor disabled
// else thyristor enabled
const uint8_t thyristor_treshold_ma = 132;
// Board should be able to provide that current so thyristors must not open
const uint8_t max_desired_current_ma = 100;
// Min Vout, mV (Vin = 12V; Load current = 100mA)
const uint16_t min_out_voltage_mv = 10600;  // fuse resistance <= 3.0 Ohms


// Test one of three thyristors (num=3: all three thyristors)
bool TestThyristor(uint8_t num);



test_mode_t ChooseMode(test_mode_t current_mode)
{

    test_mode_t test_mode;
    
    test_mode = current_mode;
        
    // Choose test mode
    while (true) {
            
        /*if (disp_update == true) {
                
            Message("TEST", "");
            if (test_mode == test_t)       
                Message("TEST 1", "CLOSE TO START");
            else
                Message("TEST 2", "PRESS START");     
                
            disp_update = false;
        }*/
            
        // Toggle mode
        /*if (WasPressed(&ModeBtn)) {
                
            if (test_mode == test_t)
                test_mode = test_no_t;
            else
                test_mode = test_t;
                
            disp_update = true;
        }*/
            
        // Choise made
        if (IsPressed(&Tamper)) {
            
            test_mode = test_t;
            break;
        }
        if (IsPressed(&StartBtn)) {
            
            test_mode = test_no_t;
            break;
        }
        if (IsPressed(&ModeBtn)) {
            
            test_mode = ver_t;
            break;
        }
    }
    
    return test_mode;
}
// Two test modes
void TestWithThyristorsApart(void)
{
    // Test was passed succesfully
    bool test_ok;
            
    Message("TESTING...", "");
    delay_ms(1000);
     
    // Test zener diodes leakage at 12V
    test_ok = TestZenerLeakage();
      
    // Test zener diodes switching on at 16V
    if (test_ok)
        test_ok = TestZenerProtection();     
        
    // Test Vout
    if (test_ok)
        test_ok = TestVout();
        
    // Test connection between thyristor gates and test needles
    if (test_ok)
        test_ok = TestConnection();     
    
    // Test thyristors switching on
    if (test_ok)
        test_ok = TestThyristorsOneByOne();
        
    // Go to start state
    ResetHardwarePi();
    delay_ms(500);
        
    if (test_ok == false)
        BlinkUntilTamper();
    else {
        Message("TEST", "OK");
        WaitForTamperUnpressed();
    }
}
void TestWithThyristorsTogether(void)
{
    // Test was passed succesfully
    bool test_ok;
    
    Message("TESTING...", "");
    delay_ms(500);
     
    // Test zener diodes leakage at 12V
    test_ok = TestZenerLeakage();
      
    // Test zener diodes switching on at 16V
    if (test_ok)
        test_ok = TestZenerProtection();  
       
    // Test Vout
    if (test_ok)
        test_ok = TestVout();
        
    // Test thyristors switching on
    if (test_ok)
        test_ok = TestThyristorsTogether();
        
    // Go to start state
    ResetHardwarePi();
        
    if (test_ok == false)
        BlinkUntilStartBtn();
    else {
        delay_ms(500);
        Message("TEST", "OK");
    }
}

// Wait for Start button is pressed
void WaitForStartBtn(void)
{
    while (true) {
        
        if (IsPressed(&StartBtn) == true)
            break;
    }
}
// Blinking display backlight until Start button pressed
void BlinkUntilStartBtn(void)
{
    uint16_t cnt;
    cnt = 0;
    
    while (true) {
        
        if (IsPressed(&StartBtn) == true)
            break;
        
        delay_ms(1);
        // Blinking
        cnt++;
        if (cnt == 600) {
            
            BacklightOff();
        }
        if (cnt == 1200) {
            
            BacklightOn();
            cnt = 0;
        }
    }
    
    BacklightOn();
    delay_ms(200);
}
// Blinking display backlight until Tamper unpressed
void BlinkUntilTamper(void)
{
    uint16_t cnt;
    cnt = 0;
    
    while (true) {
        
        if (IsPressed(&Tamper) == false)
            break;
        
        delay_ms(1);
        // Blinking
        cnt++;
        if (cnt == 600) {
            
            BacklightOff();
        }
        if (cnt == 1200) {
            
            BacklightOn();
            cnt = 0;
        }
    }
    
    BacklightOn();
}
// Test zener diodes current at 12V
bool TestZenerLeakage(void)
{
    // Test was passed succesfully
    bool test_ok;
    
    // Switch on power
    EnablePower();   
    SetAdjVoltage(12000);
    delay_ms(1000);
                
    for (int i = 0; i < 10; i++)
        GetLoCurrentUa();
    
    
    // Zener diode should be closed at 12V
    test_ok = (GetLoCurrentUa() < max_zener_leakage_ua);
        
    if (test_ok == true)
        Message("#1 zener 12V", "OK");
    else 
        Message("#1 zener 12V", "FAIL");
    
    return test_ok;
}
// Test zener diodes current at 16V
bool TestZenerProtection(void)
{
    // Test was passed succesfully
    bool test_ok;
    
    // Switch on power
    SetAdjVoltage(16000);
    delay_ms(1000);
    
    GetHiCurrentMa();
    delay_ms(1000);
    
    // Zener diode should be opened at 16V
    test_ok = GetHiCurrentMa() > min_zener_protection_ma;
        
    if (test_ok == true)
        Message("#2 zener 16V", "OK");
    else 
        Message("#2 zener 16V", "FAIL");
    
    return test_ok;
}

// Test board Vout
bool TestVout(void)
{
    // Test was passed succesfully
    bool test_ok;
    int i;
    
    ResetHardwarePi();
    EnablePower();
    SetAdjVoltage(12000);
    delay_ms(500);
    
    test_ok = true;
    
    // Increase load current (max 64 steps)
    for (i = 0; i < 64; i++) {
            
        IncLoadCurrent();
        
        // Desired current, mA
        if (GetLoadCurrentMa() > 100)
            break;
    }
    
    // Can't reach desired load current
    if (i > 60)
        test_ok = false;
    
    // Tested board should provide desired output voltage
    if (test_ok == true)
        test_ok = GetExtVoltage1Mv() > min_out_voltage_mv;
    
    if (test_ok == true)
        Message("#3 Vout", "OK");
    else 
        Message("#3 Vout", "FAIL");
    
    return test_ok;
}

// Test thyristors
// Test connection between thyristor gates and test needles
bool TestConnection(void)
{
    // Test was passed succesfully
    bool test_ok;
    
    // Switch off thyristors
    ResetHardwarePi();
    DisableThyristors();
    
    // Time for thyristors to switch off
    delay_ms(500);
      
    // After connecting load thyristors should NOT be turned on
    EnablePower();
    SetAdjVoltage(12000);
    delay_ms(100);
    
    // All thyristors are disabled so current value should be max possible
    // (thyristor do not limit load current)
    test_ok = GetMaxLoadCurrent() > thyristor_treshold_ma;
    
    if (test_ok == true)
        Message("#4 TR connection", "OK");
    else 
        Message("#4 TR connection", "FAIL");
    
    return test_ok;
}
// Test thyristors one by one while other two thyristors are disabled
bool TestThyristorsOneByOne(void)
{
    // Test was passed succesfully
    bool test_ok;
    int i;
    
    // Test with one thyristor enabled
    for (i = 0; i < 3; i++) {
        
        test_ok = TestThyristor(i);
        if (test_ok == false)
            break;
    }
    
    if (test_ok == true)
       Message("#5 TR 12V", "OK");
    else
        switch (i)
        {
        case 0:
            Message("#5 TR 12V", "TR1 FAIL");
            break;
        case 1:
            Message("#5 TR 12V", "TR2 FAIL");
            break;
        case 2:
            Message("#5 TR 12V", "TR3 FAIL");
            break;
        }
    
    return test_ok;
}
// Test all three thyristors together
bool TestThyristorsTogether(void)
{
    // Test was passed succesfully
    bool test_ok;

    // Test with all three thyristors enabled
    test_ok = TestThyristor(3);
    
    if (test_ok == true)
        Message("#4 TR 12V", "OK");
    else 
        Message("#4 TR 12V", "FAIL");
    
    return test_ok;
}
// Test one of three thyristors (returns current value)
bool TestThyristor(uint8_t num)
{
    // Test was passed succesfully
    bool test_ok;
    
    // Switch off thyristors
    ResetHardwarePi();
    EnableThyristor(num);
    
    // Time for thyristors to switch off
    delay_ms(1000);
      
    // After connecting load thyristors should NOT be turned on
    EnablePower();
    SetAdjVoltage(12000);
    delay_ms(100);
    
    // One thyristor of three is enabled so current value should be less than threshold
    // thyristor pass some current throught so load current decreased
    // but also schematic should be able to provide desired load current
    test_ok = GetMaxLoadCurrent() < thyristor_treshold_ma;
    
    return test_ok;
}


// Get max current given by load 
// if thyristor opens then current flows through it and makes load current less
int GetMaxLoadCurrent(void)
{
    int res, current;
    
    res = 0;
        
    // Increase load current to max (64 steps)
    for (int i = 0; i < 64; i++) {
            
        IncLoadCurrent();
        
        current = GetLoadCurrentMa();
        if (current > res)
            res = current;
         
        // Thyristor opened so no need to increase load current
        if (GetHiCurrentMa() > 50)
            break;
    }
    
    return res;
}
// Wait for Tamper is unpressed
void WaitForTamperUnpressed(void)
{
    
    while (true) {
        
        if (IsPressed(&Tamper) == false)
            break;
    }
}


