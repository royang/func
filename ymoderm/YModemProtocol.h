#ifndef __YMODEMPROTOCOL_H__
#define __YMODEMPROTOCOL_H__
#include <base/UpgradeProtocol.h>
#include"../../mavlink/MavlinkConnection.h"

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)                                 //包头长度，包起始+包序号+包序号补码
#define PACKET_TRAILER          (2)                                 //包尾，两字节校验和
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)
#define PACKET_TRANSMIT_SIZE 	PACKET_1K_SIZE //WEI --> 用来指定发送单帧数据长度

#define FILE_NAME_LENGTH        (64)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define RETRY_TIMES 		(0x0A)
#define ACK_TIMEOUT             (10000000) //WEI --> 用来指定一般延时长度
#define ACK_TIMEOUT_LONG        (1000000000) //WEI --> 用来指定长时间延时长度
#define NAK_TIMEOUT             (0x10000000)
#define MAX_ERRORS              (5)

class MavlinkConnection;
class YModemProtocol : public UpgradeProtocol
{
public:
	YModemProtocol(MavlinkConnection *connection);
	~YModemProtocol();

	virtual int doUpgrade();
	int doUpgrade(const char *path);

	int ymodem_Upload(const char *file);
	int ymodem_Upload(MavlinkConnection *mavlinkConnection, const char *file);

private:
	MavlinkConnection *mavConnection;
	int setupPort(int fd, int baud, int data_bits, char event, int stop_bits, int parity, int hardware_control);
	int openPort();
	int writeBuf(int fd, char *buf, int len);
	int readChar(uint8_t *chr);
	int receive_byte(uint8_t *chr);
	int send_byte(char chr);
	int handle_after_recv_file_head();
	int handle_after_recv_packet(uint8_t *packet);
	uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);
	void Int2Str(uint8_t* str, int32_t intnum);
	int32_t Receive_Byte(uint8_t *c, uint32_t timeout);
	uint32_t Send_Byte(uint8_t c);
	int32_t Receive_Packet(uint8_t *data, int32_t *length, uint32_t timeout);
	void Ymodem_SendPacket(uint8_t *data, uint16_t length);
	int32_t Ymodem_CheckResponse(uint8_t c);
	void Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t* fileName, uint32_t *length);
	void Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk);
	uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte);
	uint16_t Cal_CRC16(const uint8_t* data, uint32_t size);
	uint8_t CalChecksum(const uint8_t* data, uint32_t size);
	uint32_t Ymodem_Transmit(uint8_t *buf, const uint8_t* sendFileName, uint32_t sizeFile);
};
#endif
