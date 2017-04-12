#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Packet.h"


//-----------------------------------------------------------------
// Constant
#define START_BYTE		0xFE
#define STOP_BYTE		0xFF
//-----------------------------------------------------------------
#define ACK_WPACKET_MAX_DATA		PACKET_MAX_LEN-sizeof(ACK_WRITE_PACKET_HEADER)-sizeof(PACKET_TAIL)

#define RPACKET_MAX_DATA			PACKET_MAX_LEN-sizeof(PACKET_HEADER)-sizeof(PACKET_TAIL)
#define ACK_RPACKET_MAX_DATA		PACKET_MAX_LEN-sizeof(ACK_READ_PACKET_HEADER)-sizeof(PACKET_TAIL)

////////////////////////////////////////////////////////////////////////
CPacket::CPacket(int nSize)
	: m_pPacket(NULL)
{
	Memalloc(nSize);
	m_nDataCnt=0;
}

CPacket::~CPacket()
{
	Memfree();
}

void CPacket::Memalloc(int nSize)
{
	if(!m_pPacket)
		m_pPacket = mem_block_new(nSize);
}

void CPacket::Memfree()
{
	if(m_pPacket)
		mem_block_free(m_pPacket);
	m_pPacket = NULL;
}
////////////////////////////////////////////////////////////////////////
//
// Write command
//
bool CPacket::Pack(WORD wCmdId, int nLen, PBYTE pucData)
{
    PACKET_HEADER h={START_BYTE, {(BYTE)((wCmdId>>8)&0xFF), (BYTE)(wCmdId&0xFF)}, {(BYTE)((nLen>>8)&0xFF), (BYTE)(nLen&0xFF)}};
	PACKET_TAIL t = {STOP_BYTE};


	if((wCmdId&0x8000)==0x8000)
	{
        //Wei
        if((unsigned int)nLen>RPACKET_MAX_DATA)
			return false;
	}
	else
	{
		if(nLen>WPACKET_MAX_DATA)
			return false;
	}


	memcpy(mem_block_contents(m_pPacket), (void*)&h, sizeof(PACKET_HEADER));
	m_pPacket->_used = sizeof(PACKET_HEADER);

	if(nLen>0)
	{
		memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), pucData, nLen);
		m_pPacket->_used += nLen;
	}

	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (void*)&t, sizeof(PACKET_TAIL));
	m_pPacket->_used += sizeof(PACKET_TAIL);

	return true;
}

bool CPacket::PackWriteC786(WORD wCmdId, int nLen, PBYTE pucData)
{
	//*pucData = *pucData| 0x80; // SPI write command

	//TRACE(_T("W C786: adr=%x, dat=%x\r\n"), *pucData, *(pucData+1));

	return Pack(wCmdId, nLen, pucData);
}

bool CPacket::UnpackAckWrite(PWRITE_PACKET_HEADER pHead, PPACKET_TAIL pTail)
{
	memcpy(pHead, mem_block_contents(m_pPacket), sizeof(PACKET_HEADER));

	if(pHead->ucStart!=START_BYTE)
		return false;

	int nLen = (int)BYTES2WORD(pHead->aucLen[0], pHead->aucLen[1]);
    //Wei
    if((unsigned int)nLen>ACK_WPACKET_MAX_DATA)
		return false;

	memcpy(pTail, (PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER)+nLen, sizeof(PACKET_TAIL));

	if(pTail->ucStop!=STOP_BYTE)
		return false;

	return true;
}

bool CPacket::UnpackAckWriteData(PWRITE_PACKET_HEADER pHead, PBYTE pucData)
{
	int nLen = (int)BYTES2WORD(pHead->aucLen[0], pHead->aucLen[1]);

	if(nLen==1)
	{
		*pucData = *((PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER));
		return true;
	}
    else if(nLen == 7)
    {
        *pucData = *((PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER));
        return true;
    }
	else
	{
		*pucData = 0;
		return false;
	}
}

