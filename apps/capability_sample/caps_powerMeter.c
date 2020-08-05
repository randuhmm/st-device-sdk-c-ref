/* ***************************************************************************
 *
 * Copyright 2019-2020 Samsung Electronics All Rights Reserved.
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

#include "st_dev.h"
#include "caps_powerMeter.h"

static double caps_powerMeter_get_power_value(caps_powerMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_powerMeter.attr_power.min - 1;
    }
    return caps_data->power_value;
}

static void caps_powerMeter_set_power_value(caps_powerMeter_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->power_value = value;
}

static const char *caps_powerMeter_get_power_unit(caps_powerMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->power_unit;
}

static void caps_powerMeter_set_power_unit(caps_powerMeter_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->power_unit = (char *)unit;
}

static void caps_powerMeter_attr_power_send(caps_powerMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_powerMeter.attr_power.name,
            caps_data->power_value,
            caps_data->power_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send power value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_powerMeter_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_powerMeter_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_powerMeter_attr_power_send(caps_data);
}

caps_powerMeter_data_t *caps_powerMeter_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_powerMeter_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_powerMeter_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_powerMeter_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_powerMeter_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_power_value = caps_powerMeter_get_power_value;
    caps_data->set_power_value = caps_powerMeter_set_power_value;
    caps_data->get_power_unit = caps_powerMeter_get_power_unit;
    caps_data->set_power_unit = caps_powerMeter_set_power_unit;
    caps_data->attr_power_send = caps_powerMeter_attr_power_send;
    caps_data->power_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_powerMeter.id, caps_powerMeter_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init powerMeter handle\n");
    }

    return caps_data;
}
