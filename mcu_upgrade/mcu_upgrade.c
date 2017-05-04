
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>   
#include <limits.h> 
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
                                  

#define FRAME_INDEX_MSG_ID			2

#define COM_SRC_MAX_CNT				1
#define COM_SRC_UART_0   			0

#define UART_0_RX_BUF_SIZE 			1024
#define FIMI_LINK_MAX_PACKET_LEN	40   //full frame length include header, length, checksum

#define UD_ERR_PACKAGE_SEQUENCE_AHEAD		-2
#define UD_ERR_PACKAGE_SEQUENCE_BEHIND		-3
#define UD_ERR_CODE_CHECK					-4
#define UD_ERR_CODE_ERASE					-5
#define UD_ERR_CODE_VERSION					-6
#define UD_ERR_CODE_FIRMWARE_UID			-7
#define UD_ERR_CODE_LENGTH					-8

enum _BD_CMD_
{
	MSG_ID_ARM_IT = 0x00,
	MSG_ID_DISARM_IT,
	MSG_ID_SET_4CON,
	MSG_ID_LAND_DOWN,
	MSG_ID_HEAD_FREE,
	MSG_ID_STOP_HEAD_FREE,
	MSG_ID_FLY_STATE,
	MSG_ID_IR_REV,
	MSG_ID_ACC_CALI,
	MSG_ID_SET_EEPROM,
	MSG_ID_BTKEY_A,
	MSG_ID_BTKEY_B,
	MSG_ID_BTKEY_X,
	MSG_ID_BTKEY_Y,
	
	MSG_ID_REQ_PID,
	MSG_ID_PID_PITCH,
	MSG_ID_PID_ROLL,
	MSG_ID_PID_YAW,
	MSG_ID_PID_ALT,
	
	MSG_ID_SET_VOTAGE,// 13
	MSG_ID_CP_RUN,
	MSG_ID_PRINT,
	MSG_ID_MSG_DBG,
	MSG_ID_MOTO_IR,
	MSG_ID_EP_VER,
	MSG_ID_EP_RALL,
	MSG_ID_EP_WALL,
	MSG_ID_MAV_DBG,
	MSG_ID_IR_DATA,// 1c
	MSG_ID_IR_HIT,
	MSG_ID_MOTO_CMR_SET,
	MSG_ID_MCU_POWERON,
	MSG_ID_MOTO_CNR_SET,// 20
	MSG_ID_UPDATE_START,//0x21
	MSG_ID_UPGRADE_REQUEST,//0x22
	MSG_ID_WRITE_FLASH,
	MSG_ID_TO_AP,//0x24
	MSG_ID_TO_MCU,//0x25
	MSG_ID_SONAR_TM,//0x26

	MSG_ID_UPGRADE_FRAME,//0x27
	MSG_ID_MODEM_STX,//0x28
	MSG_ID_UPGRADE_EOT,//0x29
	MSG_ID_MODEM_ACK,//0x2a
	MSG_ID_MODEM_NAK,//0x2b
	MSG_ID_MODEM_NAK1,// 0x2c
	MSG_ID_MODEM_CAN,//0x2d
	MSG_ID_MODEM_C,//0x2e
	MSG_ID_MODEM_C1,
	
	MSG_ID_LOG_FILE,
	MSG_ID_OPTICAL,
	MSG_ID_MSG_GCS_LOG,
	MSG_ID_SET_BTSTATE,
	MSG_ID_SYNC_MCUTIME,
	
	MSG_ID_IR_TIAL = 0x50,
	
	MSG_ID_OPT_CTRL = 0x53,
	
	MSG_ID_DEV_INFO = 0xc1,	
	
	MSG_ID_BD_CMD = 0xFE,
	MSG_ID_FIMI_DBG
};

enum{
	dev_info,
	request,
	firmware,
	end_of_transmit
}fw_ug_sta;

unsigned char uart_0_rx_loop_buf[UART_0_RX_BUF_SIZE] = {0};

typedef struct {
	uint8_t *buf;
	uint16_t max_buf_len;
	uint16_t index_read;
	uint16_t index_write;
	int (*cb)(uint8_t *frame, uint16_t len, unsigned char src);
}fimi_link_info_t;

fimi_link_info_t fimi_link_info[COM_SRC_MAX_CNT];

unsigned char aprom_bin_file[10240]={0};
int aprom_bin_filesize = 0;

int fd_ser, fd_bin;

int packno=0;

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}  

unsigned short misc_get_checksum(unsigned char *buf, int len)
{
    int i;
    unsigned short cs = 0;

    for (i=0;i < len;i++)
    {
        cs += buf[i];
    }
    return cs;
}

