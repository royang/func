#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "../com/wrap/Wrapsock.h"
#include "../com/wrap/Wrapunix.h"

#define MAXLINE		4096

typedef struct {
	unsigned char frame_header;
	unsigned char len;
	unsigned char msg_id;
	unsigned char target_id;
	unsigned char operarion;
	unsigned char com;
	unsigned char status;
	unsigned char check[2];
}paired_frame_t;

uint16_t CheckSum(unsigned char* buf, unsigned char len)
{
	uint16_t res=0;
	unsigned char i=0;

	for(i=0;i<len;i++)
	{
		res += buf[i];
	}

	return res;
}

void packet_package(uint8_t *buf,uint8_t len)
{
	uint16_t res=0;
	
	buf[0] = 0xfe;
	buf[1] = len;

	res = CheckSum(buf+1, len+1);
	
	buf[len+2] = (uint8_t)res;
	buf[len+3] = (uint8_t)(res>>8);
}

void echo_client(FILE *fp,int sockfd)
{
	char recvline[MAXLINE];
	paired_frame_t paired_frame;

	while(1) {
 		printf("input cmd :\n 0	: check pairing status\n 8 	: enable paired \n 9	: disable paried\n 255 : exit\n");
				
		if(Fgets(recvline, MAXLINE, fp) != NULL){
			int cmd;
			cmd = atoi(recvline);

			paired_frame.msg_id = 0x6c;
			paired_frame.target_id = 0x0b;
			paired_frame.operarion = 1;
			paired_frame.status = 0;

			switch(cmd){
				case 0 :
					paired_frame.com = 0;
					break;
				case 8 : 
					paired_frame.com = 8;
					break;
				case 9 :
					paired_frame.com = 9;
					break;
				case 255 :
					return;
					break;
				default :
					break;
			}
			
			packet_package((uint8_t *)&paired_frame, sizeof(paired_frame_t)-4);
			Writen(sockfd, (uint8_t *)&paired_frame, sizeof(paired_frame_t));
		}else{
			printf("get null string\n");
		}
	}	
}

int main(int argc, char **argv)
{
	int 				connfd;
	struct sockaddr_in 	servaddr;

	connfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port	= htons(1234);
	Inet_pton( AF_INET , "192.168.31.2" , &servaddr.sin_addr);
	
	Connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	echo_client(stdin, connfd);
	
	close(connfd);
	printf("client closed\n");
	exit(0);
}



