# src Makefile
include ../Makefile.common

PATH_HARDWARE		= hardware
PATH_MAIN			= main

CFLAGS				+= $(CFLAGSlib)
CFLAGS				+= -Iinclude

all: src

src: mkdir objs app.a

mkdir:
	mkdir -p release

objs:
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/startup.o -c $(PATH_MAIN)/startup_stm32f10x_md.s
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/main.o -c $(PATH_MAIN)/main.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/printf.o -c $(PATH_HARDWARE)/printf.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/sysclk.o -c $(PATH_HARDWARE)/sysclk.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/timer.o -c $(PATH_HARDWARE)/timer.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/led.o -c $(PATH_HARDWARE)/led.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/btn.o -c $(PATH_HARDWARE)/btn.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/uart.o -c $(PATH_HARDWARE)/uart.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/pwm.o -c $(PATH_HARDWARE)/pwm.c
	$(CC) $(CFLAGS) -o $(PATH_TARGET)/nrf.o -c $(PATH_HARDWARE)/nrf.c

app.a:
	$(AR) cr $(PATH_TARGET)/app.a $(PATH_TARGET)/startup.o \
		$(PATH_TARGET)/main.o \
		$(PATH_TARGET)/printf.o \
		$(PATH_TARGET)/sysclk.o \
		$(PATH_TARGET)/timer.o \
		$(PATH_TARGET)/led.o \
		$(PATH_TARGET)/btn.o \
		$(PATH_TARGET)/uart.o \
		$(PATH_TARGET)/pwm.o \
		$(PATH_TARGET)/nrf.o
	
clean:
	rm -f $(PATH_TARGET)/*
	
