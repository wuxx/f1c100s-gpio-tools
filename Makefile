.PHONY: all

all:
	arm-linux-gnueabi-gcc gpio.c -o gpio

clean:
	-rm gpio
