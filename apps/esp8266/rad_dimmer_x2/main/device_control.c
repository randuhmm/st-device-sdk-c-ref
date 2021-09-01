/* ***************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pwm.h"

#include <math.h>

static os_timer_t ledTimer;
static const uint32_t period = PWM_PERIOD;
static uint32_t gammaTable[PWM_MAX_LEVEL + 1];

static pwm_level_data_t* light_a;
static pwm_level_data_t* light_b;

static rotary_encoder_data_t* encoder_a;
static rotary_encoder_data_t* encoder_b;

static uint32_t duties[PWM_NUM_CHANNELS] = { 0, 0 };
static uint32_t pins[PWM_NUM_CHANNELS] = { GPIO_OUTPUT_PWM_A, GPIO_OUTPUT_PWM_B };

pwm_level_data_t* init_pwm_level_data(uint8_t channel) {
    pwm_level_data_t *data = NULL;

    data = malloc(sizeof(pwm_level_data_t));
    if (!data) {
        printf("fail to malloc for pwm_level_data\n");
        return NULL;
    }

    memset(data, 0, sizeof(pwm_level_data_t));

    data->power = false;
    data->channel = channel;
    data->percent = 100;
    data->target_level = PWM_MAX_LEVEL;
    data->current_level = 0;
    data->current_duty = 0;

    return data;
}

rotary_encoder_data_t* init_rotary_encoder_data(uint32_t clk_pin, uint32_t dt_pin) {
    rotary_encoder_data_t *data = NULL;

    data = malloc(sizeof(rotary_encoder_data_t));
    if (!data) {
        printf("fail to malloc for rotary_encoder_data\n");
        return NULL;
    }

    memset(data, 0, sizeof(rotary_encoder_data_t));

    data->clk_state = 0;
    data->clk_level = 0;
    data->dt_state = 0;
    data->dt_level = 0;
    data->clk_pin = clk_pin;
    data->dt_pin = dt_pin;

    return data;
}

void change_switch_state_a(int switch_state)
{
    printf("change_switch_state_a %d\n", switch_state);
    if (switch_state == SWITCH_OFF) {
        light_a->power = false;
    } else {
        light_a->power = true;
    }
}

void change_switch_state_b(int switch_state)
{
    printf("change_switch_state_b %d\n", switch_state);
    if (switch_state == SWITCH_OFF) {
        light_b->power = false;
    } else {
        light_b->power = true;
    }
}

void change_switch_level(pwm_level_data_t* light, int level) {
    light->percent = level;
    light->target_level = (uint32_t)(level / 100.0 * PWM_MAX_LEVEL);
}

void change_switch_level_a(int level)
{
    printf("change_switch_level_a changed to %d\n", level);
    light_a->percent = level;
    light_a->target_level = (uint32_t)(level / 100.0 * PWM_MAX_LEVEL);
}

void change_switch_level_b(int level)
{
    printf("change_switch_level_b changed to %d\n", level);
    light_b->percent = level;
    light_b->target_level = (uint32_t)(level / 100.0 * PWM_MAX_LEVEL);
}

int get_button_event_a(int* button_event_type, int* button_event_count)
{
    static uint32_t button_count = 0;
    static uint32_t button_last_state = BUTTON_GPIO_RELEASED;
    static TimeOut_t button_timeout;
    static TickType_t long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
    static TickType_t button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);

    uint32_t gpio_level = 0;

    gpio_level = gpio_get_level(GPIO_INPUT_BUTTON_A);
    if (button_last_state != gpio_level) {
        /* wait debounce time to ignore small ripple of currunt */
        vTaskDelay( pdMS_TO_TICKS(BUTTON_DEBOUNCE_TIME_MS) );
        gpio_level = gpio_get_level(GPIO_INPUT_BUTTON_A);
        if (button_last_state != gpio_level) {
            printf("Button event, val: %d, tick: %u\n", gpio_level, (uint32_t)xTaskGetTickCount());
            button_last_state = gpio_level;
            if (gpio_level == BUTTON_GPIO_PRESSED) {
                button_count++;
            }
            vTaskSetTimeOutState(&button_timeout);
            button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);
            long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
        }
    } else if (button_count > 0) {
        if ((gpio_level == BUTTON_GPIO_PRESSED)
                && (xTaskCheckForTimeOut(&button_timeout, &long_press_tick ) != pdFALSE)) {
            *button_event_type = BUTTON_LONG_PRESS;
            *button_event_count = 1;
            button_count = 0;
            return true;
        } else if ((gpio_level == BUTTON_GPIO_RELEASED)
                && (xTaskCheckForTimeOut(&button_timeout, &button_delay_tick ) != pdFALSE)) {
            *button_event_type = BUTTON_SHORT_PRESS;
            *button_event_count = button_count;
            button_count = 0;
            return true;
        }
    }

    return false;
}

