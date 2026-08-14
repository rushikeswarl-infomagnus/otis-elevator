#include "hal.h"

#include <stdio.h>

static hardware_status_t g_hw = {0};

void hal_init(void)
{
    g_hw.current_floor = 1;
    g_hw.emergency_stop = false;
    g_hw.door_obstructed = false;
    g_hw.fire_mode = false;
    g_hw.over_weight = false;
}

void hal_set_motor(motor_direction_t direction)
{
    (void)direction;
}

void hal_open_door(void)
{
    printf("Door opening\n");
}

void hal_close_door(void)
{
    printf("Door closing\n");
}

void hal_stop_door(void)
{
    printf("Door stopped\n");
}

uint8_t hal_get_current_floor(void)
{
    return g_hw.current_floor;
}

bool hal_floor_sensor_active(uint8_t floor)
{
    return floor == g_hw.current_floor;
}

bool hal_is_emergency_stop(void)
{
    return g_hw.emergency_stop;
}

bool hal_is_door_obstructed(void)
{
    return g_hw.door_obstructed;
}

bool hal_is_over_weight(void)
{
    return g_hw.over_weight;
}

void hal_delay_ms(uint32_t ms)
{
    (void)ms;
}
