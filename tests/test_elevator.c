#include <stdio.h>

#include "elevator.h"

static int g_tests_run = 0;
static int g_tests_failed = 0;

#define CHECK(condition)                                                                              \
    do {                                                                                              \
        g_tests_run++;                                                                               \
        if (!(condition)) {                                                                          \
            fprintf(stderr, "FAILED: %s:%d: %s\n", __FILE__, __LINE__, #condition);             \
            g_tests_failed++;                                                                        \
        }                                                                                             \
    } while (0)

static void test_init_and_request(void)
{
    elevator_context_t ctx = {0};

    elevator_init(&ctx);
    CHECK(ctx.current_floor == 1);
    CHECK(ctx.target_floor == 0);
    CHECK(ctx.state == ELEVATOR_IDLE);
    CHECK(ctx.direction == DIRECTION_NONE);
    CHECK(hal_get_current_floor() == 1);
    CHECK(!hal_is_emergency_stop());

    elevator_init(NULL);
    elevator_request_floor(NULL, 2);
    elevator_request_floor(&ctx, 21);
    CHECK(ctx.target_floor == 0);

    elevator_reset(&ctx);
    CHECK(ctx.state == ELEVATOR_IDLE);

    elevator_request_floor(&ctx, 4);
    CHECK(ctx.target_floor == 4);
    CHECK(ctx.state == ELEVATOR_MOVING_UP);
    CHECK(ctx.direction == DIRECTION_UP);

    elevator_request_floor(&ctx, 0);
    CHECK(ctx.target_floor == 4);

    elevator_request_floor(&ctx, 1);
    CHECK(ctx.state == ELEVATOR_DOOR_OPEN);
    CHECK(ctx.door_open);
    CHECK(ctx.target_floor == 0);
}

static void test_downward_and_emergency_paths(void)
{
    elevator_context_t ctx = {0};

    elevator_init(&ctx);
    ctx.current_floor = 5;
    elevator_request_floor(&ctx, 2);
    CHECK(ctx.state == ELEVATOR_MOVING_DOWN);
    CHECK(ctx.direction == DIRECTION_DOWN);

    elevator_handle_emergency(NULL, true);
    elevator_tick(NULL);
    elevator_handle_emergency(&ctx, true);
    CHECK(ctx.state == ELEVATOR_EMERGENCY_STOP);
    CHECK(ctx.emergency_stop);
    CHECK(ctx.direction == DIRECTION_NONE);

    elevator_handle_emergency(&ctx, false);
    CHECK(ctx.state == ELEVATOR_IDLE);
    CHECK(!ctx.emergency_stop);
}

static void test_tick_behaviors(void)
{
    elevator_context_t ctx = {0};

    elevator_init(&ctx);
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_IDLE);
    CHECK(ctx.direction == DIRECTION_NONE);

    elevator_request_floor(&ctx, 3);
    elevator_tick(&ctx);
    CHECK(ctx.current_floor == 2);
    CHECK(ctx.state == ELEVATOR_MOVING_UP);
    CHECK(ctx.direction == DIRECTION_UP);

    elevator_tick(&ctx);
    CHECK(ctx.current_floor == 3);
    CHECK(ctx.state == ELEVATOR_DOOR_OPEN);
    CHECK(ctx.door_open);
    CHECK(ctx.target_floor == 0);

    ctx.current_floor = 2;
    ctx.target_floor = 2;
    ctx.door_open = false;
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_DOOR_OPEN);
    CHECK(ctx.door_open);
    CHECK(ctx.target_floor == 0);

    ctx.current_floor = 3;
    ctx.target_floor = 1;
    ctx.door_open = false;
    ctx.state = ELEVATOR_IDLE;
    ctx.direction = DIRECTION_NONE;
    elevator_tick(&ctx);
    CHECK(ctx.current_floor == 2);
    CHECK(ctx.state == ELEVATOR_MOVING_DOWN);
    CHECK(ctx.direction == DIRECTION_DOWN);

    ctx.emergency_stop = true;
    hal_set_emergency_stop(false);
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_EMERGENCY_STOP);

    ctx.current_floor = 2;
    ctx.target_floor = 1;
    ctx.door_open = false;
    ctx.emergency_stop = false;
    ctx.state = ELEVATOR_IDLE;
    ctx.direction = DIRECTION_NONE;
    elevator_tick(&ctx);
    CHECK(ctx.current_floor == 1);
    CHECK(ctx.state == ELEVATOR_DOOR_OPEN);
    CHECK(ctx.door_open);
    CHECK(ctx.target_floor == 0);
}

static void test_door_and_hal_states(void)
{
    elevator_context_t ctx = {0};

    elevator_init(&ctx);
    ctx.door_open = true;
    ctx.target_floor = 2;
    hal_set_door_obstructed(true);
    hal_set_over_weight(false);
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_EMERGENCY_STOP);
    CHECK(ctx.emergency_stop);

    hal_set_door_obstructed(false);
    hal_set_over_weight(true);
    ctx.emergency_stop = false;
    ctx.state = ELEVATOR_DOOR_OPEN;
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_EMERGENCY_STOP);
    CHECK(ctx.emergency_stop);

    hal_set_door_obstructed(false);
    hal_set_over_weight(false);
    ctx.emergency_stop = false;
    ctx.state = ELEVATOR_DOOR_OPEN;
    elevator_tick(&ctx);
    CHECK(ctx.state == ELEVATOR_DOOR_OPEN);

    hal_set_current_floor(2);
    hal_set_emergency_stop(true);
    hal_set_door_obstructed(false);
    hal_set_over_weight(false);
    hal_open_door();
    hal_close_door();
    hal_stop_door();
    hal_delay_ms(5);
    CHECK(hal_floor_sensor_active(2));
    CHECK(!hal_floor_sensor_active(3));
    CHECK(hal_is_emergency_stop());
    CHECK(!hal_is_door_obstructed());
    CHECK(!hal_is_over_weight());
}

static void test_run_cycle_and_print(void)
{
    elevator_context_t ctx = {0};

    elevator_init(&ctx);
    elevator_run_cycle(&ctx);
    ctx.door_open = true;
    elevator_print_state(&ctx);
    elevator_print_state(NULL);
}

int main(void)
{
    test_init_and_request();
    test_downward_and_emergency_paths();
    test_tick_behaviors();
    test_door_and_hal_states();
    test_run_cycle_and_print();

    if (g_tests_failed > 0) {
        fprintf(stderr, "Test suite failed: %d/%d checks failed\n", g_tests_failed, g_tests_run);
        return 1;
    }

    printf("All tests passed (%d checks)\n", g_tests_run);
    return 0;
}
