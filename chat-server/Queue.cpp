#include "pch.h"
#include "Queue.h"

template<typename T>
Queue<T>::Queue (int maxSize)
{
	m_arrQueue = new T[maxSize];
	m_QueueMaxSize = maxSize;
	ClearQueue ();
}

template<typename T>
Queue<T>::~Queue ()
{
	delete[] m_arrQueue;
}

template<typename T>
bool Queue<T>::PushQueue (T typeQueueItem)
{
	Monitor::Owner lock (m_csQueue);
	{
		if (m_CurentSize >= m_QueueMaxSize)
		{
			return false;
		}

		++m_CurentSize;
		if (m_EndMark == m_QueueMaxSize)
		{
			m_EndMark = 0;
		}

		m_arrQueue[m_EndMark++] = typeQueueItem;
	}

	return true;
}

template<typename T>
void Queue<T>::PopQueue ()
{
	Monitor::Owner lock (m_csQueue);
	{
		--m_CurentSize;
		++m_BeginMark;
	}
}

template<typename T>
bool Queue<T>::IsEmptyQueue ()
{
	bool flag = false;
	Monitor::Owner lock (m_csQueue);
	{
		flag = (m_CurentSize > 0) ? false : true;
	}

	return flag;
}

template<typename T>
T Queue<T>::GetFrontQueue ()
{
	T queueItem;
	Monitor::Owner lock (m_csQueue);
	{
		if (0 >= m_CurentSize)
		{
			return nullptr;
		}

		if (m_BeginMark == m_QueueMaxSize)
		{
			m_BeginMark = 0;
		}

		queueItem = m_arrQueue[m_BeginMark];
	}

	return queueItem;
}

template<typename T>
int Queue<T>::GetQueueSize ()
{
	int size;
	Monitor::Owner lock (m_csQueue);
	{
		size = m_CurentSize;
	}

	return size;
}

template<typename T>
void Queue<T>::ClearQueue ()
{
	Monitor::Owner lock (m_csQueue);
	{
		m_CurentSize = 0;
		m_EndMark = 0;
		m_BeginMark = 0;
	}
}
