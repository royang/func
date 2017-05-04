#define LOG_TAG "firmupg"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <resolv.h>
#include <net/route.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>


#include "libubox/uloop.h"
#include "libubox/blobmsg_json.h"
#include "libubus.h"

#include "firmupg.h"
#include "fmlink_client.h"
#include "netd_client.h"

#include "utils.h"
#include "debug.h"

extern struct ubus_context *ctx;


char firmware_filename[256];
static int firmware_receive_start;
static int firmupg_need_uboot;
static int firmupg_need_firmware;

static int firmupg_is_erase_ready; 
static int firmupg_is_burn_ready;
static int firmupg_is_resdef_ready;

#define MTDNAME (firmupg_need_uboot ? "UBOOT mtd " FIRMWARE_UBOOT_MTD : "FIRMWARE mtd " FIRMWARE_DEVNAME_MTD)

int firmupg_state = FIRMUPG_STATE_UNKNOWN;

char * FIRMUPG_STATE_STR[] = {
	[FIRMUPG_STATE_UNKNOWN] = 	"UNKNOWN",
	[FIRMUPG_STATE_INITIALIZE] = 	"INITIALIZE",
	[FIRMUPG_STATE_IDLE] = 		"IDLE",
	[FIRMUPG_STATE_RECEIVING] =	"RECEIVING",
	[FIRMUPG_STATE_CHECKSUM] = 	"CHECKSUM",
	[FIRMUPG_STATE_ERASE] = 		"ERASE",
	[FIRMUPG_STATE_BURNING] = 	"BURNING",
	[FIRMUPG_STATE_FINISHED] = 	"FINISHED",
	[FIRMUPG_STATE_RESDEF] = 	"RESTORE_DEFAULTS",
	[FIRMUPG_STATE_REBOOT] = 	"REBOOT",
	[FIRMUPG_STATE_FATAL] = 		"FATAL"
};

int firmupg_errcode =  FIRMUPG_ERRCODE_NONE;
char *  FIRMUPG_ERRCODE_STR[] = {
	[FIRMUPG_ERRCODE_NONE] = 			"NONE",
	[FIRMUPG_ERRCODE_UNKNOWN] = 		"UNKNOWN",
	[FIRMUPG_ERRCODE_INITIALIZE] = 		"INITIALIZE",
	[FIRMUPG_ERRCODE_RECEIVING] = 		"RECEIVING",
	[FIRMUPG_ERRCODE_CHECKSUM] = 		"CHECKSUM",
	[FIRMUPG_ERRCODE_ERASE] = 			"ERASE",
	[FIRMUPG_ERRCODE_BURNING] = 		"BURNING",
	[FIRMUPG_ERRCODE_RESDEF] =			"RESTORE_DEFAULTS",
	[FIRMUPG_ERRCODE_FATAL] = 			"FATAL",
};

static void firmupg_timer_cb(struct uloop_timeout *timeout)
{
	firmupg_state_proc();
	uloop_timeout_set(timeout, TIMER_TICKER_MS);
}

static struct uloop_timeout firmupg_timer = {
	.cb = firmupg_timer_cb,
};



static int firmupg_erase_ready()
{
	if (firmupg_state ==  FIRMUPG_STATE_RESDEF) {
		LOGD("Informed Defaults MTD Erase Ready");
		firmupg_is_resdef_ready = 1;
	} else {
		LOGD("Informed Firmware MTD Erase Ready");
		firmupg_is_erase_ready = 1;
	}
	return 0;
}

static int firmupg_burn_ready()
{
	firmupg_is_burn_ready = 1;
	return 0;
}

int firmupg_receive_finish(unsigned int ready, char *filename, unsigned int length)
{
	char *c;

	if (ready == 1) {
		c = strrchr(filename, '/');
		if (c != NULL)
			c += 1;
		else
			c = filename;
		snprintf(firmware_filename, sizeof(firmware_filename) - 1, "/tmp/%s", c);
		if (firmupg_state < FIRMUPG_STATE_CHECKSUM) {
			firmupg_state = FIRMUPG_STATE_CHECKSUM;
			//remcntlsrv_broadcast_send();
		}
	}
	return 0;
}

