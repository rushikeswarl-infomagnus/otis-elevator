#include "elevator.h"

#include <stdio.h>

static bool elevator_has_pending_request(const elevator_context_t *ctx)
{
    return ctx->target_floor != 0;
}

void elevator_init(elevator_context_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    ctx->current_floor = 1;
    ctx->target_floor = 0;
    ctx->state = ELEVATOR_IDLE;
    ctx->direction = DIRECTION_NONE;
    ctx->emergency_stop = false;
    ctx->door_open = false;
    ctx->door_obstructed = false;
    ctx->fire_mode = false;

    hal_init();
}

void elevator_reset(elevator_context_t *ctx)
{
    elevator_init(ctx);
}

void elevator_request_floor(elevator_context_t *ctx, uint8_t floor)
{
    if (ctx == NULL) {
        return;
    }

    if (floor < ELEVATOR_MIN_FLOOR || floor > ELEVATOR_MAX_FLOORS) {
        return;
    }

    ctx->target_floor = floor;

    if (ctx->current_floor < floor) {
        ctx->direction = DIRECTION_UP;
        ctx->state = ELEVATOR_MOVING_UP;
    } else if (ctx->current_floor > floor) {
        ctx->direction = DIRECTION_DOWN;
        ctx->state = ELEVATOR_MOVING_DOWN;
    } else {
        ctx->direction = DIRECTION_NONE;
        ctx->state = ELEVATOR_DOOR_OPEN;
        ctx->door_open = true;
        ctx->target_floor = 0;
    }
}

void elevator_handle_emergency(elevator_context_t *ctx, bool active)
{
    if (ctx == NULL) {
        return;
    }

    ctx->emergency_stop = active;
    if (active) {
        ctx->state = ELEVATOR_EMERGENCY_STOP;
        ctx->direction = DIRECTION_NONE;
        hal_set_motor(MOTOR_STOP);
        hal_stop_door();
        return;
    }

    ctx->state = ELEVATOR_IDLE;
    ctx->direction = DIRECTION_NONE;
}

void elevator_tick(elevator_context_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->emergency_stop || hal_is_emergency_stop()) {
        ctx->state = ELEVATOR_EMERGENCY_STOP;
        return;
    }

    if (!elevator_has_pending_request(ctx)) {
        ctx->state = ELEVATOR_IDLE;
        ctx->direction = DIRECTION_NONE;
        return;
    }

    if (ctx->door_open) {
        ctx->state = ELEVATOR_DOOR_OPEN;
        if (hal_is_door_obstructed() || hal_is_over_weight()) {
            ctx->state = ELEVATOR_EMERGENCY_STOP;
            ctx->emergency_stop = true;
        }
        return;
    }

    if (ctx->current_floor < ctx->target_floor) {
        ctx->direction = DIRECTION_UP;
        ctx->state = ELEVATOR_MOVING_UP;
        ctx->current_floor++;
        if (ctx->current_floor == ctx->target_floor) {
            ctx->state = ELEVATOR_DOOR_OPEN;
            ctx->door_open = true;
            ctx->target_floor = 0;
            hal_open_door();
            hal_set_motor(MOTOR_STOP);
        } else {
            hal_set_motor(MOTOR_UP);
        }
    } else if (ctx->current_floor > ctx->target_floor) {
        ctx->direction = DIRECTION_DOWN;
        ctx->state = ELEVATOR_MOVING_DOWN;
        ctx->current_floor--;
        if (ctx->current_floor == ctx->target_floor) {
            ctx->state = ELEVATOR_DOOR_OPEN;
            ctx->door_open = true;
            ctx->target_floor = 0;
            hal_open_door();
            hal_set_motor(MOTOR_STOP);
        } else {
            hal_set_motor(MOTOR_DOWN);
        }
    } else {
        ctx->state = ELEVATOR_DOOR_OPEN;
        ctx->door_open = true;
        ctx->target_floor = 0;
        hal_open_door();
    }
}

void elevator_run_cycle(elevator_context_t *ctx)
{
    elevator_tick(ctx);
    elevator_print_state(ctx);
}

void elevator_print_state(const elevator_context_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    printf("Floor: %u | State: %d | Direction: %d | DoorOpen: %s\n",
           (unsigned)ctx->current_floor,
           ctx->state,
           ctx->direction,
           ctx->door_open ? "true" : "false");
}
