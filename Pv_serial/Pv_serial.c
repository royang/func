#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>

#include <errno.h>

#define MAX_BUF_LEN	1024
#define DEV_NAME	"/dev/ttyUSB0"


int serial_port_open(char* dev)
{
	struct termios opt;
	int fd=0,ret = 0;

	fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd <0){
		perror(dev);
		close(fd);
		return -1;
	}else{
		printf("device %s opened\n",dev);
	}

	tcgetattr(fd,&opt);

	cfsetispeed(&opt,B115200);
	cfsetospeed(&opt,B115200);

	opt.c_lflag &= ~(ECHO | ICANON | ECHOE | ISIG);
	opt.c_cflag &= ~(CSIZE | INPCK | PARENB | CSTOPB);//no parity verify,1 bit stop
	opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	opt.c_oflag &= ~(OPOST);
	opt.c_cflag |= CLOCAL | CREAD | CS8;//ignore modem status,enable character receiver,8 bit trancever


	opt.c_cc[VTIME] = 150;
	opt.c_cc[VMIN] = 250;

	tcflush(fd,TCIFLUSH);

	ret = tcsetattr(fd,TCSANOW,&opt);
	if(ret < 0){
		perror("serial attr set");
	}else{
		printf("%s device attr configuration finished\n",dev);
	}

	return fd;
}



int main(int argv, char* argc){
	int fd;
	int len = 0,i=0;
	uint8_t serial_read_buf[MAX_BUF_LEN]={0};

	fd = serial_port_open(DEV_NAME);

	while(1){
		len = 0;
		len = read(fd, serial_read_buf, MAX_BUF_LEN);
		if(len < 0){
			if(errno != EAGAIN){
				perror("serial read ");
			}
		}else if(len > 0){
			serial_read_buf[len] = 0;
			printf("serial : %s\n", serial_read_buf);
            write(fd, serial_read_buf, len);
		}
	}

	return 0;
}

