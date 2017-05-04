export STAGING_DIR=/home/chen/ws/qcom/qca9531/qsdk/staging_dir
STAGING_DIR=/home/chen/ws/qcom/qca9531/qsdk/staging_dir

APP:=ueventd

PLATFORM=x86
#PLATFORM=mips

ifeq ("$(PLATFORM)", "mips")
CPP=$(STAGING_DIR)/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-g++
CC=$(STAGING_DIR)/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc
endif

ifeq ("$(PLATFORM)", "x86")
CPP=g++
CC=gcc
endif

LIB_PATH:=
INC_PATH:=
CPPFLAGS := -Wall -g -O2 -D_GNU_SOURCE

ALL_LIBS=-lpthread -lrt -ldl 
SRC:= *.c

all:
	$(CC) $(SRC) $(CPPFLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(ALL_LIBS) -o $(APP)
clean:
	rm $(APP)

