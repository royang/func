#include <poll.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "uevent.h"

#define UEVENT_MSG_LEN  1024

static int device_fd = -1;

static void parse_event(const char *msg, struct uevent *uevent)
{
	uevent->action = "";
	uevent->path = "";
	uevent->subsystem = "";
	uevent->button= "";
	uevent->major = -1;
	uevent->minor = -1;
	uevent->partition_name = NULL;
	uevent->partition_num = -1;
	uevent->device_name = NULL;

//	printf("===>msg = %s\n", msg);

	/* currently ignoring SEQNUM */
	while(*msg) {
		printf("--->msg = %s\n", msg);
	    if(!strncmp(msg, "ACTION=", 7)) {
	        msg += 7;
	        uevent->action = msg;
	    } else if(!strncmp(msg, "BUTTON=", 7)){
	    	  msg += 7;
		  uevent->button = msg;
	    }else if(!strncmp(msg, "DEVPATH=", 8)) {
	        msg += 8;
	        uevent->path = msg;
	    } else if(!strncmp(msg, "SUBSYSTEM=", 10)) {
	        msg += 10;
	        uevent->subsystem = msg;
	    } else if(!strncmp(msg, "FIRMWARE=", 9)) {
	        msg += 9;
	        uevent->firmware = msg;
	    } else if(!strncmp(msg, "MAJOR=", 6)) {
	        msg += 6;
	        uevent->major = atoi(msg);
	    } else if(!strncmp(msg, "MINOR=", 6)) {
	        msg += 6;
	        uevent->minor = atoi(msg);
	    } else if(!strncmp(msg, "PARTN=", 6)) {
	        msg += 6;
	        uevent->partition_num = atoi(msg);
	    } else if(!strncmp(msg, "PARTNAME=", 9)) {
	        msg += 9;
	        uevent->partition_name = msg;
	    } else if(!strncmp(msg, "DEVNAME=", 8)) {
	        msg += 8;
	        uevent->device_name = msg;
	    }

	    /* advance to after the next \0 */
	    while(*msg++)
	        ;
	}

	printf("event { '%s', '%s', '%s', '%s', %d, %d }\n",
	                uevent->action, uevent->path, uevent->subsystem,
	                uevent->button, uevent->major, uevent->minor);
}


void handle_device_fd()
{
	char msg[UEVENT_MSG_LEN+2];
	int n;
	while ((n = uevent_kernel_multicast_recv(device_fd, msg, UEVENT_MSG_LEN)) > 0) {
		if(n >= UEVENT_MSG_LEN)   /* overflow -- discard */
		    continue;

		msg[n] = '\0';
		msg[n+1] = '\0';

		struct uevent uevent;
		parse_event(msg, &uevent);
	}
}

void device_init(void)
{

	/* is 64K enough? udev uses 16MB! */
	device_fd = uevent_open_socket(64*1024, true);
	if(device_fd < 0)
	    	return;

	fcntl(device_fd, F_SETFD, FD_CLOEXEC);
	fcntl(device_fd, F_SETFL, O_NONBLOCK);
}



int main(void)
{
	struct pollfd ufd;
	int nr;

	device_init();

	ufd.events = POLLIN;
	ufd.fd = device_fd;

	while(1) {
		ufd.revents = 0;
		nr = poll(&ufd, 1, -1);
		if (nr <= 0)
		    continue;
		if (ufd.revents == POLLIN)
		       handle_device_fd();
	}
	
	return 0;
}
