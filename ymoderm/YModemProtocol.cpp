#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "YModemProtocol.h"
#include "../../../include/api/pv_sdk_api_common.h"

int uart_fd = -1;
//#define PRINT_BUFFER
#define WRITE_FLASH_DELAY 300000

YModemProtocol::YModemProtocol(MavlinkConnection *connection) : mavConnection(NULL)
{
	mavConnection = connection;
}

YModemProtocol::~YModemProtocol()
{

}

int YModemProtocol::writeBuf(int fd, char *buf, int len)
{
	if(mavConnection) {
		return mavConnection->writeBuf(buf, len);
	}
	return -1;
}

int YModemProtocol::readChar(uint8_t *chr)
{
	if(mavConnection) {
		return mavConnection->readBuf((char *)chr, 1);
	}
	return -1;
}

int YModemProtocol::receive_byte(uint8_t *chr)
{
	return readChar(chr);
}

int YModemProtocol::send_byte(char chr)
{
	return writeBuf(uart_fd, &chr, 1);
}

int YModemProtocol::handle_after_recv_file_head()
{
	printf("Receive file head\n");
	return 0;
};

int YModemProtocol::handle_after_recv_packet(uint8_t *packet)
{
	printf("Receive file packet\n");
	return 0;
}

uint8_t file_name[FILE_NAME_LENGTH];

//最大接收和发送长度，超过此长度要求发送端终止传输
#define MAX_RECV_SIZE 4196000
#define MAX_SEND_SIZE 4196000
//定义自己的接收1Byte的函数,成功接收到一个Byte就返回1

#define RECEVIE_BYTE(RecvChar) receive_byte(RecvChar)
//定义自己发送1Byte的函数
#define SEND_BYTE(SendChar) send_byte(SendChar)
//定义自己接受到文件头后的操作
#define HANDLE_AFTER_RECV_FILE_HEAD() handle_after_recv_file_head()
//定义自己接受到一个数据包后的操作
#define HANDLE_AFTER_RECV_PACKET(pPacketData) handle_after_recv_packet(pPacketData)

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')
#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Convert a string to an integer
 * @param  inputstr: The string to be converted
 * @param  intnum: The intger value
 * @retval 1: Correct
 *         0: Error
 */
