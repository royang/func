#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV_SEL		0xb8
#define I2C_SLAVE	0x0703
#define DATA_LEN	5

int main()
{
	int fd, ret, i, len;
	char buf[DATA_LEN];

	fd = open("/dev/i2c-1", O_RDWR);
	if(fd < 0)
	{
		perror("open device :");
		return -1;
	}

	ret = ioctl(fd, I2C_SLAVE, DEV_SEL>>1);
	if(ret < 0)
	{
		perror("ioctl :");
		return -1;
	}
	
	while(1)
	{
		buf[0] = 0;

		len = write(fd, buf, 1);
		if(len < 0)
		{
			perror("write addr :");
			return -1;
		}

		len = read(fd, buf, DATA_LEN);
		if(len < 0)
		{
			perror("read data:");
			return -1;
		}
		buf[len] = 0;
		
		printf("RH: %d.%d T: %d.%d check: %d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
		fflush(stdout);

		sleep(1);
	}

	return 0;
}
