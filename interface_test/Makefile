OBJ_WARPS += \
./Wrapsock.o \
./Wrapunix.o

OBJ_CLIENT += \
./client.o   \
$(OBJ_WARPS)

CROSS = /home/roy/samba_dir/lsdk-9.5.5/build/gcc-4.3.3/build_mips/staging_dir/usr/bin/mips-linux-
#CC = $(CROSS)gcc
CC = gcc

CFLAGS += -Wall
LIBS = -lpthread -lm

ALL = client 

all : $(ALL)

%.o : /%.c	
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../com/wrap/%.c	
	$(CC) -c $(CFLAGS) $< -o $@

client : $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o client $(OBJ_CLIENT) $(LIBS)

.PHONY : all clean

clean :
	-rm -rf $(ALL) *.o 