int get_button_event_b(int* button_event_type, int* button_event_count)
{
    static uint32_t button_count = 0;
    static uint32_t button_last_state = BUTTON_GPIO_RELEASED;
    static TimeOut_t button_timeout;
    static TickType_t long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
    static TickType_t button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);

    uint32_t gpio_level = 0;

    gpio_level = gpio_get_level(GPIO_INPUT_BUTTON_B);
    if (button_last_state != gpio_level) {
        /* wait debounce time to ignore small ripple of currunt */
        vTaskDelay( pdMS_TO_TICKS(BUTTON_DEBOUNCE_TIME_MS) );
        gpio_level = gpio_get_level(GPIO_INPUT_BUTTON_B);
        if (button_last_state != gpio_level) {
            printf("Button event, val: %d, tick: %u\n", gpio_level, (uint32_t)xTaskGetTickCount());
            button_last_state = gpio_level;
            if (gpio_level == BUTTON_GPIO_PRESSED) {
                button_count++;
            }
            vTaskSetTimeOutState(&button_timeout);
            button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);
            long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
        }
    } else if (button_count > 0) {
        if ((gpio_level == BUTTON_GPIO_PRESSED)
                && (xTaskCheckForTimeOut(&button_timeout, &long_press_tick ) != pdFALSE)) {
            *button_event_type = BUTTON_LONG_PRESS;
            *button_event_count = 1;
            button_count = 0;
            return true;
        } else if ((gpio_level == BUTTON_GPIO_RELEASED)
                && (xTaskCheckForTimeOut(&button_timeout, &button_delay_tick ) != pdFALSE)) {
            *button_event_type = BUTTON_SHORT_PRESS;
            *button_event_count = button_count;
            button_count = 0;
            return true;
        }
    }

    return false;
}

void ICACHE_FLASH_ATTR ledTimer_f(void *args) {
    update_pwm(light_a);
    update_pwm(light_b);
}

void update_pwm(pwm_level_data_t* data) {
    if(data->power) {
        if(data->current_level == data->target_level) {
            return;
        } else if (data->current_level < data->target_level) {
            data->current_level += PWM_MAX_LEVEL * PWM_ANIMATION_VELOCITY;
            if(data->current_level > data->target_level) {
                data->current_level = data->target_level;
            }
        } else if(data->current_level > data->target_level) {
            data->current_level -= PWM_MAX_LEVEL * PWM_ANIMATION_VELOCITY;
            if(data->current_level < data->target_level || data->current_level > PWM_MAX_LEVEL) {
                data->current_level = data->target_level;
            }
        }
    } else {
        if(data->current_level == 0) {
            if(data->current_duty != 0) {
                data->current_duty = 0;                
                pwm_set_duty(data->channel, data->current_duty);
                pwm_start();
            }
            return;
        } else {
            data->current_level -= PWM_MAX_LEVEL * PWM_ANIMATION_VELOCITY;
            if(data->current_level > PWM_MAX_LEVEL) {
                data->current_level = 0;
            }
        }
    }
    data->current_duty = gammaTable[data->current_level];
    pwm_set_duty(data->channel, data->current_duty);
    pwm_start();
}


static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


static void update_encoders(void* arg) {
    uint32_t io_num;
    uint8_t level;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if(io_num == GPIO_INPUT_ROTARY_CLK_A || io_num == GPIO_INPUT_ROTARY_DT_A)
                update_encoder(io_num, encoder_a, light_a);
            else if(io_num == GPIO_INPUT_ROTARY_CLK_B || io_num == GPIO_INPUT_ROTARY_DT_B) {
                update_encoder(io_num, encoder_b, light_b);
            }
        }
    }
}

