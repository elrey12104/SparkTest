#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "input.h"
#include "buzzer.h"
#include "timer.h"

#define BUTTON_INPUT GPIO_NUM_2
#define ESP_INTR_FLAG_DEFAULT 0
#define BUZZER_OUTPUT GPIO_NUM_5

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t queue = NULL;
static gptimer_handle_t gptimer = NULL;

typedef struct {
    uint64_t event_count;
} example_queue_element_t;

static bool IRAM_ATTR
example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // Stop timer the sooner the better
    gptimer_stop(timer);
    // Retrieve the count value from event data
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    // Optional: send the event data to other task by OS queue
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return high_task_awoken == pdTRUE;
}

void
timer_isr_task(void* arg) {
    uint32_t clk_cnt;
    bool* butState = getButtonState();
    for(;;) {
        if(xQueueReceive(queue, &clk_cnt, portMAX_DELAY)) {
            while(!(*butState)) gpio_set_level(BUZZER_OUTPUT, 1);
            ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer,0));
            gpio_set_level(BUZZER_OUTPUT, 0);
        }
    }
}

static void IRAM_ATTR 
gpio_isr_handler(void* arg) {
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

void 
timerIntHandler(void) {
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // register user callback
    };
    queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(timer_isr_task, "timer_task_example", 2048, NULL, 10, NULL);
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
}

void 
gpioIntHandler(void) {
    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(button_isr_task, "gpio_task_example", 2048, NULL, 10, NULL);
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_INPUT, gpio_isr_handler, (void*) BUTTON_INPUT);
}

void app_main(void)
{
    initButton();
    initBuzzer();
    initTimer(&gptimer);

    timerIntHandler();
    gpioIntHandler();

    bool* butState = getButtonState();
    while(1) {
        if (!(*butState)) resetRestartTimer(gptimer);
        else stopTimer(gptimer);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
