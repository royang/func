#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define UART_0_RX_BUF_SIZE 256

typedef struct{
	uint8_t *buf;
	uint16_t max_buf_len;
	uint16_t index_read;
	uint16_t index_write;
	int (*cb)(uint8_t *frame, uint16_t len);
	uint8_t source;
}fimi_link_loop_buf_info_t;

unsigned char uart_0_rx_loop_buf[UART_0_RX_BUF_SIZE]  = {0};
volatile unsigned int uart_0_rx_loop_buf_read_index  = 0;
volatile unsigned int uart_0_rx_loop_buf_write_index  = 0;

fimi_link_loop_buf_info_t uart_0_


int fimi_link_uart_0_init()
{
	
}



int driver_fimi_link_frame_parse()
{
	int8_t ret = 0;
	uint8_t frame_head=0;
    uint16_t i=0,len=0,index_frame_len=0;
    uint8_t tmp[FIMI_LINK_MAX_PACKET_LEN] = {0};

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

        if((frame_head == 0xfe) && (len <= FIMI_LINK_MAX_PACKET_LEN) && (len > 0))
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
                        driver_usart_payload_put_in_buf(tmp+1,len);

                        *index_read = index_frame_len;
#ifdef DEBUG_INFO_VALID						
                        debug_packet_frame_parse_succed_cnt++;
#endif						
						
						ret = 0;
                    }
                    else
                    {
#ifdef DEBUG_INFO_VALID						
                        debug_packet_frame_parse_failed_cnt++;
#endif
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
                       driver_usart_payload_put_in_buf(tmp+1,len);

                        *index_read = index_frame_len;
						
#ifdef DEBUG_INFO_VALID
                        debug_packet_frame_parse_succed_cnt++;
#endif						
						
						ret = 0;
                    }
                    else
                    {
#ifdef DEBUG_INFO_VALID						
                        debug_packet_frame_parse_failed_cnt++;
#endif

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

