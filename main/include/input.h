// *******************************************************
// input.h
//
// Support for a single button operating with an any-edge interrupt on the ESP32 D1 board.
//
// Jacob Elrey
// Last modified:  3.10.2022
// 
// *******************************************************

#define NUM_BUT_POLLS_PUSHED 3
#define NUM_BUT_POLLS_RELEASED 4

// *******************************************************
// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants above.
void
initButton (void);

void
updateButton(void); 
#endif /*INPUTS_H_*/
