// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>
#include <stdint.h>

//user
#include "main.h"
#include "init.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "debug_led.h"
#include "button.h"
#include "hardware.h"
#include "i2c.h"
#include "display.h"
#include "dig_res.h"
#include "test_pi.h"
#include "verification_pi.h"




int main()
{    
    
    // System init
    InitRcc();
    InitGpio();
    InitSysTick();
    //InitUart();
    InitI2c();
    InitAdc();
    InitInterrupt();
    InitIwdg();
    
    // Test mode
    test_mode_t test_mode = test_t;

    
    delay_ms(1000);
    InitDisp();
    
    ResetHardwarePi();
    
    Message("TEST", "");
    
    while (true) {

        
        test_mode = ChooseMode(test_mode);
        
        switch (test_mode)
        {
        case test_t:
            // Test PI board
            TestWithThyristorsApart();
            ResetHardwarePi();
            break;
            
        case test_no_t:
            // Test PI bord in enclosure
            TestWithThyristorsTogether();
            break;
            
        case ver_t:
            // Verification of device
            VerificationPi();
            break;
        }
        

    }
    
        
}


