#pragma once

#include "Monitor.h"
#include "Define.h"
#include <boost/winapi/basic_types.hpp>

class RingBuffer : Monitor
{
public:
	RingBuffer ();
	~RingBuffer ();

	// Assign memory.
	bool Create (int bufferSize = MAX_RINGBUFFSIZE);
	bool Initialize ();
	
	inline int GetBufferSize () { return m_BufferSize; }

	inline char* GetBeginMark () { return m_pBeginMark; }
	inline char* GetCurrentMark () { return m_pCurrentMark; }
	inline char* GetEndMark () { return m_pEndMark; }

	char* ForwardMark (int forwardLength);
	char* ForwardMark (int forwardLength, int nextLength, boost::winapi::DWORD_ remainLength);

	void ReleaseBuffer (int releaseSize);
	int GetUsedBufferSize () { return m_UsedBufferSize; }
	int GetAllUsedBufferSize () { return m_AllUserBufferSize; }

	char* GetBuffer (int readSize, int* pReadSize);

private:
	char* m_pRingBuffer;
	
	char* m_pBeginMark;
	char* m_pEndMark;
	char* m_pCurrentMark;
	char* m_pGettedBufferMark;
	char* m_pLastMoveMark;

	int m_BufferSize;
	int m_UsedBufferSize;
	unsigned int m_AllUserBufferSize;

	Monitor m_csRingBuffer;

	RingBuffer (const RingBuffer &rhs);
	RingBuffer &operator=(const RingBuffer &rhs);

};

