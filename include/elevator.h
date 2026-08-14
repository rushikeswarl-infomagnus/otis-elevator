#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <stdbool.h>
#include <stdint.h>

#include "hal.h"

typedef enum {
    ELEVATOR_IDLE = 0,
    ELEVATOR_MOVING_UP = 1,
    ELEVATOR_MOVING_DOWN = 2,
    ELEVATOR_DOOR_OPEN = 3,
    ELEVATOR_DOOR_CLOSING = 4,
    ELEVATOR_EMERGENCY_STOP = 5
} elevator_state_t;

typedef enum {
    DIRECTION_UP = 1,
    DIRECTION_DOWN = -1,
    DIRECTION_NONE = 0
} elevator_direction_t;

typedef struct {
    uint8_t current_floor;
    uint8_t target_floor;
    elevator_state_t state;
    elevator_direction_t direction;
    bool emergency_stop;
    bool door_open;
    bool door_obstructed;
    bool fire_mode;
} elevator_context_t;

void elevator_init(elevator_context_t *ctx);
void elevator_reset(elevator_context_t *ctx);
void elevator_request_floor(elevator_context_t *ctx, uint8_t floor);
void elevator_handle_emergency(elevator_context_t *ctx, bool active);
void elevator_tick(elevator_context_t *ctx);
void elevator_run_cycle(elevator_context_t *ctx);
void elevator_print_state(const elevator_context_t *ctx);

#endif
