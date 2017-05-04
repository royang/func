#ifndef __USBD_CLIENT_H__
#define __USBD_CLIENT_H__

struct firmupg_info{
	uint32_t  firmupg_state;
	uint32_t  firmupg_errcode;
	uint32_t  firmupg_finished;
};

int netd_init(struct ubus_context *ctx);
int netd_set_firmupg(struct ubus_context *ctx, struct firmupg_info *info);


#endif