uint32_t YModemProtocol::Str2Int(uint8_t *inputstr, int32_t *intnum)
{
	uint32_t i = 0, res = 0;
	uint32_t val = 0;

	if(inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')) {
		if(inputstr[2] == '\0') {
			return 0;
		}
		for(i = 2; i < 11; i++) {
			if(inputstr[i] == '\0') {
				*intnum = val;
				/* return 1; */
				res = 1;
				break;
			}
			if(ISVALIDHEX(inputstr[i])) {
				val = (val << 4) + CONVERTHEX(inputstr[i]);
			} else {
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* over 8 digit hex --invalid */
		if(i >= 11) {
			res = 0;
		}
	} else { /* max 10-digit decimal input */
		for(i = 0; i < 11; i++) {
			if(inputstr[i] == '\0') {
				*intnum = val;
				/* return 1 */
				res = 1;
				break;
			} else if((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)) {
				val = val << 10;
				*intnum = val;
				res = 1;
				break;
			} else if((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)) {
				val = val << 20;
				*intnum = val;
				res = 1;
				break;
			} else if(ISVALIDDEC(inputstr[i])) {
				val = val * 10 + CONVERTDEC(inputstr[i]);
			} else {
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* Over 10 digit decimal --invalid */
		if(i >= 11) {
			res = 0;
		}
	}

	return res;
}

/**
 * @brief  Convert an Integer to a string
 * @param  str: The string
 * @param  intnum: The intger to be converted
 * @retval None
 */
void YModemProtocol::Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for(i = 0; i < 10; i++) {
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if((str[j - 1] == '0') & (Status == 0)) {
			j = 0;
		} else {
			Status++;
		}
	}
}

/**
 * @brief  Receive byte from sender
 * @param  c: Character
 * @param  timeout: Timeout
 * @retval 0: Byte received
 *         -1: Timeout
 */
int32_t YModemProtocol::Receive_Byte(uint8_t *c, uint32_t timeout)
{
	while(timeout-- > 0) {
		int val = RECEVIE_BYTE(c);
		if(val == 1) {
			return 0;
		}
	}
	return -1;
}

/**
 * @brief  Send a byte
 * @param  c: Character
 * @retval 0: Byte sent
 */
uint32_t YModemProtocol::Send_Byte(uint8_t c)
{
	SEND_BYTE(c);
	return 0;
}

/**
 * @brief  Receive a packet from sender
 * @param  data
 * @param  length
 * @param  timeout
 *     0: end of transmission
 *    -1: abort by sender
 *    >0: packet length
 * @retval 0: normally return
 *        -1: timeout or packet error
 *         1: abort by user
 */
int32_t YModemProtocol::Receive_Packet(uint8_t *data, int32_t *length, uint32_t timeout)
{
	uint16_t i, packet_size;
	uint8_t c;
	*length = 0;
	if(Receive_Byte(&c, timeout) != 0) {
		//接收超时
		return -1;
	}
	switch(c) {
		//Ymodem支持128字节和1024字节一个数据包。128字节以（ＳＯＨ）开始，１０２４字节以（ＳＴＸ）开始
		case SOH:
			packet_size = PACKET_SIZE;
			break;
		case STX:
			packet_size = PACKET_1K_SIZE;
			break;
		/*文件发送完以后，发送方发出一个“EOT”信号，接收方也以“ACK”回应。
		  然后接收方会再次发出“C”开启另一次传输，若接着发送方会发出一个“全0数据包”，
		  接收方“ACK”以后，本次通信正式结束*/
		case EOT:
			return 0;
		//传输结束以两个CA信号为标志
		case CA:
			if((Receive_Byte(&c, timeout) == 0) && (c == CA)) {
				*length = -1;
				return 0;
			} else {
				return -1;
			}
		//发送端终止传输
		case ABORT1:
		case ABORT2:
			return 1;
		default:
			return -1;
	}

	//开始接收一包数据
	*data = c;
	for(i = 1; i < (packet_size + PACKET_OVERHEAD); i ++) {
		if(Receive_Byte(data + i, timeout) != 0) {
			return -1;
		}
	}

	//看包序号与包序号反码是否相同，不相同表示该包发送错误，此处还可以加入检验校验码是否OK，保证数据传输正确
	if(data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
		return -1;
	}
	*length = packet_size;
	return 0;
}

//发送部分
/**
 * @brief  Transmit a data packet using the ymodem protocol
 * @param  data
 * @param  length
 * @retval None
 */
void YModemProtocol::Ymodem_SendPacket(uint8_t *data, uint16_t length)
{
	uint16_t i;
	i = 0;
	while(i < length) {
		Send_Byte(data[i]);
		i++;
	}
}

/**
 * @brief  check response using the ymodem protocol
 * @param  buf: Address of the first byte
 * @retval The size of the file
 */
int32_t YModemProtocol::Ymodem_CheckResponse(uint8_t c)
{
	return 0;
}

/**
 * @brief  Prepare the first block
 * @param  timeout
 *     0: end of transmission
 */
void YModemProtocol::Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t* fileName, uint32_t *length)
{
	uint16_t i, j;
	uint8_t file_ptr[FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 1];

	memset(file_ptr, 0x0, FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 1);
	/* Make first three packet */
	data[0] = SOH;
	data[1] = 0x00;
	data[2] = 0xff;

	/* Filename packet has valid data */
	for(i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH); i++) {
		data[i + PACKET_HEADER] = fileName[i];
	}

	data[i + PACKET_HEADER] = 0x00;

	Int2Str(file_ptr, *length);
	for(j = 0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ;) {
		data[i++] = file_ptr[j++];
	}

	for(j = i; j < PACKET_SIZE + PACKET_HEADER; j++) {
		data[j] = 0;
	}
}


/******************************************************************************
 * FUNCTION: Ymodem_PreparePacket ( )
 * DESCRIPTION:
 *    Input the description of function:
 * Input Parameters: 待发送数据，发送数据放置的包，包编号，待发送数据长度
 * Output Parameters:
 * Returns Value:
 *
 * Author: FuDongQiang @ 2015/07/04
 *
 * modification history
 *   ...
 ******************************************************************************/
void YModemProtocol::Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk)
{
	uint16_t i, size, packetSize;
	uint8_t* file_ptr;

	/* Make first three packet */
	//packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;

	packetSize = PACKET_TRANSMIT_SIZE;

	size = sizeBlk < packetSize ? sizeBlk : packetSize;
	if(packetSize == PACKET_1K_SIZE) {
		data[0] = STX;
	} else {
		data[0] = SOH;
	}
	data[1] = pktNo;
	data[2] = (~pktNo);
	file_ptr = SourceBuf;

	/* Filename packet has valid data */
	for(i = PACKET_HEADER; i < size + PACKET_HEADER; i++) {
		data[i] = *file_ptr++;
	}
	//不足一个包的要补足一个包
	if(size  <= packetSize) {
		for(i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++) {
			data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
		}
	}
}

