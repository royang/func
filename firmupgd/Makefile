#
# Fimi Linux Center Manager Master Makefile
# Copyright (c) 2015 Fimi, Inc.
# All rights reserved.
# Developed by Richard Lee <liqilinux@qq.com>
#

ifndef INSTALL_ROOT
	INSTALL_ROOT=/usr/local
endif

# Center Manager as master
MASTER_CFLAGS = $(TARGET_CFLAGS) -I. -I$(OPENWRT_DIR)/shares -I$(INSTALL_ROOT)/usr/include -I$(INSTALL_ROOT)/include -DCROSS_COMPILE=$(CROSS_COMPILE)
MASTER_LDFLAGS = $(TARGET_LDFLAGS) -L. -L$(OPENWRT_DIR)/shares -L$(INSTALL_ROOT)/usr/lib -L$(INSTALL_ROOT)/lib -lubox -lblobmsg_json -lubus -ljson-c -luci -liw
FIRMUPGD_OBJS = main.o firmupg.o utils.o fmlink_client.o netd_client.o

DEPDIR = .deps

all: $(DEPDIR) firmupgd

-include $(DEPDIR)/*

$(DEPDIR):
	@mkdir -p $(DEPDIR)

firmupgd: $(FIRMUPGD_OBJS)
	$(CROSS_COMPILE)gcc $(MASTER_CFLAGS) -o $@ $^ $(MASTER_LDFLAGS)

clean:
	rm -f *.o *.a *~
	@rm -rf $(DEPDIR)
	rm -f firmupgd
	
install:
	install -D -m 755 firmupgd $(OPENWRT_DIR)/rootfs-openwrt_mi124_dr/defaults/bin
	install -D -m 755 firmupgd $(OPENWRT_DIR)/rootfs-openwrt_mi124_rc/defaults/bin
#	install -D -m 755 dhcpc_usbnet.script $(INSTALL_ROOT)/usr/local/sbin

%.o: %.c
	depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(CROSS_COMPILE)gcc $(MASTER_CFLAGS) -MT $@ -MD -MP -MF $$depbase.d -c -o $@ $<

.PHONY: all clean install