int misc_fimi_link_package(unsigned char *dest, unsigned char *src, int len)
{
    unsigned short checksum = 0;
	
	dest[0] = 0xfe;
	dest[1] = len;
	
	memcpy(dest+2, src, len);
	
	checksum = misc_get_checksum(dest+1, len+1);
	dest[len+2] = checksum & 0xff;
	dest[len+3] = (checksum >> 8) & 0xff;
	
	return len + 4;
}

int Driver_uart_0_init(int (*cb)(unsigned char *frame, unsigned short len, unsigned char src))
{
	memset(uart_0_rx_loop_buf, 0, UART_0_RX_BUF_SIZE);
	
	fimi_link_info[COM_SRC_UART_0].buf 			= uart_0_rx_loop_buf;
	fimi_link_info[COM_SRC_UART_0].max_buf_len 	= UART_0_RX_BUF_SIZE;
	fimi_link_info[COM_SRC_UART_0].index_read 	= 0;
	fimi_link_info[COM_SRC_UART_0].index_write 	= 0;
	fimi_link_info[COM_SRC_UART_0].cb 			= cb;

	printf("datalink 0 init ok\n");
	
	return 0;
}

void Driver_fimi_link_frame_parse_server()
{
	uint8_t i=0,j=0;
    
    uint8_t frame[FIMI_LINK_MAX_PACKET_LEN] = {0};
	
	for(i=0; i<COM_SRC_MAX_CNT; i++)    //traversal every communication source
	{
		uint16_t frame_len=0, index_tmp=0;
		
		if(fimi_link_info[i].index_read != fimi_link_info[i].index_write)
		{
			if(fimi_link_info[i].buf[fimi_link_info[i].index_read] == 0xfe)  //frame header vaild
			{
				if(fimi_link_info[i].index_write > fimi_link_info[i].index_read)
				{
					frame_len = fimi_link_info[i].buf[ fimi_link_info[i].index_read + 1 ];
					
					if(fimi_link_info[i].index_write - fimi_link_info[i].index_read >= frame_len + 4)
					{	
						if( (frame_len < FIMI_LINK_MAX_PACKET_LEN-4) && (frame_len > 0) ) //frame length vaild 
						{	
							index_tmp = fimi_link_info[i].index_read;
				
							for(j=0; j<frame_len+4; j++)
							{
								frame[j] = fimi_link_info[i].buf[ index_tmp++ ];
							}
							
							if( misc_get_checksum(frame+1, frame_len+1) == ( (uint16_t)frame[frame_len+2] + ((uint16_t)frame[frame_len+3]<<8) ) )
							{
								fimi_link_info[i].cb(frame, frame_len+4, i);
									
								fimi_link_info[i].index_read = index_tmp;
							}
							else
							{
								fimi_link_info[i].index_read = (fimi_link_info[i].index_read + 1 >= fimi_link_info[i].max_buf_len) ? 0 : (fimi_link_info[i].index_read + 1);
							}
						}
						else
						{
							fimi_link_info[i].index_read ++;
						}
					}	
					else
					{

					}
				}
				else
				{
					frame_len = fimi_link_info[i].buf[ (fimi_link_info[i].index_read + 1 >= fimi_link_info[i].max_buf_len) ? 0 : (fimi_link_info[i].index_read + 1) ];
					
					if(fimi_link_info[i].max_buf_len + fimi_link_info[i].index_write - fimi_link_info[i].index_read >= frame_len + 4)
					{	
						if( (frame_len < FIMI_LINK_MAX_PACKET_LEN-4) && (frame_len > 0) ) //frame length vaild 
						{
							index_tmp = fimi_link_info[i].index_read;
							
							for(j=0; j<frame_len+4; j++)
							{
								frame[j] = fimi_link_info[i].buf[index_tmp++];
								if(index_tmp >= fimi_link_info[i].max_buf_len)
								{
									index_tmp = 0;
								}
								
							}
							
							if(misc_get_checksum(frame+1, frame_len+1) == ( (uint16_t)frame[frame_len+2] + ((uint16_t)frame[frame_len+3]<<8) ) )
							{
								fimi_link_info[i].cb(frame, frame_len+4, i);
									
								fimi_link_info[i].index_read = index_tmp;
							}
							else
							{
								fimi_link_info[i].index_read = (fimi_link_info[i].index_read + 1 >= fimi_link_info[i].max_buf_len) ? 0 : (fimi_link_info[i].index_read + 1);
							}
						}
						else
						{
							fimi_link_info[i].index_read = (fimi_link_info[i].index_read + 1 >= fimi_link_info[i].max_buf_len) ? 0 : (fimi_link_info[i].index_read + 1);
						}
					}	
					else
					{
						
					}
				}	
			}
			else
			{
				fimi_link_info[i].index_read = (fimi_link_info[i].index_read + 1 >= fimi_link_info[i].max_buf_len) ? 0 : (fimi_link_info[i].index_read + 1);
			}
		}
		else
		{
			
		}
	}
}