/**
 * @brief  Update CRC16 for input byte
 * @param  CRC input value
 * @param  input byte
 * @retval None
 */
uint16_t YModemProtocol::UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
	uint32_t crc = crcIn;
	uint32_t in = byte | 0x100;
	do {
		crc <<= 1;
		in <<= 1;
		if(in & 0x100)
			++crc;
		if(crc & 0x10000)
			crc ^= 0x1021;
	} while(!(in & 0x10000));
	return crc & 0xffffu;
}


/**
 * @brief  Cal CRC16 for YModem Packet
 * @param  data
 * @param  length
 * @retval None
 */
uint16_t YModemProtocol::Cal_CRC16(const uint8_t* data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t* dataEnd = data + size;
	while(data < dataEnd)
		crc = UpdateCRC16(crc, *data++);

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);
	return crc & 0xffffu;
}

/**
 * @brief  Cal Check sum for YModem Packet
 * @param  data
 * @param  length
 * @retval None
 */
uint8_t YModemProtocol::CalChecksum(const uint8_t* data, uint32_t size)
{
	uint32_t sum = 0;
	const uint8_t* dataEnd = data + size;
	while(data < dataEnd)
		sum += *data++;
	return sum & 0xffu;
}

/**
 * @brief  Transmit a file using the ymodem protocol
 * @param  buf: Address of the first byte
 * @retval The size of the file
 */