/*
 * Inform firmware upgrading is ready, waiting for FTP connection
 */
int firmupg_inform_ready()
{
	return 0;
}


int firmupg_initialize()
{
	firmware_filename[0] = 0;
	firmupg_is_erase_ready = 0;
	firmupg_is_burn_ready = 0;
	firmupg_is_resdef_ready = 0;
	return 0;
}

static const struct blobmsg_policy method_getstate_policy[] = {};
static const struct blobmsg_policy method_recvstart_policy[] = {};
static const struct blobmsg_policy method_recvfinish_policy[] = {
		[INDEX_RECVFINISH_READY] = { .name = "ready", .type = BLOBMSG_TYPE_INT32 },
		[INDEX_RECVFINISH_FILENAME] = { .name = "name", .type = BLOBMSG_TYPE_STRING },
		[INDEX_RECVFINISH_LENGTH] = { .name = "length", .type = BLOBMSG_TYPE_INT32 },
};
static const struct blobmsg_policy method_eraseready_policy[] = {};
static const struct blobmsg_policy method_burnready_policy[] = {};



/* firmupg methods */
static int method_getstate(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	fprintf(stdout, "%s\n", FIRMUPG_STATE_STR[firmupg_state]);
	return firmupg_state;
}

static int method_recvstart(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	LOGD("Informed to START FIRMWARE UPGRADING and receiving image");

	if (firmupg_state > FIRMUPG_STATE_IDLE && firmupg_state != FIRMUPG_STATE_FATAL) {
		LOGD("Firmware upgrading was already started, MUST NOT receiving image again");
		return 0;
	}

	//master_state_next = MASTER_STATE_UPGRADE;
	firmware_receive_start = 1;
	firmupg_need_uboot = 0;
	firmupg_need_firmware = 0;
	if(firmupg_state == FIRMUPG_STATE_UNKNOWN){
		firmupg_state = FIRMUPG_STATE_INITIALIZE;
		uloop_timeout_set(&firmupg_timer, TIMER_TICKER_MS);
	}
	return 0;
}

