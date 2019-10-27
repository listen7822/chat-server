#include "pch.h"
#include "RingBuffer.h"
#include <cstring>


RingBuffer::RingBuffer ()
{
	m_pRingBuffer = nullptr;
	m_pBeginMark = nullptr;
	m_pEndMark = nullptr;
	m_pCurrentMark = nullptr;
	m_pGettedBufferMark = nullptr;
	m_pLastMoveMark = nullptr;
	m_UsedBufferSize = 0;
	m_AllUserBufferSize = 0;
}


RingBuffer::~RingBuffer ()
{
	if (nullptr != m_pBeginMark)
	{
		delete[] m_pBeginMark;
	}
}

bool RingBuffer::Create (int bufferSize)
{
	if (nullptr != m_pBeginMark)
	{
		delete[] m_pBeginMark;
	}

	m_pBeginMark = new char[bufferSize];
	if (nullptr == m_pBeginMark)
	{
		return false;
	}

	m_pEndMark = m_pBeginMark + bufferSize - 1;
	m_BufferSize = bufferSize;

	Initialize ();
	return true;
}

bool RingBuffer::Initialize ()
{
	Monitor::Owner lock (m_csRingBuffer);
	{
		m_UsedBufferSize = 0;
		m_pCurrentMark = m_pBeginMark;
		m_pGettedBufferMark = m_pBeginMark;
		m_pLastMoveMark = m_pEndMark;
		m_AllUserBufferSize = 0;
	}

	return false;
}

char * RingBuffer::ForwardMark (int forwardLength)
{
	char* pPreCurrentMark = nullptr;
	Monitor::Owner lock (m_csRingBuffer);
	{
		if ((m_UsedBufferSize + forwardLength) > m_BufferSize)
		{
			return nullptr;
		}

		if ((m_pEndMark - m_pCurrentMark) >= forwardLength)
		{
			pPreCurrentMark = m_pCurrentMark;
			m_pCurrentMark += forwardLength;
		}
		else
		{
			m_pLastMoveMark = m_pCurrentMark;
			m_pCurrentMark = m_pBeginMark + forwardLength;
			pPreCurrentMark = m_pBeginMark;
		}

		m_UsedBufferSize += forwardLength;
		m_AllUserBufferSize += forwardLength;
	}

	return pPreCurrentMark;
}

char * RingBuffer::ForwardMark (int forwardLength, int nextLength, boost::winapi::DWORD_ remainLength)
{
	Monitor::Owner lock (m_csRingBuffer);
	{
		if ((m_UsedBufferSize + forwardLength + nextLength) > m_BufferSize)
		{
			return nullptr;
		}

		if ((m_pEndMark - m_pCurrentMark) > (nextLength + forwardLength))
		{
			m_pCurrentMark += forwardLength;
		}
		else
		{
			m_pLastMoveMark = m_pCurrentMark;
			std::memcpy (m_pBeginMark, m_pCurrentMark - (remainLength - forwardLength), remainLength);
			m_pCurrentMark = m_pBeginMark + remainLength;
		}

		m_UsedBufferSize += forwardLength;
		m_AllUserBufferSize += forwardLength;
	}

	return m_pCurrentMark;
}

void RingBuffer::ReleaseBuffer (int releaseSize)
{
	Monitor::Owner lock (m_csRingBuffer);
	{
		m_UsedBufferSize -= releaseSize;
	}
}

char * RingBuffer::GetBuffer (int readSize, int * pReadSize)
{
	char* pRet = nullptr;
	Monitor::Owner lock (m_csRingBuffer);
	{
		if (m_pLastMoveMark == m_pGettedBufferMark)
		{
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark = m_pEndMark;
		}

		if (m_UsedBufferSize > readSize)
		{
			if ((m_pLastMoveMark - m_pGettedBufferMark) >= readSize)
			{
				*pReadSize = readSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += readSize;
			}
			else
			{
				*pReadSize = (int)(m_pLastMoveMark - m_pGettedBufferMark);
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
		else if(m_UsedBufferSize > 0)
		{
			if ((m_pLastMoveMark - m_pGettedBufferMark) >= m_UsedBufferSize)
			{
				*pReadSize = m_UsedBufferSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += m_UsedBufferSize;
			}
			else
			{
				*pReadSize = (int)(m_pLastMoveMark - m_pGettedBufferMark);
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
	}

	return pRet;
}