uint32_t YModemProtocol::Ymodem_Transmit(uint8_t *buf, const uint8_t* sendFileName, uint32_t sizeFile)
{
	uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	uint8_t FileName[FILE_NAME_LENGTH];
	uint8_t *buf_ptr, tempCheckSum ;
	uint8_t *uploadpress;
	uint16_t tempCRC, blkNumber;
	uint8_t receivedC[2], CRC16_F = 0, i;
	uint32_t errors, ackReceived, size = 0, pktSize;

	errors = 0;
	ackReceived = 0;

#if 0 //接收起始的'C'
	printf("\n");
	int val;
	while(1) {
		do {
			val = RECEVIE_BYTE(&receivedC[0]);
			char tmp[2];
			memset(tmp, '\0', 2);
			tmp[0] = receivedC[0];
			printf("%s", tmp);
		} while (val != 1);
	}
	printf("\n");
#else
	do {
		receivedC[0] = 0;
		if(Receive_Byte(&receivedC[0], ACK_TIMEOUT) == 0) {
			if(receivedC[0] == CRC16) {
				break;
			}
		} else {
			errors++;
		}
	} while (1);//(errors < RETRY_TIMES);

	if(errors >= RETRY_TIMES) {
		return errors;
	}
#endif

	for(i = 0; i < (FILE_NAME_LENGTH - 1); i++) {
		FileName[i] = sendFileName[i];
	}
	FileName[FILE_NAME_LENGTH - 1] = '\0';
	CRC16_F = 1;


	/* Prepare first block */
	//memset(packet_data, 0x0, PACKET_1K_SIZE + PACKET_OVERHEAD);
	memset(packet_data, 0x0, PACKET_SIZE + PACKET_OVERHEAD);
	Ymodem_PrepareIntialPacket(&packet_data[0], FileName, &sizeFile);

#ifdef PRINT_BUFFER
	int p;
	for(p = 0; p < PACKET_SIZE + PACKET_OVERHEAD; p++) {
		printf("0x%02x  ", packet_data[p]);
	}
#endif

	int count = 1;
	do {
		/* Send Packet */
		Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);
#if 0
		usleep(100000);
#endif
		/* Send CRC or Check Sum based on CRC16_F */
		if(CRC16_F) {
			tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
			Send_Byte(tempCRC >> 8);
			Send_Byte(tempCRC & 0xFF);
		} else {
			tempCheckSum = CalChecksum(&packet_data[3], PACKET_SIZE);
			Send_Byte(tempCheckSum);
		}

		/* Wait for Ack and 'C' */
		if(Receive_Byte(&receivedC[0], ACK_TIMEOUT) == 0) {
			if(receivedC[0] == ACK) {
				/* Packet transfered correctly */
				ackReceived = 1;
			}
		} else {
			errors++;
		}
	} while(!ackReceived && (errors < RETRY_TIMES));

	if(errors >=  RETRY_TIMES) {
		if(g_callBack.firmwareUpgradeNotify) {
			PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
			firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
			firmwareUpgradeCallbackInfo.param = -1;
			g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
		}
		return errors;
	}

	buf_ptr = buf;
	size = sizeFile;
	blkNumber = 0x01;

	int send_count = 0;
	/* Here 1024 bytes package is used to send the packets */
	/* Resend packet if NAK  for a count of 10 else end of commuincation */
	while(size) {
		/* Prepare next packet */
		Ymodem_PreparePacket(buf_ptr, &packet_data[0], blkNumber, size);

		ackReceived = 0;
		receivedC[0] = 0;
		errors = 0;
		do {
			/* Send next packet */
			pktSize = PACKET_TRANSMIT_SIZE;
#if 1
			Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER);

			/* Send CRC or Check Sum based on CRC16_F */
			if(CRC16_F) {
				tempCRC = Cal_CRC16(&packet_data[3], pktSize);
				Send_Byte(tempCRC >> 8);
				Send_Byte(tempCRC & 0xFF);
#ifdef PRINT_BUFFER
				int p;
				for(p = 0; p < pktSize + 5; p++) {
					printf("0x%02x  ", packet_data[p]);
				}
				printf("\n");
#endif
			} else {
				tempCheckSum = CalChecksum(&packet_data[3], pktSize);
				Send_Byte(tempCheckSum);
			}
#endif

#if 1
			//非常重要延时，等待flash写入成功
			usleep(WRITE_FLASH_DELAY);
#endif

			/* Wait for Ack */
			if((Receive_Byte(&receivedC[0], ACK_TIMEOUT_LONG) == 0)  && (receivedC[0] == ACK)) {
				ackReceived = 1;
				if(size > pktSize) {
					buf_ptr += pktSize;
					size -= pktSize;
					if(blkNumber == (MAX_SEND_SIZE / PACKET_TRANSMIT_SIZE)) {
						if(g_callBack.firmwareUpgradeNotify) {
							PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
							firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
							firmwareUpgradeCallbackInfo.param = -1;
							g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
						}
						return 0xFF; /*  error */
					} else {
						blkNumber++;
					}
				} else {
					buf_ptr += pktSize;
					size = 0;
				}
			} else {
				errors++;
			}

			if(g_callBack.firmwareUpgradeNotify) {
				PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.param = ((sizeFile - size) * 100)/sizeFile;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_UPGRADING;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
			}
		} while(!ackReceived);// &&)(errors < RETRY_TIMES));

		/* Resend packet if NAK  for a count of 10 else end of commuincation */
		if(errors >=  RETRY_TIMES) {
			if(g_callBack.firmwareUpgradeNotify) {
				PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
				firmwareUpgradeCallbackInfo.param = -1;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
			}
			return errors;
		}
	}

	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;
	do {
		/* Send (EOT); */
		Send_Byte(EOT);
		/* Wait for Ack */
		if((Receive_Byte(&receivedC[0], ACK_TIMEOUT) == 0) && receivedC[0] == ACK) {
			ackReceived = 1;
		} else {
			errors++;
		}
	} while(!ackReceived && (errors < RETRY_TIMES));

	if(errors >= RETRY_TIMES) {
		if(g_callBack.firmwareUpgradeNotify) {
				PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
				firmwareUpgradeCallbackInfo.param = -1;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
		}
		return errors;
	}

	/* Last packet preparation */
	ackReceived = 0;
	receivedC[0] = 0x00;
	errors = 0;

	packet_data[0] = SOH;
	packet_data[1] = 0;
	packet_data [2] = 0xFF;

	for(i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++) {
		packet_data [i] = 0x00;
	}

	do {
		/* Send Packet */
		Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);

		/* Send CRC or Check Sum based on CRC16_F */
		tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
		Send_Byte(tempCRC >> 8);
		Send_Byte(tempCRC & 0xFF);

		/* Wait for Ack and 'C' */
		if(Receive_Byte(&receivedC[0], ACK_TIMEOUT) == 0) {
			if(receivedC[0] == ACK) {
				/* Packet transfered correctly */
				ackReceived = 1;
			}
		} else {
			errors++;
		}

	} while(!ackReceived && (errors < RETRY_TIMES));

	/* Resend packet if NAK  for a count of 10  else end of commuincation */
	if(errors >=  RETRY_TIMES) {
		if(g_callBack.firmwareUpgradeNotify) {
				PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
				firmwareUpgradeCallbackInfo.param = -1;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
		}
		return errors;
	}

	do {
		Send_Byte(EOT);
		/* Send (EOT); */
		/* Wait for Ack */
		if((Receive_Byte(&receivedC[0], ACK_TIMEOUT) == 0) && receivedC[0] == ACK) {
			ackReceived = 1;
		} else {
			errors++;
		}
	} while(!ackReceived && (errors < RETRY_TIMES));

	if(errors >= RETRY_TIMES) {
		if(g_callBack.firmwareUpgradeNotify) {
				PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_FAILED;
				firmwareUpgradeCallbackInfo.param = -1;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
		}
		return errors;
	}

	if(g_callBack.firmwareUpgradeNotify) {
			PVSDK_FIRMWAREUPGRADE_CALLBACK_INFO firmwareUpgradeCallbackInfo;
				firmwareUpgradeCallbackInfo.status = PVSDK_FIRMWAREUPGRADE_CALLBACK_STATUS_SUCCESS;
				g_callBack.firmwareUpgradeNotify(firmwareUpgradeCallbackInfo);
	}
	return 0; /* file trasmitted successfully */
}

