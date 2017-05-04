#ifndef __FIRMUPGD_H__
#define __FIRMUPGD_H__

#define TIMER_TICKER_MS 250

#define FIRMWARE_DEVNAME_MTD				"/dev/mtd3"
#define FIRMWARE_DEVNAME_MTD_DEFAULTS	"/dev/mtd4"
#define FIRMWARE_UBOOT_MTD				"/dev/mtd0"
#define FIRMWARE_UBOOT_MTD_ENV			"/dev/mtd1"

#define SHELL_ENV_PATH	"/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin:/etc/ath"
#define SHELL_ENV_LIB	"/lib:/usr/lib:/usr/local/lib"

/*
 * Firmware Upgrade States
 */
typedef enum {
	FIRMUPG_STATE_UNKNOWN,		/* Unknown */
	FIRMUPG_STATE_INITIALIZE,		/* Initialize for kill unused processes and clear memory */
	FIRMUPG_STATE_IDLE,			/* Idle and watiing for FTP connection */
	FIRMUPG_STATE_RECEIVING,		/* Recieving upload firmware */
	FIRMUPG_STATE_CHECKSUM,		/* Check firmware */
	FIRMUPG_STATE_ERASE,			/* Erase MTD */
	FIRMUPG_STATE_BURNING,		/* Burning MTD */
	FIRMUPG_STATE_FINISHED,		/* Finish firmware upgrading */
	FIRMUPG_STATE_RESDEF,			/* Restore to factory defaults */
	FIRMUPG_STATE_REBOOT,		/* Request to rebooting */
	FIRMUPG_STATE_FATAL,			/* FATAL Error */
} firmupg_state_t;

typedef enum {
	FIRMUPG_ERRCODE_NONE,			/* None */
	FIRMUPG_ERRCODE_UNKNOWN,		/* Unknown */
	FIRMUPG_ERRCODE_INITIALIZE,		/* Initialize for kill unused processes and clear memory */
	FIRMUPG_ERRCODE_RECEIVING,		/* Recieving upload firmware */
	FIRMUPG_ERRCODE_CHECKSUM,		/* Check firmware */
	FIRMUPG_ERRCODE_ERASE,			/* Erase MTD */
	FIRMUPG_ERRCODE_BURNING,		/* Burning MTD */
	FIRMUPG_ERRCODE_RESDEF,			/* Restore to factory defaults */
	FIRMUPG_ERRCODE_FATAL,			/* FATAL Error */
} master_firmupg_errcode_t;


typedef enum INDEX_RECVFINISH {
	INDEX_RECVFINISH_READY,
	INDEX_RECVFINISH_FILENAME,
	INDEX_RECVFINISH_LENGTH,
	INDEX_RECVFINISH_MAX
} INDEX_RECVFINISH_E;






int firmupg_add_object(struct ubus_context *ctx);
void firmupg_state_proc();


#endif