static int method_recvfinish(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	
	struct blob_attr *tb[ARRAY_SIZE(method_recvfinish_policy)];
	uint32_t ready, length;
	char *str = NULL;

	if (firmupg_state > FIRMUPG_STATE_RECEIVING && firmupg_state != FIRMUPG_STATE_FATAL) {
		LOGD("Firmware upgrading was already started, MUST NOT receiving image again");
		return 0;
	}

	blobmsg_parse(method_recvfinish_policy, ARRAY_SIZE(method_recvfinish_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[0]) {
		LOGD("Parse firmware receive finish blobmsg failure");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[INDEX_RECVFINISH_READY])
		ready = blobmsg_get_u32(tb[INDEX_RECVFINISH_READY]);
	else {
		LOGD("No ready argument");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	if (tb[INDEX_RECVFINISH_LENGTH])
		length = blobmsg_get_u32(tb[INDEX_RECVFINISH_LENGTH]);
	else {
		LOGD("No length argument");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	str = blobmsg_get_string(tb[INDEX_RECVFINISH_FILENAME]);

	LOGD("Informed firmware receiving finished as ready=%u file=[%s] length=%u", ready, str ? : "", length);
	if (strstr(str, "u-boot")) {
		firmupg_need_uboot = 1;
	} else {
		firmupg_need_firmware = 1;
	}

	firmupg_receive_finish(ready, str, length);
	return UBUS_STATUS_OK;
}

static int method_eraseready(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	if (firmupg_state != FIRMUPG_STATE_ERASE && firmupg_state != FIRMUPG_STATE_RESDEF) {
		LOGD("Firmware upgrading was NOT at ERASE state but %s", FIRMUPG_STATE_STR[firmupg_state]);
		return 0;
	}

	firmupg_erase_ready();
	return 0;
}

static int method_burnready(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	LOGD("Informed Firmware Burn Ready");

	if (firmupg_state != FIRMUPG_STATE_BURNING) {
		LOGD("Firmware upgrading was NOT at BURNING state but %s", FIRMUPG_STATE_STR[firmupg_state]);
		return 0;
	}

	firmupg_burn_ready();
	return 0;
}

#define FIRMUPG_MD5SUM_FILE "/tmp/result_md5sum"
int checksum()
{
	int retval = -1;
	char filename_recv[256], *filename, *md5sum_recv, *md5sum_calc;
	char cmdstr[256], resultstr[512];
	FILE *fp = NULL;

	strncpy(filename_recv, firmware_filename, sizeof(filename_recv));
	md5sum_recv = filename_recv;
	if ((filename = strsep(&md5sum_recv, "_")) == NULL || md5sum_recv == NULL) {
		LOGD("Checksum failure for no md5sum");
		return -1;
	}
	strtok(md5sum_recv, ".");

	snprintf(cmdstr, sizeof(cmdstr), "md5sum %s > " FIRMUPG_MD5SUM_FILE, firmware_filename);
	system(cmdstr);
	if ((fp = fopen(FIRMUPG_MD5SUM_FILE, "r")) == NULL) {
		LOGD("Calculate MD5SUM for %s failure", firmware_filename);
		return -1;
	}

	if (fread(resultstr, 1, sizeof(resultstr), fp) == 0) {
		LOGD("Can't get MD5SUM for %s at %s", firmware_filename, FIRMUPG_MD5SUM_FILE);
		goto ret;
	}

	filename = resultstr;
	if ((md5sum_calc = strsep(&filename, " \t")) == NULL || filename == NULL) {
		LOGD("Can't find calculate md5sum from [%s]", resultstr);
		goto ret;
	}

	if (strcasecmp(md5sum_calc, md5sum_recv) != 0) {
		LOGD("Firmware md5sum error recv=[%s] calc=[%s]", md5sum_recv, md5sum_calc);
		goto ret;
	}

	retval = 0;

ret:
	if (fp)
		fclose(fp);
	unlink(FIRMUPG_MD5SUM_FILE);
	return retval;
}



static const struct ubus_method firmupg_methods[] = {
	UBUS_METHOD("getstate", method_getstate, method_getstate_policy),
	UBUS_METHOD("recvstart", method_recvstart, method_recvstart_policy),
	UBUS_METHOD("recvfinish", method_recvfinish, method_recvfinish_policy),
	UBUS_METHOD("eraseready", method_eraseready, method_eraseready_policy),
	UBUS_METHOD("burnready", method_burnready, method_burnready_policy),
};

static struct ubus_object_type firmupg_object_type = \
		UBUS_OBJECT_TYPE("master.firmware", firmupg_methods);

static struct ubus_object firmupg_object = {
	.name = "master.firmware",
	.type = &firmupg_object_type, 
	.methods = firmupg_methods,
	.n_methods = ARRAY_SIZE(firmupg_methods)
};


int mtd_erase()
{
	pid_t pid;
	char *argv[] = { "mtd" , "erase", FIRMWARE_DEVNAME_MTD, NULL, NULL, NULL, NULL, NULL, NULL };

	_evalpid(argv, NULL, 0, &pid);

	return 0;
}

int mtd_burn()
{
	pid_t pid;
	char *argv[] = { "mtd" , firmware_filename, FIRMWARE_DEVNAME_MTD, NULL, NULL, NULL, NULL, NULL, NULL };

	_evalpid(argv, NULL, 0, &pid);

	return 0;
}

int mtd_erase_uboot()
{
	pid_t pid;
	char *argv[] = { "mtd" , "erase", FIRMWARE_UBOOT_MTD, NULL, NULL, NULL, NULL, NULL, NULL };

	_evalpid(argv, NULL, 0, &pid);

	return 0;
}

int mtd_burn_uboot()
{
	pid_t pid;
	char *argv[] = { "mtd" , firmware_filename, FIRMWARE_UBOOT_MTD, NULL, NULL, NULL, NULL, NULL, NULL };

	_evalpid(argv, NULL, 0, &pid);

	return 0;
}

int restore_defaults_uboot()
{
	pid_t pid;
	char *argv[] = { "mtd" , "erase", FIRMWARE_UBOOT_MTD_ENV, NULL, NULL, NULL, NULL, NULL, NULL };

	_evalpid(argv, NULL, 0, &pid);

	return 0;
}


/*
 * Restore to factory defaults
 */
int restore_defaults_opt()
{
	char *argv[] = { NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	pid_t pid;
	int i;
	int fd;

	//signal(SIGCHLD, _reap);
	if ((pid = fork()) < 0) {
		/* error */
		LOGD("Error in forking");
		return errno;

	} else if (pid == 0) {
		/* Child */
		/* Clean up */
		setsid();
		closelog();

		/* Reset signal handlers set for parent process */
		for (i = 0; i < (_NSIG - 1); i++)
			signal(i, SIG_DFL );

		/* Clearup sockets and reset stdin/stdout/stderr */
		for(i = 3; i < 32; i ++) close(i);
		if ((fd = open("/dev/null", O_RDWR | O_NONBLOCK | O_NOCTTY)) >= 0) {
			dup2(fd, STDIN_FILENO);
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
				close(fd);
		}
		if ((fd = open("/dev/null", O_RDWR | O_NONBLOCK | O_NOCTTY)) >= 0) {
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
				close(fd);
		}

		/* Set execute environment */
		setenv("PATH", SHELL_ENV_PATH, 1);
		setenv("LD_LIBRARY_PATH", SHELL_ENV_LIB, 1);

		system("killall syslogd");
		system("sync");
		system("umount /opt");
		system("mtd erase " FIRMWARE_DEVNAME_MTD_DEFAULTS);
		
		exit(errno);
	}

	/* Parent */
	return 0;
}


/*int fmlink_power_management_start(ctx)
{
	if (ctx && hsuartd_ubus_object_id > 0)
		ubus_invoke(ctx, hsuartd_ubus_object_id, "powermgmstart", NULL, NULL, NULL, 100);
	return 0;
}

int fmlink_power_management_stop(ctx)
{
	if (ctx && hsuartd_ubus_object_id > 0)
		ubus_invoke(ctx, hsuartd_ubus_object_id, "powermgmstop", NULL, NULL, NULL, 100);
	return 0;
}*/


void remcntlsrv_broadcast_send()
{
	struct firmupg_info info;
	int FIRMUPG_STATE_PERCENT[] = {
		[FIRMUPG_STATE_UNKNOWN] = 	-1,
		[FIRMUPG_STATE_INITIALIZE] = -1,
		[FIRMUPG_STATE_IDLE] = 		-1,
		[FIRMUPG_STATE_RECEIVING] =	10,
		[FIRMUPG_STATE_CHECKSUM] = 	15,
		[FIRMUPG_STATE_ERASE] = 		45,
		[FIRMUPG_STATE_BURNING] = 	65,
		[FIRMUPG_STATE_FINISHED] = 	70,
		[FIRMUPG_STATE_RESDEF] = 	90,
		[FIRMUPG_STATE_REBOOT] = 	100,
		[FIRMUPG_STATE_FATAL] = 		-1
	};

	info.firmupg_state = firmupg_state;
	info.firmupg_errcode = firmupg_errcode;
	info.firmupg_finished = FIRMUPG_STATE_PERCENT[firmupg_state];
	netd_set_firmupg(ctx, &info);
}

/* public APIs */
int firmupg_add_object(struct ubus_context *ctx)
{
	int ret;
	if((ret = ubus_add_object(ctx, &firmupg_object))){
		LOGD("failed to add firmupgd_object");
	}
	return ret;
}

void firmupg_state_proc()
{
	static int looptimes, laststate;
	int procnext, retval;

	do {
		procnext = 0;
		if (firmupg_state != laststate) {
			looptimes = 0;
			laststate = firmupg_state;
		} else
			looptimes++;

		switch (firmupg_state) {
		case FIRMUPG_STATE_UNKNOWN:
			break;

		case FIRMUPG_STATE_INITIALIZE:
			if (firmupg_initialize() == 0) {
				firmupg_state = FIRMUPG_STATE_IDLE;
				firmupg_errcode =  FIRMUPG_ERRCODE_NONE;
				procnext = 1;
			} else {
				firmupg_state =  FIRMUPG_STATE_FATAL;
				firmupg_errcode =  FIRMUPG_ERRCODE_INITIALIZE;
				remcntlsrv_broadcast_send();
			}
			break;

		case  FIRMUPG_STATE_IDLE:
			if (firmware_receive_start == 0) {
				/* Inform firmware upgrading is ready, waiting for FTP connection */
				if (looptimes == 0) {
					LOGD("Waiting for FTP connection to start upgrading");
					firmupg_inform_ready();
					remcntlsrv_broadcast_send();
				}
			} else {
				firmupg_state = FIRMUPG_STATE_RECEIVING;
				procnext = 1;
			}
			break;

		case FIRMUPG_STATE_RECEIVING:
			if (looptimes == 0) {
				LOGD("Confirm receiving firmware image");
				remcntlsrv_broadcast_send();
				fmlink_power_management_start(ctx);
			} else if (looptimes > (1000 / TIMER_TICKER_MS) * 45) {
				LOGD("Failure to receive firmware for timeout");
				firmupg_state = FIRMUPG_STATE_IDLE;
				firmupg_errcode = FIRMUPG_ERRCODE_RECEIVING;
				firmware_receive_start = 0;
				remcntlsrv_broadcast_send();
				fmlink_power_management_stop(ctx);
			}
			break;

		case FIRMUPG_STATE_CHECKSUM:
			LOGD("Successfully receive firmware and Calculating image checksum");
			if (checksum() == 0) {
				LOGD("Firmware image checksum is OK");
				firmupg_state = FIRMUPG_STATE_ERASE;
			} else {
				LOGD("Firmware checksum verify failure");
				firmupg_state = FIRMUPG_STATE_IDLE;
				firmupg_errcode = FIRMUPG_ERRCODE_CHECKSUM;
				firmware_receive_start = 0;
				procnext = 1;
				remcntlsrv_broadcast_send();
				fmlink_power_management_stop(ctx);
				unlink(firmware_filename);
			}
			break;

		case FIRMUPG_STATE_ERASE:
			if (looptimes == 0) {
				LOGD("Start to ERASING %s", MTDNAME);
				fmlink_power_management_start(ctx);
				remcntlsrv_broadcast_send();
				firmupg_is_erase_ready = 0;
				if (firmupg_need_uboot){
					retval = mtd_erase_uboot();
				}
				else{
					retval = mtd_erase();
				}
				if (retval != 0) {
					LOGD(" Failure to launch erasing %s", MTDNAME);
					firmupg_state = FIRMUPG_STATE_FATAL;
					firmupg_errcode = FIRMUPG_ERRCODE_ERASE;
					remcntlsrv_broadcast_send();
					fmlink_power_management_stop(ctx);
					break;
				}
			}
			if (firmupg_is_erase_ready) {
				firmupg_state = FIRMUPG_STATE_BURNING;
			} else if (looptimes > (1000 / TIMER_TICKER_MS) * 45) {
				LOGD(": Erasing %s timeout", MTDNAME);
				firmupg_state = FIRMUPG_STATE_IDLE;
				firmupg_errcode = FIRMUPG_ERRCODE_ERASE;
				firmware_receive_start = 0;
				remcntlsrv_broadcast_send();
			}
			break;

		case FIRMUPG_STATE_BURNING:
			if (looptimes == 0) {
				LOGD("Start to BURNNING %s to %s", firmware_filename, MTDNAME);
				remcntlsrv_broadcast_send();
				firmupg_is_burn_ready = 0;
				if (firmupg_need_uboot)
					retval = mtd_burn_uboot();
				else
					retval = mtd_burn();
				if (retval != 0) {
					LOGD("Failure to launch burning %s", MTDNAME);
					firmupg_state =  FIRMUPG_STATE_FATAL;
					firmupg_errcode = FIRMUPG_ERRCODE_BURNING;
					remcntlsrv_broadcast_send();
					fmlink_power_management_stop(ctx);
					break;
				}
			}
			if (firmupg_is_burn_ready) {
				firmupg_state = FIRMUPG_STATE_FINISHED;
			} else if (looptimes > (1000 / TIMER_TICKER_MS) * 45) {
				LOGD("Burning %s timeout", MTDNAME);
				firmupg_state =  FIRMUPG_STATE_IDLE;
				firmupg_errcode = FIRMUPG_ERRCODE_BURNING;
				firmware_receive_start = 0;
				remcntlsrv_broadcast_send();
				fmlink_power_management_stop(ctx);
			}
			break;

		case FIRMUPG_STATE_FINISHED:
			LOGD("Finish upgrading %s as %s to %s", firmupg_need_uboot ? "UBOOT" : "FIRMWARE", firmware_filename, MTDNAME);
			remcntlsrv_broadcast_send();
			firmupg_state = FIRMUPG_STATE_RESDEF;
			break;

		case FIRMUPG_STATE_RESDEF:
			if (looptimes == 0) {
				if (firmupg_need_uboot) {
					LOGD("Restore UBOOT to factory defaults at MTD " FIRMWARE_UBOOT_MTD_ENV);
				} else{
					LOGD("Restore to factory defaults at MTD " FIRMWARE_DEVNAME_MTD_DEFAULTS);
				}
				remcntlsrv_broadcast_send();
				firmupg_is_resdef_ready = 0;

				if (firmupg_need_uboot)
					retval = restore_defaults_uboot();
				else
					retval =restore_defaults_opt();
				if (retval != 0) {
					LOGD("Failure to restore %s to factory defaults", firmupg_need_uboot ? "UBOOT" : "FIRMWARE");
					firmupg_state = FIRMUPG_STATE_IDLE;
					firmupg_errcode = FIRMUPG_ERRCODE_RESDEF;
					firmware_receive_start = 0;
					remcntlsrv_broadcast_send();
				}
			}
			if (firmupg_is_resdef_ready) {
				firmupg_state = FIRMUPG_STATE_REBOOT;
			} else if (looptimes > (1000 / TIMER_TICKER_MS) * 30) {
				LOGD("Erase MTD %s for restore to factory defaults timeout",
						firmupg_need_uboot ? "UBOOT " FIRMWARE_UBOOT_MTD_ENV : "FIRMWARE " FIRMWARE_DEVNAME_MTD_DEFAULTS);
				firmupg_state = FIRMUPG_STATE_IDLE;
				firmupg_errcode = FIRMUPG_ERRCODE_RESDEF;
				firmware_receive_start = 0;
				remcntlsrv_broadcast_send();
			}
			break;

		case FIRMUPG_STATE_REBOOT:
			if (looptimes == 0) {
				system("sync");
				system("sync");
				LOGD("Request to REBOOT");
				system("reboot");
				//master_state_next = MASTER_STATE_RESTART;
				remcntlsrv_broadcast_send();
				fmlink_power_management_stop(ctx);
			}
			break;

		case FIRMUPG_STATE_FATAL:
			LOGD("Request to REBOOT");
			system("reboot");
			//master_state_next = MASTER_STATE_RESTART;
			break;

		default:
			break;
		}
	} while (procnext);
}


