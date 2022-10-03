#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUTTON_INPUT GPIO_NUM_2
#define ESP_INTR_FLAG_DEFAULT 0
#define BUZZER_OUTPUT GPIO_NUM_5

#define NUM_BUT_POLLS_PUSHED 3
#define NUM_BUT_POLLS_RELEASED 4

enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};
static bool but_state = 0;	// Corresponds to the electrical state
static uint8_t but_count = 0;

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

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(button_isr_task, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_INPUT, gpio_isr_handler, (void*) BUTTON_INPUT);

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (!(*getButtonState())) {
            printf("door open\n");
            gpio_set_level(BUZZER_OUTPUT, 1);
        } else {
            printf("door closed\n");
            gpio_set_level(BUZZER_OUTPUT, 0);
        }
    }
}
