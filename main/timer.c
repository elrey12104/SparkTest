#include "timer.h"
#include "driver/gptimer.h"

static bool timer_state = false;

void
initTimer (gptimer_handle_t* gptimer) {
    gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, gptimer));
    gptimer_alarm_config_t alarm_config = {
    .alarm_count = 15 * 1000 * 1000, // alarm target = 1s @resolution 1MHz
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(*gptimer, &alarm_config));
}

void
resetRestartTimer (gptimer_handle_t gptimer) {
    if (!timer_state) {
        printf("timer enabled\n");
        timer_state = true;
        ESP_ERROR_CHECK(gptimer_enable(gptimer));
        ESP_ERROR_CHECK(gptimer_start(gptimer));
        ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer,0));
    }
}

void
stopTimer(gptimer_handle_t gptimer) {
    if (timer_state) {
        timer_state = false;
        ESP_ERROR_CHECK(gptimer_stop(gptimer));
        ESP_ERROR_CHECK(gptimer_disable(gptimer));
    }
}