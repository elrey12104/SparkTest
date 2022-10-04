// *******************************************************
// input.h
//
// Support for a single button operating with an any-edge interrupt on the ESP32 D1 board.
//
// Jacob Elrey
// Last modified:  3.10.2022
// 
// *******************************************************
#ifndef INPUT_H_
#define INPUT_H_
#include <stdbool.h>

#define NUM_BUT_POLLS_PUSHED 3
#define NUM_BUT_POLLS_RELEASED 4

// *******************************************************
// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants above.
void
initButton (void);

bool*
getButtonState (void);

void
updateButton(void); 
#endif /*INPUT_H_*/
