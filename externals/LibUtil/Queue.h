#pragma once
#include "Monitor.h"

template<typename T>
class Queue : public Monitor
{
public:
	Queue (int maxSize = MAX_QUEUE_SIZE);
	~Queue ();

	bool PushQueue (T typeQueueItem);
	void PopQueue ();
	bool IsEmptyQueue ();
	T GetFrontQueue ();
	int GetQueueSize ();

	int GetQueueMaxSize () { return m_QueueMaxSize; }
	void SetQueueMaxSize (int maxSize) { m_QueueMaxSize = maxSize; }
	void ClearQueue ();

private:
	T* m_arrQueue;
	int m_QueueMaxSize;
	Monitor m_csQueue;

	int m_CurentSize;
	int m_EndMark;
	int m_BeginMark;
};

