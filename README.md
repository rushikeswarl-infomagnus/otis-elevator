# Otis Elevator Embedded C Project

This repository contains a starter embedded-C elevator controller design for a generic multi-floor elevator system. The code is organized around a hardware abstraction layer and a state-machine based controller so it can be ported to Cortex-M or any embedded target.

## Project structure

- `include/hal.h` - hardware abstraction interface
- `include/elevator.h` - elevator control API and state model
- `src/hal.c` - hardware stub for simulation and host-side testing
- `src/elevator_controller.c` - request handling and movement logic
- `src/main.c` - example usage and demo loop
- `Makefile` - host build for validation

## Elevator behavior

The initial implementation supports:

- floor request queue
- directional movement (`UP`, `DOWN`, `IDLE`)
- door open/close transitions
- emergency stop handling
- floor indicator updates

## Build and run

```bash
make
./build/elevator_app
```

## Run unit tests

```bash
make test
```

## How to port to real hardware

1. Replace the HAL layer in `src/hal.c` with the MCU-specific driver calls.
2. Connect door motor, traction motor, limit switches, and hall sensors to the HAL interface.
3. Add a real-time scheduler or timer interrupt that calls `elevator_tick()` at a fixed interval.
4. Extend the request queue to support destination and hall-call logic.

## Suggested next features

- priority queuing for emergency or VIP calls
- acceleration/deceleration profiles
- door timing and obstruction detection
- fault detection and watchdog supervision
- logging over UART / CAN / Modbus

This project is intentionally a clean starting point, not a full production elevator system.
