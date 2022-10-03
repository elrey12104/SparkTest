#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "input.h"

#define BUTTON_INPUT GPIO_NUM_2
#define ESP_INTR_FLAG_DEFAULT 0
#define BUZZER_OUTPUT GPIO_NUM_5

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void
button_isr_task(void* arg) {
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            updateButton();
        }
    }
}
void app_main(void)
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

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(button_isr_task, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_INPUT, gpio_isr_handler, (void*) BUTTON_INPUT);

    while(1) {
        if (!(*getButtonState())) {
            printf("door open\n");
            gpio_set_level(BUZZER_OUTPUT, 1);
        } else {
            printf("door closed\n");
            gpio_set_level(BUZZER_OUTPUT, 0);
        }
    }
}
