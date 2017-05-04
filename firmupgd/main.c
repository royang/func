/*
 * Fimi Linux Center Manager Makefile
 * Copyright (c) 2015 Fimi, Inc.
 * All rights reserved.
 * Developed by chenyongqun@fimi.com
 */
#define LOG_TAG "main_firm"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <linux/types.h>
#include <syslog.h>

#include "libubox/uloop.h"
#include "libubox/blobmsg_json.h"
#include "libubus.h"
#include "firmupg.h"
#include "debug.h"


struct ubus_context *ctx = NULL;


int main(void)
{
	int rt;

	uloop_init();

	ctx = ubus_connect(NULL);
	if(!ctx){
		LOGD("ubus_connect fail");
		return -1;
	}

	ubus_add_uloop(ctx);

	rt = firmupg_add_object(ctx);
	if(rt){
		LOGD("firmupgd_add_object fail");
		goto bad;
	}
	
	fmlink_init(ctx);
	netd_init(ctx);
	
	uloop_run();

bad:
	if(ctx){
		ubus_free(ctx);
	}

	uloop_done();
	return 0;
}
