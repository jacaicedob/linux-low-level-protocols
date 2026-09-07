CC      := gcc
CFLAGS  := -I. -Wall -Wextra
BUILD   := build
TARGET  := $(BUILD)/i2c.out
SRCS    := main.c i2c-driver.c mpu6050.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

