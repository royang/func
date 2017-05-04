#define LOG_TAG "main"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "netlink.h"
#include "debug.h"
#include "eloop.h"

//have to include 
#include "linux_wext.h"
#include "priv_netlink.h"
#include "includes.h"



void print_mac(u8 *bssid)
{
	ulog("mac = %02x:%02x:%02x:%02x:%02x:%02x",
			bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
}


static void process_custom(char *custom, int len)
{
	char buf[256];

	memset(buf, 0, sizeof(buf));
	sprintf(buf, custom, len);
	
	ulog("received from kernel: %s", buf);
}

static void process_wireless_event(char *data, int len)
{
	struct iw_event iwe_buf, *iwe = &iwe_buf;
	char *pos, *end, *custom, *buf;

	pos = data;
	end = data + len;

	//#define IW_EV_LCP_LEN	(sizeof(struct iw_event) - sizeof(union iwreq_data))
	while (pos + IW_EV_LCP_LEN <= end) {
		/* Event data may be unaligned, so make a local, aligned copy  before processing. */
		memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
		ulog("Wireless event: cmd=0x%x len=%d", iwe->cmd, iwe->len);
		if (iwe->len <= IW_EV_LCP_LEN){
			return;
		}

		custom = pos + IW_EV_POINT_LEN;
		//we_version = 22 in ar9342
		if ((iwe->cmd == IWEVMICHAELMICFAILURE ||
		     iwe->cmd == IWEVASSOCREQIE ||
		     iwe->cmd == IWEVCUSTOM)) {
			/* WE-19 removed the pointer from struct iw_point */
			char *dpos = (char *) &iwe_buf.u.data.length;
			int dlen = dpos - (char *) &iwe_buf;
			memcpy(dpos, pos + IW_EV_LCP_LEN, sizeof(struct iw_event) - dlen);
		} else {
			memcpy(&iwe_buf, pos, sizeof(struct iw_event));
			custom += IW_EV_POINT_OFF;
		}

		switch (iwe->cmd) {
		case IWEVEXPIRED:
			//ulog("--->IWEVEXPIRED, drv_event_disassoc");
			//drv_event_disassoc(drv->hapd, (u8 *) iwe->u.addr.sa_data);
			break;
		case IWEVREGISTERED:
			//ulog("--->IWEVREGISTERED, atheros_new_sta");
			//atheros_new_sta(drv, (u8 *) iwe->u.addr.sa_data);
			break;
		case IWEVASSOCREQIE:
			//ulog("--->IWEVASSOCREQIE");
			/* Driver hack.. Use IWEVASSOCREQIE to bypass
			 * IWEVCUSTOM size limitations. Need to handle this
			 * just like IWEVCUSTOM.
			 */
			 //this app don't care assoc request , so skip it
			 break;
		case IWEVCUSTOM:
			//ulog("--->IWEVCUSTOM");
			
			if (custom + iwe->u.data.length > end){
				return;
			}
			
			buf = malloc(iwe->u.data.length + 1);
			if (buf == NULL){
				ulog("malloc error");
				return;		/* XXX */
			}
			memcpy(buf, custom, iwe->u.data.length);
			
			buf[iwe->u.data.length] = '\0';

			if (iwe->u.data.flags != 0) {
				ulog("---->flag = %d, don't care", iwe->u.data.flags);
				//atheros_wireless_event_atheros_custom(drv, (int) iwe->u.data.flags, buf, len);
			} else {
				
				ulog("---->flag == 0");
				process_custom(buf, iwe->u.data.length);
				//atheros_wireless_event_wireless_custom(drv, buf, buf + iwe->u.data.length);
			}
			free(buf);
			break;
		}

		pos += iwe->len;
	}
}


void newlink_callback(void *ctx, struct ifinfomsg *ifi, u8 *buf, size_t len)
{
	int attrlen, rta_len;
	struct rtattr *attr;
	
	//ulog("------------------------->");
	/*if (ifi->ifi_index != drv->ifindex)
		return;*/

	attrlen = len;
	attr = (struct rtattr *) buf;

	rta_len = RTA_ALIGN(sizeof(struct rtattr));
	while (RTA_OK(attr, attrlen)) {
		if (attr->rta_type == IFLA_WIRELESS) {
			//process every wireless event
			process_wireless_event(((char *) attr) + rta_len, attr->rta_len - rta_len);
		}
		attr = RTA_NEXT(attr, attrlen);
	}
}

void dellink_callback(void *ctx, struct ifinfomsg *ifi, u8 *buf, size_t len)
{
	ulog("------------------------->");
}



int main(void)
{
	struct netlink_config *cfg;
	struct netlink_data * dat;
	
	cfg = os_zalloc(sizeof(*cfg));
	if (cfg == NULL)
		return -1;
	
	//netlink_receive(dat->sock, NULL, NULL);
	eloop_init();
	cfg->newlink_cb = newlink_callback;
	cfg->dellink_cb = dellink_callback;
	dat = netlink_init(cfg);
	eloop_run();
	ulog("never happy");

	return 0;
}

