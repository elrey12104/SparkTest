#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// *******************************************************
// inputs.c
//
// Support for a single button operating with an any-edge interrupt on the ESP32 D1 board.
//
// Jacob Elrey
// Last modified:  3.10.2022
// 
// *******************************************************

#include "input.h"
#define BUTTON_INPUT GPIO_NUM_2

enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};
static bool but_state = 0;	// Corresponds to the electrical state
static uint8_t but_count = 0;

void 
initButton(void) 
{
    gpio_config_t in_conf = {
    //interrupt of rising edge
    .intr_type = GPIO_INTR_ANYEDGE,
    //bit mask of the pins, use GPIO4/5 here
    .pin_bit_mask = (1<<BUTTON_INPUT),
    //set as input mode
    .mode = GPIO_MODE_INPUT,
    //disable pull-up mode
    .pull_up_en = 0,
    //enable pull-down mode
    .pull_down_en = 1
    };
    gpio_config(&in_conf);
}

bool* getButtonState()
{
    return &but_state;
}

void 
updateButton(void)
{
    bool but_value = gpio_get_level(BUTTON_INPUT);
    if (but_value != but_state) {
        but_count++;
        if (but_value == PUSHED && but_count >= NUM_BUT_POLLS_PUSHED) {
            but_state = but_value;
        } else if (but_value == RELEASED && but_count >= NUM_BUT_POLLS_RELEASED) {
            but_state = but_value;
            but_count = 0;
        } else if (but_value == PUSHED) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            updateButton();
        }
    } else {
        but_count = 0;
    }
}