int YModemProtocol::ymodem_Upload(MavlinkConnection *mavlinkConn, const char *file)
{

	mavConnection = mavlinkConn;
	FILE *stream = fopen(file, "r");
	//FILE *stream = fopen("../test.txt", "r");
	if(!stream) {
		return -1;
	}

	fseek(stream, 0L, SEEK_END);
	uint32_t size = ftell(stream);
	if(size <= 0) {
		fclose(stream);
		return -1;
	}
	fseek(stream, 0L, SEEK_SET);
	uint8_t *buf = (uint8_t *)malloc(size);
	long count = fread(buf, 1, size, stream);
	if(count != size) {
		free(buf);
		fclose(stream);
		return -1;
	}
	fclose(stream);
	return Ymodem_Transmit(buf, (const uint8_t *)file, size);
}

int YModemProtocol::ymodem_Upload(const char *file)
{
	FILE *stream = fopen(file, "r");
	//FILE *stream = fopen("../test.txt", "r");
	if(!stream) {
		fclose(stream);
		return -1;
	}

	fseek(stream, 0L, SEEK_END);
	uint32_t size = ftell(stream);
	if(size <= 0) {
		fclose(stream);
		return -1;
	}
	fseek(stream, 0L, SEEK_SET);
	uint8_t *buf = (uint8_t *)malloc(size);
	long count = fread(buf, 1, size, stream);
	if(count != size) {
		free(buf);
		fclose(stream);
		return -1;
	}

	fclose(stream);
	return Ymodem_Transmit(buf, (const uint8_t *)file, size);
}

int YModemProtocol::doUpgrade()
{
	return 0;
}

int YModemProtocol::doUpgrade(const char *path)
{
	return ymodem_Upload(path);
}
