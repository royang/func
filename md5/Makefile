SRC = md5sum.c
OBJ = md5sum.o

CFLAGS += -Wall
LIBS = -lcrypto

#CROSS = /home/roy/link/lsdk/build/gcc-4.3.3/build_mips/staging_dir/usr/bin/mips-linux-uclibc-
CC = $(CROSS)gcc

all : md5

$(OBJ) : $(SRC)
	$(CC) -c $(CFLAGS) $(SRC) -o $(OBJ)

md5 : $(OBJ)
	$(CC) $(CFLAGS) -o md5 $(OBJ) $(LIBS)
	rm $(OBJ)

clean:
	-rm -rf *.o md5
