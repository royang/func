OBJ = md5sum.o check.o 
EXE = check

CFLAGS += -Wall
#INC = -I./
#CROSS = /home/roy/link/lsdk/build/gcc-4.3.3/build_mips/staging_dir/usr/bin/mips-linux-uclibc-
CC = $(CROSS)gcc

all : $(EXE)

%.o:%.c
	$(CC)  $(INC) -c $(CFLAGS) $< -o $@

$(EXE) : $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIBS)


clean:
	-rm -rf *.o $(EXE)