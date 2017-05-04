
APP:=netlink

#PLATFORM=x86
PLATFORM=mips

ifeq ("$(PLATFORM)", "mips")
CPP=mips-linux-g++
CC=mips-linux-gcc
endif

ifeq ("$(PLATFORM)", "x86")
CPP=g++
CC=gcc
endif

LIB_PATH:=
INC_PATH:=./
CPPFLAGS := -Wall -g -O2 -D_GNU_SOURCE 

ALL_LIBS=-lpthread -lrt -ldl 
SRC:= *.c

all:
	$(CC) $(SRC) $(CPPFLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(ALL_LIBS) -o $(APP)
clean:
	rm $(APP)