void update_encoder(uint32_t io_num, rotary_encoder_data_t* encoder, pwm_level_data_t* light) {
    uint8_t level;
    level = gpio_get_level(io_num);
    if(io_num == encoder->clk_pin && level != encoder->clk_level) {
        encoder->clk_level = level;
        if(encoder->clk_level != encoder->dt_level) {
            if(encoder->clk_level == 1 && encoder->clk_state == 1) {
                if(light->percent <= 95 && light->power) {
                    printf("ROTARY CLK\n");
                    change_switch_level(light, light->percent + 5);
                } else {
                    change_switch_level(light, 100);
                }
                encoder->clk_state = 2;
            } else if(encoder->clk_level == 0) {
                encoder->clk_state = 1;
            } else {
                encoder->clk_state = 0;
            }
        }
    } else if(io_num == encoder->dt_pin && level != encoder->dt_level) {
        encoder->dt_level = level;
        if(encoder->dt_level != encoder->clk_level) {
            if(encoder->dt_level == 1 && encoder->dt_state == 1) {
                if(light->percent >= 5 && light->power) {
                    printf("ROTARY DT\n");
                    change_switch_level(light, light->percent - 5);
                } else {
                    change_switch_level(light, 0);
                }
                encoder->dt_state = 2;
            } else if(encoder->dt_level == 0) {
                encoder->dt_state = 1;
            } else {
                encoder->dt_state = 0;
            }
        }
    }
}

void iot_gpio_init(void)
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = (BUTTON_GPIO_RELEASED == 0);
    io_conf.pull_up_en = (BUTTON_GPIO_RELEASED == 1);

    // Switch
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_BUTTON_A;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_BUTTON_A, GPIO_INTR_ANYEDGE);
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_BUTTON_B;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_BUTTON_B, GPIO_INTR_ANYEDGE);

    // Encoder A
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_ROTARY_CLK_A;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_ROTARY_CLK_A, GPIO_INTR_ANYEDGE);
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_ROTARY_DT_A;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_ROTARY_DT_A, GPIO_INTR_ANYEDGE);
    
    // Encoder B
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_ROTARY_CLK_B;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_ROTARY_CLK_B, GPIO_INTR_ANYEDGE);
    io_conf.pin_bit_mask = 1 << GPIO_INPUT_ROTARY_DT_B;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_ROTARY_DT_B, GPIO_INTR_ANYEDGE);


    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(update_encoders, "update_encoders", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);

    // initialize the data structures
    encoder_a = init_rotary_encoder_data(GPIO_INPUT_ROTARY_CLK_A, GPIO_INPUT_ROTARY_DT_A);
    encoder_b = init_rotary_encoder_data(GPIO_INPUT_ROTARY_CLK_B, GPIO_INPUT_ROTARY_DT_B);
    encoder_a->clk_level = gpio_get_level(GPIO_INPUT_ROTARY_CLK_A);
    encoder_a->dt_level = gpio_get_level(GPIO_INPUT_ROTARY_DT_A);
    encoder_b->clk_level = gpio_get_level(GPIO_INPUT_ROTARY_CLK_B);
    encoder_b->dt_level = gpio_get_level(GPIO_INPUT_ROTARY_DT_B);
    
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_ROTARY_CLK_A, gpio_isr_handler, (void*) GPIO_INPUT_ROTARY_CLK_A);
    gpio_isr_handler_add(GPIO_INPUT_ROTARY_DT_A, gpio_isr_handler, (void*) GPIO_INPUT_ROTARY_DT_A);
    gpio_isr_handler_add(GPIO_INPUT_ROTARY_CLK_B, gpio_isr_handler, (void*) GPIO_INPUT_ROTARY_CLK_B);
    gpio_isr_handler_add(GPIO_INPUT_ROTARY_DT_B, gpio_isr_handler, (void*) GPIO_INPUT_ROTARY_DT_B);

    // setup pwm
    light_a = init_pwm_level_data(PWM_CHANNEL_A);
    light_b = init_pwm_level_data(PWM_CHANNEL_B);
    pwm_init(period, &duties, PWM_NUM_CHANNELS, &pins);
    pwm_set_phase(PWM_CHANNEL_A, 0);
    pwm_set_phase(PWM_CHANNEL_B, 0);

    // setup gamma table
    for(int i = 0; i <= PWM_MAX_LEVEL; i++) {
        gammaTable[i] = (uint32_t)(DELTA_DUTY * pow((float)i / PWM_MAX_LEVEL, PWM_GAMMA)) + MIN_DUTY;
        printf("%d - %d\n", i, gammaTable[i]);
    }

    // setup timer for animation of dimmer
    os_timer_setfn(&ledTimer, (os_timer_func_t *)ledTimer_f, NULL);
    os_timer_arm(&ledTimer, PWM_ANIMATION_PERIOD, 1);
}


