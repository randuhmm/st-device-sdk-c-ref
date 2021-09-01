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

#include <stdbool.h>
#include <stdint.h>

//#define CONFIG_TARGET_WITTY_CLOUD
//#define CONFIG_TARGET_ESP8266_DEVKITC_V1
#if defined(CONFIG_TARGET_WITTY_CLOUD)

#define GPIO_INPUT_BUTTON 4

#define GPIO_OUTPUT_MAINLED 15
#define GPIO_OUTPUT_MAINLED_0 16 /* use as ground */

#define GPIO_OUTPUT_NOUSE1 12
#define GPIO_OUTPUT_NOUSE2 13

#elif defined(CONFIG_TARGET_ESP8266_DEVKITC_V1)

#define GPIO_INPUT_BUTTON 0

#define GPIO_OUTPUT_MAINLED 14
#define GPIO_OUTPUT_MAINLED_0 15 /* use as ground */

#define GPIO_OUTPUT_NOUSE1 12
#define GPIO_OUTPUT_NOUSE2 13

#else //default

#define GPIO_INPUT_BUTTON_A 4
#define GPIO_INPUT_BUTTON_B 5

#define GPIO_OUTPUT_PWM_A 12
#define GPIO_OUTPUT_PWM_B 14

#define GPIO_INPUT_ROTARY_CLK_A 0
#define GPIO_INPUT_ROTARY_DT_A 2

#define GPIO_INPUT_ROTARY_CLK_B 13
#define GPIO_INPUT_ROTARY_DT_B 15

#endif

// PWM defines
#define PWM_MAX_LEVEL 255
#define PWM_FREQUENCY 100 // Hz
#define PWM_PERIOD ( 1000000 / PWM_FREQUENCY )
#define MAX_DUTY PWM_PERIOD
#define MIN_DUTY 275
#define DELTA_DUTY ( MAX_DUTY - MIN_DUTY )
#define PWM_GAMMA 2.2
#define PWM_ANIMATION_PERIOD 35 // ms
#define PWM_ANIMATION_VELOCITY 0.01
#define PWM_NUM_CHANNELS 2
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1


enum switch_onoff_state {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
};

enum main_led_gpio_state {
    MAINLED_GPIO_ON = 1,
    MAINLED_GPIO_OFF = 0,
};

enum button_gpio_state {
    BUTTON_GPIO_RELEASED = 1,
    BUTTON_GPIO_PRESSED = 0,
};

#define BUTTON_DEBOUNCE_TIME_MS 20
#define BUTTON_LONG_THRESHOLD_MS 5000
#define BUTTON_DELAY_MS 300

enum button_event_type {
    BUTTON_LONG_PRESS = 0,
    BUTTON_SHORT_PRESS = 1,
};

typedef struct pwm_level_data {
    bool power;
    uint8_t channel;
    uint8_t percent;
    uint32_t target_level;
    uint32_t current_level;
    uint32_t current_duty;
} pwm_level_data_t;

typedef struct rotary_encoder_data {
    uint8_t clk_state;
    uint8_t clk_level;
    uint8_t dt_state;
    uint8_t dt_level;
    uint32_t clk_pin;
    uint32_t dt_pin;
} rotary_encoder_data_t;

void change_switch_state_a(int switch_state);
void change_switch_state_b(int switch_state);
void update_color_info(int color_temp);
void change_switch_level(pwm_level_data_t* light, int level);
void change_switch_level_a(int level);
void change_switch_level_b(int level);
void button_isr_handler(void *arg);
int get_button_event_a(int* button_event_type, int* button_event_count);
int get_button_event_b(int* button_event_type, int* button_event_count);
void led_blink(int switch_state, int delay, int count);
void change_led_mode(int noti_led_mode);
void iot_gpio_init(void);

pwm_level_data_t* init_pwm_level_data(uint8_t channel);
rotary_encoder_data_t* init_rotary_encoder_data(uint32_t clk_pin, uint32_t dt_pin);


void update_pwm(pwm_level_data_t* data);
void update_encoder(uint32_t io_num, rotary_encoder_data_t* encoder, pwm_level_data_t* light);
