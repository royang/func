#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define PV_UPGRADE_MSG_ID_REQUEST  5

#define PV_HEADER_MEMBER_SET(x, m, v) (x.header.m = v)

struct pv_upgrade_header{
	uint8_t msg_id;
	uint8_t operation;
	uint8_t report;
	uint8_t protocol_version;
};

struct {
	struct pv_upgrade_header header;
	uint32_t firmware_length;
}pv_upgrade_request;

int main()
{
	memset(&pv_upgrade_request, 0, sizeof(pv_upgrade_request));

	printf("before set : %d\n", pv_upgrade_request.header.msg_id);
	PV_HEADER_MEMBER_SET(pv_upgrade_request, msg_id, PV_UPGRADE_MSG_ID_REQUEST);
	printf("after set : %d\n", pv_upgrade_request.header.msg_id);
	
	return 0;
}
