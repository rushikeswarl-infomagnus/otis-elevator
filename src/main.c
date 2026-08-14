#include "elevator.h"

#include <stdio.h>

int main(void)
{
    elevator_context_t elevator = {0};

    elevator_init(&elevator);

    printf("Elevator controller booted\n");

    elevator_request_floor(&elevator, 4);
    elevator_run_cycle(&elevator);
    elevator_run_cycle(&elevator);
    elevator_run_cycle(&elevator);
    elevator_run_cycle(&elevator);

    elevator_request_floor(&elevator, 2);
    elevator_run_cycle(&elevator);
    elevator_run_cycle(&elevator);
    elevator_run_cycle(&elevator);

    elevator_handle_emergency(&elevator, true);
    elevator_print_state(&elevator);

    return 0;
}
