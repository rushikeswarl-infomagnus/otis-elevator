CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iinclude
TARGET = build/elevator_app
TEST_TARGET = build/test_elevator
SRC = src/main.c src/hal.c src/elevator_controller.c
OBJ = $(SRC:.c=.o)
TEST_SRC = tests/test_elevator.c
TEST_OBJ = build/test_elevator.o build/hal_test.o build/elevator_controller_test.o

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(TEST_TARGET): $(TEST_OBJ)
	@mkdir -p build
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -o $@ $(TEST_OBJ)

build/test_elevator.o: $(TEST_SRC) include/elevator.h
	@mkdir -p build
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -c $< -o $@

build/hal_test.o: src/hal.c include/hal.h
	@mkdir -p build
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -c $< -o $@

build/elevator_controller_test.o: src/elevator_controller.c include/elevator.h
	@mkdir -p build
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -c $< -o $@

test: $(TEST_TARGET)
	rm -f build/*.gcda
	./$(TEST_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build src/*.o tests/*.o

.PHONY: all clean test
