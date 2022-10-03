
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUZZER_OUTPUT GPIO_NUM_5

void
initBuzzer(void)
{  
gpio_config_t out_conf = {
    //interrupt of rising edge
    .intr_type = GPIO_INTR_DISABLE,
    //bit mask of the pins, use GPIO4/5 here
    .pin_bit_mask = (1<<BUZZER_OUTPUT),
    //set as input mode
    .mode = GPIO_MODE_OUTPUT,
    //disable pull-up mode
    .pull_up_en = 0,
    //enable pull-down mode
    .pull_down_en = 0
    };    //change gpio interrupt type for one pin
    gpio_config(&out_conf);
}