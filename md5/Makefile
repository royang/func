SRC = md5sum.c
OBJ = md5sum.o

CFLAGS += -Wall
LIBS = -lcrypto

CC = gcc

all : md5

$(OBJ) : $(SRC)
	$(CC) -c $(CFLAGS) $(SRC) -o $(OBJ)

md5 : $(OBJ)
	$(CC) $(CFLAGS) -o md5 $(OBJ) $(LIBS)
	rm $(OBJ)

clean:
	-rm -rf *.o md5
