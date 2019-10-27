#include "pch.h"
//#include "Thread.h"
//#include "Log.h"
//#include "asio.hpp"
//#include "asio/windows/object_handle.hpp"
//
//
//Thread::Thread ()
//{
//	m_hQuitEvent = nullptr;
//	m_IsRun = false;
//	m_WaitTick = 0;
//	m_TickCount = 0;
//}
//
//
//Thread::~Thread ()
//{
//	ClaseHandle (m_hQuitEvent);
//	if (m_hThread)
//	{
//		CloseHandle (m_hThread);
//	}
//}
//
//bool Thread::CreateThread (boost::winapi::DWORD_ waitTick)
//{
//	int threadId = 0;
//	m_hThread = (HANDLE)_beginthreadex (null, 0, &CallTickThread, this, CREATE_SUSPENDED, &threadId);
//	if (nullptr == m_hThread)
//	{
//		LOG (LOG_ERROR_NORMAL, "SYSTEM | Thread::Create() | TickThread create fail: Error(%u)", GetLastError ());
//		return false;
//	}
//	m_WaitTick = waitTick;
//	return true;
//}
//
//void Thread::DestroyThread ()
//{
//	Run ();
//	SetEvent (m_hQuitEvent);
//	WiatForSingleObject (m_hThread, INFINITE);
//}
//
//void Thread::Run ()
//{
//	if (false == m_IsRun)
//	{
//		m_IsRun = true;
//		ResumeThread (m_hThread);
//	}
//}
//
//void Thread::Stop ()
//{
//	if (true == m_IsRun)
//	{
//		m_IsRun = false;
//		SuspendThread (m_hThread);
//	}
//}
//
//void Thread::TickTread ()
//{
//	while (true)
//	{
//		int ret = WaitForSingleObject (m_hQuitEvent, m_WaitTick);
//		if (WAIT_OBJECT_0 == ret)
//		{
//			break;
//		}
//		else if (WAIT_TIMEOUT == ret)
//		{
//			++m_TickCount;
//			OnProcess ();
//		}
//	}
//}
//
//int WINAPI CallTickThread (LPVOID p)
//{
//	Thread* pTickThread = (Thread*)p;
//	pTickThread->TickTread ();
//}
