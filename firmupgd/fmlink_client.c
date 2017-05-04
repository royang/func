#define LOG_TAG "fmlink_client"

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
#include "utils.h"
#include "debug.h"

static uint32_t fmlink_object_id = 0;

int fmlink_init(struct ubus_context *ctx)
{
	uint32_t id;
	if (ubus_lookup_id(ctx, "fmlink", &id) == 0){
		fmlink_object_id = id;
	}
	else{
		LOGE("ubus_lookup_id fail");
	}
	return 0;
}

int fmlink_test(struct ubus_context *ctx)
{
	if (ctx && fmlink_object_id > 0)
		ubus_invoke(ctx, fmlink_object_id, "status", NULL, NULL, NULL, 100);
	return 0;
}

int fmlink_power_management_start(struct ubus_context *ctx)
{
	if (ctx && fmlink_object_id > 0)
		ubus_invoke(ctx, fmlink_object_id, "powermgmstart", NULL, NULL, NULL, 100);
	return 0;
}

int fmlink_power_management_stop(struct ubus_context *ctx)
{
	if (ctx && fmlink_object_id > 0)
		ubus_invoke(ctx, fmlink_object_id, "powermgmstop", NULL, NULL, NULL, 100);
	return 0;
}
