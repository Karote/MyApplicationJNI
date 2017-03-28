#pragma once
/******************************************************************
 * Warp Box packet
 ******************************************************************/
//#include "util_memblock.h"
#include "memblock.h"
//-----------------------------------------------------------------
// Constant
//-----------------------------------------------------------------
#define WPACKET_MAX_DATA		2048
#define PACKET_MAX_LEN			WPACKET_MAX_DATA+6

//-----------------------------------------------------------------
// MACRO
//-----------------------------------------------------------------
// convert bytes to word
// h : high byte
// l : low byte
#define BYTES2WORD(h, l)			((WORD)h<<8|(WORD)l)

#define IS_WRITE_BYTE(h)			((h&0x80)==0x80)

#define IS_WRITE_COMMAND(n)			((n&0x0080)==0x0080)
//-----------------------------------------------------------------


////////////////////////////////////////////////////////////////////////
// CPacket
class CPacket:public memblock
{
public:
	CPacket(int nSize=PACKET_MAX_LEN);

	~CPacket();
public:
	// Packet Tail
	typedef struct tagPacketTail
	{BYTE ucStop;

	} PACKET_TAIL, *PPACKET_TAIL;

	// packet header
	typedef struct tagPacketHeader
	{
		BYTE ucStart;
		BYTE aucCmdId[2];
		BYTE aucLen[2]; // data length

	} PACKET_HEADER, *PWRITE_PACKET_HEADER, *PREAD_PACKET_HEADER;

	// Ack write packet header
	typedef struct tagAckWritePacketHeader
	{
		BYTE ucStart;
		BYTE aucCmdId[2];
		BYTE aucLen[2];

	} ACK_WRITE_PACKET_HEADER, *PACK_WRITE_PACKET_HEADER;

	// Ack read packet header
	typedef struct tagAckReadPacketHeader
	{
		BYTE ucStart;
		BYTE aucCmdId[2];
		BYTE aucLen[2];
	} ACK_READ_PACKET_HEADER, *PACK_READ_PACKET_HEADER;

public:
	mem_block* m_pPacket;
public:
	void Memalloc(int nSize=PACKET_MAX_LEN);
	void Memfree();

// write
public:
	// [in] nLen : the total length of data
	bool Pack(WORD wCmdId, int nLen, PBYTE pucData);

	bool PackWriteC786(WORD wCmdId, int nLen, PBYTE pucData);

	bool UnpackAckWrite(PWRITE_PACKET_HEADER pHead, PPACKET_TAIL pTail);

	bool UnpackAckWriteData(PWRITE_PACKET_HEADER pHead, PBYTE pucData);

// read
public:
	// [in] nLen : the total length of data
//	bool PackRead(WORD wCmdId, int nLen, PBYTE pucData);

	bool UnpackAckRead(PREAD_PACKET_HEADER pHead, PPACKET_TAIL pTail);

	bool UnpackAckReadData(PREAD_PACKET_HEADER pHead, PBYTE pucData);

// write/read
// Tammy_20130321
public:
	int m_nDataCnt;

	// [in] nLen : the total length of data
	bool PackHeader(WORD wCmdId, int nLen);

	bool PackData(PBYTE pucData, int nLen);
	bool PackData(unsigned int d);
	bool PackData(unsigned short d);
	bool PackData(unsigned char d);
	bool PackMemReg(unsigned char adr, unsigned char dat);

	bool PackTail();

	// fill the total length of data
	bool FillLength();


};