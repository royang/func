#define LOG_TAG "udbd_client"

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
#include "netd_client.h"

static uint32_t netd_object_id = 0;

int netd_init(struct ubus_context *ctx)
{
	uint32_t id;
	if (ubus_lookup_id(ctx, "netd", &id) == 0){
		netd_object_id = id;
	}
	else{
		LOGE("ubus_lookup_id fail");
	}
	return 0;
}

int netd_set_firmupg(struct ubus_context *ctx, struct firmupg_info *info)
{
	struct blob_buf b;

	if (ctx && netd_object_id > 0){
		blob_buf_init(&b, 0);
		blobmsg_add_u32(&b, "firmupg_state", info->firmupg_state);
		blobmsg_add_u32(&b, "firmupg_errcode", info->firmupg_errcode);
		blobmsg_add_u32(&b, "firmupg_finished", info->firmupg_finished);
		ubus_invoke(ctx, netd_object_id, "set_firmupg", b.head, NULL, 0, 3000);
	}
	return 0;
}