int datalink_id_parse(unsigned char *buf, unsigned short len, unsigned char src)
{
	struct {
		uint8_t msg_id;
		uint8_t reserve;
		uint16_t hardware_version;
		uint16_t software_version;
		uint32_t uid_0;
		uint32_t uid_1;
		uint32_t uid_2;
	}dev_infos;
	
	switch(buf[FRAME_INDEX_MSG_ID])
	{
		case MSG_ID_DEV_INFO:
			if(len == sizeof(dev_infos) + 4){
				memcpy((uint8_t *)&dev_infos, buf+FRAME_INDEX_MSG_ID, sizeof(dev_infos));
				printf("\nhardware_version : %d",dev_infos.hardware_version);
				printf("\nsoftware_version : %d",dev_infos.software_version);
				printf("\nuid_0: %x",dev_infos.uid_0);
				printf("\nuid_1: %x",dev_infos.uid_1);
				printf("\nuid_2: %x\n",dev_infos.uid_2);
				fw_ug_sta = request;
			}else{
				printf("get device info error\n");
				exit(-1);
			}
			break;

		case MSG_ID_UPGRADE_REQUEST :
			switch((int8_t)buf[FRAME_INDEX_MSG_ID+2])
			{
				case 0:
					printf("upgrade request succed\n");
					fw_ug_sta = firmware;
					break;
				case -1:
					printf("upgrade request failed\n");
					exit(-1);
					break;
				case UD_ERR_CODE_VERSION:
					printf("upgrade protocol not support,upgarde exit\n");
					exit(-1);
					break;
				default:
					break;
			}
			break;
		case MSG_ID_UPGRADE_FRAME :
			switch((int8_t)buf[FRAME_INDEX_MSG_ID+4])
			{
				case 0:
					printf("upgrade frame %d send succed\n",packno);
					packno++;
					if(20*packno > aprom_bin_filesize)
					{
						packno--;
						fw_ug_sta = end_of_transmit;
					}
					break;
				case -1:
					printf("upgrade frame send failed\n");
					
					break;
				case UD_ERR_PACKAGE_SEQUENCE_BEHIND:
					printf("upgrade frame package squence behind\n");
					packno++;
					if(20*packno > aprom_bin_filesize)
					{
						packno--;
						fw_ug_sta = end_of_transmit;
					}
					break;
				case UD_ERR_PACKAGE_SEQUENCE_AHEAD:
					printf("upgrade frame package squence ahead\n");
					packno--;
					
					break;
				default:
					break;
			}
			break;
		case MSG_ID_UPGRADE_EOT :
			switch((int8_t)buf[FRAME_INDEX_MSG_ID+2])
			{
				case 0:
					printf("upgrade end transmit succed\n");
					fw_ug_sta = dev_info;
					break;
				case -1:
					printf("upgrade end transmit failed\n");
					exit(-1);
					break;
				case UD_ERR_CODE_FIRMWARE_UID:
					printf("upgrade firmware uid error\n");
					exit(-1);
					break;
				case UD_ERR_CODE_LENGTH:
					printf("upgrade firmware length error\n");
					exit(-1);
					break;
				case UD_ERR_CODE_CHECK:
					printf("upgrade firmware check error\n");
					exit(-1);
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}
	
	return 0;
}

/*********************************************************************************************************
** Function name:           openSerial
** Descriptions:            open serial port at raw mod
** input paramters:         iNum        serial port which can be value at: 1, 2, 3, 4
** output paramters:        NONE
** Return value:            file descriptor
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static int openSerial()
{
    int iFd;

    struct termios opt; 

    iFd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);                        
    if(iFd < 0) {
        perror("open serial port");
        return -1;
    }

    if(tcgetattr(iFd, &opt)!=0)
    {
    	perror("set serial port attr");
    	exit(-1);
    }

    cfsetispeed(&opt, B115200);
    cfsetospeed(&opt, B115200);

    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    opt.c_cc[VMIN]   =   20;                                      
    opt.c_cc[VTIME]  =   1;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }

    return iFd;
}

void datalink_buf_handler(int fd)
{
	unsigned char buf[100]={0};
	int i=0,len=0;

	memset(buf,0,sizeof(buf));
		
	len = read(fd, buf, 100);	
	if(len>0)
	{
		for(i=0; i<len; i++)
		{
			fimi_link_info[COM_SRC_UART_0].buf[fimi_link_info[COM_SRC_UART_0].index_write++] = buf[i];
			if( fimi_link_info[COM_SRC_UART_0].index_write >= fimi_link_info[COM_SRC_UART_0].max_buf_len)
			{
					 fimi_link_info[COM_SRC_UART_0].index_write = 0;
			}
		}
	}
}

int aprom_init()
{
	int fd=0;

	fd = open("./aprom.bin",O_RDONLY);

	if(fd < 0)
	{
		perror("open aprom.bin");
		exit(-1);
	}
	aprom_bin_filesize = get_file_size("./aprom.bin");
	if(aprom_bin_filesize % 20)
	{
		aprom_bin_filesize = ((aprom_bin_filesize / 20) + 1)*20;
		printf("aprom file length appended\n");
	}
	
	
	read(fd, aprom_bin_file, aprom_bin_filesize);

	printf("open aprom with filesize %d\n",aprom_bin_filesize);

	return fd;
}

void *mcu_upgrade_status(void *arg)
{
	unsigned char payload[30] = {0};
	unsigned char frame[34] = {0};
	int len=0;
	int i=0;
	struct {
		uint8_t msg_id;
		uint16_t packno;
		uint8_t firmware[20];
	}__attribute__((packed)) upgrade_frame;
	
	struct{
		uint8_t	 protocol_version;
		uint16_t firmware_length;
		uint16_t firmware_version;
		uint32_t firmware_checksum;
	}__attribute__((packed)) upgrade_request;

	while(1)
	{
		switch(fw_ug_sta)
		{
			case dev_info:
				payload[0] = MSG_ID_DEV_INFO;
				len = misc_fimi_link_package(frame,payload,1);
				write(fd_ser, frame, len);
				printf("send device info request\n");
				sleep(3);
				break;	
			case request:
				payload[0] = MSG_ID_UPGRADE_REQUEST;
				upgrade_request.protocol_version = 0;
				upgrade_request.firmware_length = aprom_bin_filesize;
				upgrade_request.firmware_checksum = 0;
				for(i=0; i<upgrade_request.firmware_length; i+=4)
				{
					upgrade_request.firmware_checksum += *((uint32_t *)&aprom_bin_file[i]);
				}
				upgrade_request.firmware_version = *(uint32_t *)(aprom_bin_file + 0x800);
				memcpy(payload+1, (uint8_t *)&upgrade_request, sizeof(upgrade_request));
				len = misc_fimi_link_package(frame,payload,sizeof(upgrade_request) + 1);
				packno = 0;
				write(fd_ser, frame, len);	
				printf("send upgrade request:firmware size: %u,firmware_version: %u, firmware_checksum : %u\n",upgrade_request.firmware_length, upgrade_request.firmware_version, upgrade_request.firmware_checksum);
				usleep(30000);
				break;
			case firmware:
				upgrade_frame.msg_id = MSG_ID_UPGRADE_FRAME;
				upgrade_frame.packno = packno;
				memcpy(upgrade_frame.firmware, aprom_bin_file + 20*packno, 20);
				
				len = misc_fimi_link_package(frame, (uint8_t *)&upgrade_frame, sizeof(upgrade_frame));
				write(fd_ser, frame, len);	
				printf("send firmware packet %d\n", packno);
				usleep(20000);
				break;
			case end_of_transmit:
				payload[0] = MSG_ID_UPGRADE_EOT;
				len = misc_fimi_link_package(frame,payload,1);
				write(fd_ser, frame, len);
				printf("send end of transmit \n");
				sleep(10);
				break;
			default :
				break;
		}
		
		
	}

	return 0;
}

int main(void) 
{
	int i;
	pthread_t tid;
	int rc;

	fd_ser = openSerial();
	fd_bin = aprom_init();
	
	Driver_uart_0_init(datalink_id_parse);

	rc = pthread_create(&tid, NULL, mcu_upgrade_status, (void *)0);
	if(rc){
		perror("thread create");
		exit(-1);
	}
	
	while (1) {
		datalink_buf_handler(fd_ser);
		Driver_fimi_link_frame_parse_server();		
	}
}


/*********************************************************************************************************
    end file
*********************************************************************************************************/
