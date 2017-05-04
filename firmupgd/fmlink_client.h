#ifndef __FMLINK_H__
#define __FMLINK_H__

int fmlink_init(struct ubus_context *ctx);
int fmlink_power_management_start(struct ubus_context *ctx);
int fmlink_power_management_stop(struct ubus_context *ctx);

int fmlink_test(struct ubus_context *ctx);

#endif

