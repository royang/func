CROSS_COMPILE = /home/roy/opensource/openwrt-14.07/openwrt/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-

CC = $(CROSS_COMPILE)gcc
CFLAGS += -Wall

EXE = hello
OBJ = hello.o

all : $(EXE)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(EXE) : $(OBJ)
	$(CC) $(CFALGS) -o $(EXE) $(OBJ)

.PHONY : all clean

clean :
	-rm -rf $(EXE) $(OBJ) *.o
