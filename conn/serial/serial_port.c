#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>

#include <errno.h>

#include "serial_port.h"
#include "link_list.h"

#define USART_COM_RX_BUF_SIZE			512

#define USART_DEBUG_RX_BUF_SIZE 		USART_COM_RX_BUF_SIZE
#define LINK_MAX_LEN  					LINK_PAYLOAD_MAX_LEN+4

#define DEV_NAME "/dev/ttyUSB0"

unsigned char usart_debug_rx_buffer[USART_DEBUG_RX_BUF_SIZE]= {0};
uint16_t usart_debug_rx_buffer_write_index = 0;
uint16_t usart_debug_rx_buffer_read_index = 0;

uint8_t usart_rx_buffer[USART_DEBUG_RX_BUF_SIZE] = {0};
uint16_t usart_rx_buf_write_index = 0,usart_rx_buf_read_index = 0;

uint32_t debug_packet_frame_parse_failed_cnt = 0,debug_packet_frame_parse_succed_cnt=0;

struct Node_t *serial_list_head_p,serial_list_head;

int serial_port_open(char* dev)
{
	struct termios opt;
	int fd=0,ret = 0;

	serial_list_head_p = &serial_list_head;
	serial_list_head_p->next = NULL;

	fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd <0){
		perror(dev);
		close(fd);
		return -1;
	}else{
		printf("device %s open\n",dev);
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

uint16_t Cal_packetCheckSum(uint8_t* buf,uint16_t len)
{
	uint16_t ret=0,i=0;
	
	for(i=0;i<len;i++)
	{
		ret += buf[i];
	}
	
	return ret;
}

int8_t driver_usart_packetFrameParse(uint8_t* buf,uint16_t* index_read,uint16_t* index_write,uint8_t source)
{
	int8_t ret = 0;
	uint8_t frame_head=0;
	uint16_t i=0,len=0,index_frame_len=0;
	uint8_t tmp[LINK_PAYLOAD_MAX_LEN] = {0};

	if(*index_read != *index_write)
	{
		if(*index_read != 0)
		{
			frame_head = buf[*index_read-1];
		}
		else
		{
			frame_head = buf[USART_COM_RX_BUF_SIZE - 1];
		}

		len = buf[*index_read];

		if((frame_head == 0xfe) && (len <= LINK_PAYLOAD_MAX_LEN) && (len > 0))
		{
			if(*index_write > *index_read)
			{
				if(*index_write - *index_read >= len + 3)
				{
					index_frame_len = *index_read ;
					
					for(i=0; i<len+3; i++)//tmp copy form length byte
					{
						tmp[i] = buf[index_frame_len++];

						if(index_frame_len>=USART_COM_RX_BUF_SIZE)
						{
							index_frame_len =0;
						}
					}

					if(Cal_packetCheckSum(tmp ,len + 1) == *(uint16_t*)(tmp+len+1))
					{
						linklist_node_add(serial_list_head_p,-1,tmp+1,len);

						*index_read = index_frame_len;
					
						debug_packet_frame_parse_succed_cnt++;
						
						
						ret = 0;
					}
					else
					{
						
						debug_packet_frame_parse_failed_cnt++;

						(*index_read)++;
						if(*index_read>=USART_COM_RX_BUF_SIZE)
							*index_read = 0;

						ret = -1;
					}
				}
				else
				{

				}
			}
			else
			{
				if(USART_COM_RX_BUF_SIZE - *index_read + *index_write >= len + 3)
				{
					index_frame_len = *index_read ;

					for(i=0; i<len+3; i++)
					{
						tmp[i] = buf[index_frame_len++];

						if(index_frame_len>=USART_COM_RX_BUF_SIZE)
						{
							index_frame_len =0;
						}
					}

					if(Cal_packetCheckSum(tmp ,len + 1) == *(uint16_t*)(tmp+len+1))
					{
					   linklist_node_add(serial_list_head_p,-1,tmp+1,len);

						*index_read = index_frame_len;
						
						debug_packet_frame_parse_succed_cnt++;
			
						
						ret = 0;
					}
					else
					{
					
						debug_packet_frame_parse_failed_cnt++;

						(*index_read)++;
						if(*index_read>=USART_COM_RX_BUF_SIZE)
							*index_read = 0;

						ret = -1;
					}
				}
				else
				{
				}
			}
		}
		else
		{
			(*index_read)++;
			if(*index_read>=USART_COM_RX_BUF_SIZE)
				*index_read = 0;
		}
	}

	return ret;
}

int FMLink_SerialPort_PacketParse(uint8_t *buf_tty_read,int len_tty_read,uint8_t *buf_parsed,int len_parsed)
{
	int i=0;

	for(i=0;i<len_tty_read;i++){
		usart_rx_buffer[usart_rx_buf_write_index++] = buf_tty_read[i];
	}



	return 0;
}

void *pthread_serial(){
	int fd;
	int len = 0,i=0;
	uint8_t serial_read_buf[LINK_MAX_LEN]={0};

	fd = serial_port_open(DEV_NAME);

	while(1){
		len = read(fd,serial_read_buf,LINK_MAX_LEN);
		if(len < 0){
			if(errno != EAGAIN){
				printf("serial read error: %d\n",len);
			}
		}else if(len > 0){
			for(i=0;i<len;i++)
			{
				usart_debug_rx_buffer[usart_debug_rx_buffer_write_index++] = serial_read_buf[i];
				if(usart_debug_rx_buffer_write_index >= USART_DEBUG_RX_BUF_SIZE){
					usart_debug_rx_buffer_write_index = 0;
				}
			}
		}

		driver_usart_packetFrameParse(usart_debug_rx_buffer,&usart_debug_rx_buffer_read_index,&usart_debug_rx_buffer_write_index,0);

//		len = linklist_node_del(serial_list_head_p,0,buf);
//		if(len>0){
//			buf[len] = '\0';
//			printf("%s:%s\n",__func__,buf);
//		}
	}

	return (void*)0;
}

	










