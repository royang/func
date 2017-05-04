

CROSS = /home/roy/samba_dir/lsdk-9.5.5/build/gcc-4.3.3/build_mips/staging_dir/usr/bin/mips-linux-
CC = $(CROSS)gcc

all : time

./%.o : ./%.c	
	$(CC) -c $< -o $@

	
time : time.o
	$(CC) -o time time.o 

.PHONY : all clean

clean :
	-rm -rf time *.o 




