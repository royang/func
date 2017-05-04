OBJ += ./i2c.o

CC = /home/roy/Documents/opensource/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc
CFLAGS += -Wall

all: i2c

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

i2c : $(OBJ)
	$(CC) $(CFLAGS) -o i2c $(OBJ)

.PHONY : all clean

clean : 
	-rm -rf i2c *.o
