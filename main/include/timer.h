// *******************************************************
// timer.h
//
// Support for a single button operating with an any-edge interrupt on the ESP32 D1 board.
//
// Jacob Elrey
// Last modified:  3.10.2022
// 
// *******************************************************

#ifndef TIMER_H_
#define TIMER_H_
#include "driver/gptimer.h"

void
initTimer (gptimer_handle_t* gptimer);

void
resetRestartTimer (gptimer_handle_t gptimer);

void
stopTimer(gptimer_handle_t gptimer); 
#endif /*TIMER_H_*/
