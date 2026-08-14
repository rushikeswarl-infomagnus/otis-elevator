CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iinclude
TARGET = build/elevator_app
SRC = src/main.c src/hal.c src/elevator_controller.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build src/*.o

.PHONY: all clean
