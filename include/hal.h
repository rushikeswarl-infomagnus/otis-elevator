#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stdint.h>

#define ELEVATOR_MAX_FLOORS 20
#define ELEVATOR_MIN_FLOOR 1

typedef enum {
    FLOOR_SENSOR_OFF = 0,
    FLOOR_SENSOR_ON = 1
} floor_sensor_state_t;

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_UP = 1,
    MOTOR_DOWN = 2
} motor_direction_t;

typedef enum {
    DOOR_CLOSED = 0,
    DOOR_OPENING = 1,
    DOOR_OPEN = 2,
    DOOR_CLOSING = 3,
    DOOR_FAULT = 4
} door_state_t;

typedef struct {
    uint8_t current_floor;
    bool emergency_stop;
    bool door_obstructed;
    bool fire_mode;
    bool over_weight;
} hardware_status_t;

void hal_init(void);
void hal_set_motor(motor_direction_t direction);
void hal_open_door(void);
void hal_close_door(void);
void hal_stop_door(void);
uint8_t hal_get_current_floor(void);
bool hal_floor_sensor_active(uint8_t floor);
bool hal_is_emergency_stop(void);
bool hal_is_door_obstructed(void);
bool hal_is_over_weight(void);
void hal_delay_ms(uint32_t ms);

#endif
