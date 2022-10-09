// Functions for testing PI board

// cstd
#include <stdbool.h>

#ifndef __TEST_PI_H__
#define __TEST_PI_H__


// Types
typedef enum
{
    test_t,     // test every thyristor independently by switching others off
    test_no_t,
    ver_t       // verification of device
}
test_mode_t;


// Functions

// Choose between some modes using mode button and screen mode indication
test_mode_t ChooseMode(test_mode_t current_mode);
// Two test modes
void TestWithThyristorsApart(void);
void TestWithThyristorsTogether(void);


// Test zener diodes current at 12V
bool TestZenerLeakage(void);
// Test zener diodes current at 16V
bool TestZenerProtection(void);

// Test board Vout
bool TestVout(void);

// Test thyristors
// Test connection between thyristor gates and test needles
bool TestConnection(void);
// Test thyristors one by one while other two thyristors are disabled
bool TestThyristorsOneByOne(void);
// Test all three thyristors together
bool TestThyristorsTogether(void);

// Get max current given by load 
// if thyristor opens then current flows through it and makes load current less
int GetMaxLoadCurrent(void);

// Blinking display backlight until Start button pressed
void BlinkUntilStartBtn(void);
// Blinking display backlight until Tamper unpressed
void BlinkUntilTamper(void);
// Wait for Start button is pressed
void WaitForStartBtn(void);
// Wait for Tamper is unpressed
void WaitForTamperUnpressed(void);



#endif  // TEST_PI_H