////////////////////////////////////////////////////////////////////////
//
// Read command
//
/*bool CPacket::PackRead(WORD wCmdId, int nLen, PBYTE pucData)
{
	PACKET_HEADER h = {START_BYTE, {(wCmdId>>8)&0xFF, wCmdId&0xFF}, {(nLen>>8)&0xFF, nLen&0xFF}};
	PACKET_TAIL t={STOP_BYTE};


	if(nLen>RPACKET_MAX_DATA)
		return false;

	memcpy(mem_block_contents(m_pPacket), (void*)&h, sizeof(PACKET_HEADER));
	m_pPacket->_used = sizeof(PACKET_HEADER);

	if(nLen>0)
	{
		memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), pucData, nLen);
		m_pPacket->_used += nLen;
	}

	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (void*)&t, sizeof(PACKET_TAIL));
	m_pPacket->_used += sizeof(PACKET_TAIL);

	return true;
}*/

bool CPacket::UnpackAckRead(PREAD_PACKET_HEADER pHead, PPACKET_TAIL pTail)
{
	memcpy(pHead,mem_block_contents(m_pPacket), sizeof(PACKET_HEADER));
	if(pHead->ucStart!=START_BYTE)
		return false;

	int nLen = (int)BYTES2WORD(pHead->aucLen[0], pHead->aucLen[1]);
    //Wei
    if((unsigned int)nLen>ACK_RPACKET_MAX_DATA)
		return false;

	memcpy(pTail, (PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER)+nLen, sizeof(PACKET_TAIL));
	if(pTail->ucStop!=STOP_BYTE)
		return false;

	return true;
}

bool CPacket::UnpackAckReadData(PREAD_PACKET_HEADER pHead, PBYTE pucData)
{	
	int nLen = (int)BYTES2WORD(pHead->aucLen[0], pHead->aucLen[1]);

	if(nLen==0)
	{
		return false;
	}
	else if(nLen==1)
	{
		*pucData = *((PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER));
		return true;
	}
	else
	{
		memcpy(pucData,	(PBYTE)mem_block_contents(m_pPacket)+sizeof(PACKET_HEADER), nLen);
		return true;
	}
}
///////////////////////////////////////////////////////////
// Tammy_20130321
bool CPacket::PackHeader(WORD wCmdId, int nLen)
{
    PACKET_HEADER h = {START_BYTE, {BYTE((wCmdId>>8)&0xFF), BYTE(wCmdId&0xFF)}, {BYTE((nLen>>8)&0xFF), BYTE(nLen&0xFF)}};
	memcpy(mem_block_contents(m_pPacket), (void*)&h, sizeof(PACKET_HEADER));
	m_pPacket->_used = sizeof(PACKET_HEADER);
	m_nDataCnt=0;
	return true;
}

bool CPacket::PackData(PBYTE pucData, int nLen)
{
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), pucData, nLen);
	m_pPacket->_used += nLen;
	m_nDataCnt+=nLen;
	return true;
}

bool CPacket::PackData(unsigned int d)
{
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (LPBYTE)&d, sizeof(unsigned int));
	m_pPacket->_used += sizeof(unsigned int);
	m_nDataCnt+=sizeof(unsigned int);
	return true;
}

bool CPacket::PackData(unsigned short d)
{
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (LPBYTE)&d, 2);
	m_pPacket->_used += 2;
	m_nDataCnt+=2;
	return true;
}

bool CPacket::PackData(unsigned char d)
{
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (LPBYTE)&d, 1);
	m_pPacket->_used++;
	m_nDataCnt++;
	return true;
}

bool CPacket::PackMemReg(unsigned char adr, unsigned char dat)
{
	unsigned char mr[]={adr, dat};
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (LPBYTE)mr, 2);
	m_pPacket->_used+=2;
	m_nDataCnt+=2;
	return true;
}

bool CPacket::PackTail()
{
	PACKET_TAIL t={STOP_BYTE};
	memcpy((PBYTE)mem_block_contents(m_pPacket)+mem_block_used(m_pPacket), (void*)&t, sizeof(PACKET_TAIL));
	m_pPacket->_used += sizeof(PACKET_TAIL);
	return true;
}

bool CPacket::FillLength()
{
    BYTE data_len[] = {BYTE((m_nDataCnt>>8)&0xFF), BYTE(m_nDataCnt&0xFF)};
	memcpy(((LPBYTE)mem_block_contents(m_pPacket))+3, (LPBYTE)data_len, sizeof(data_len));
	return true;
}
