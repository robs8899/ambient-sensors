CC = cc
LD = cc
RM = rm

CFLAGS  = -g0 -O2 -Wall -fdata-sections -ffunction-sections
LDFLAGS = -Wl,--gc-sections
RMFLAGS = -f

default: all

all: util.o gpio.o spi.o bmp180.o si7021.o si4432.o
	$(LD) util.o bmp180.o -o bmp180 $(LDFLAGS)
	$(LD) util.o si7021.o -o si7021 $(LDFLAGS)
	$(LD) util.o gpio.o spi.o si4432.o -o si4432 $(LDFLAGS)

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

gpio.o: gpio.c gpio.h
	$(CC) $(CFLAGS) -c gpio.c

spi.o: spi.c spi.h
	$(CC) $(CFLAGS) -c spi.c

bmp180.o: bmp180.c
	$(CC) $(CFLAGS) -c bmp180.c

si7021.o: si7021.c
	$(CC) $(CFLAGS) -c si7021.c

si4432.o: si4432.c
	$(CC) $(CFLAGS) -c si4432.c

clean: 
	$(RM) $(RMFLAGS) bmp180 si7021 si4432 *.o
