#pragma once

#include <boost/winapi/basic_types.hpp>
#include <boost/thread/condition.hpp>

class Thread
{
public:
	Thread ();
	~Thread ();

	bool CreateThread (boost::winapi::DWORD_ waitTick);
	void DestroyThread ();
	void Run ();
	void Stop ();
	void TickTread ();
	virtual void OnProcess () = 0;

	inline boost::winapi::DWORD_ GetTickCount () { return m_TickCount; }
	bool IsRun () { return m_IsRun; }

protected:
	boost::winapi::HANDLE_ m_hThread;
	boost::winapi::HANDLE_ m_hQuitEvent;
	std::condition_variable m_QuitEvent;
	bool m_IsRun;
	boost::winapi::DWORD_  m_WaitTick;
	boost::winapi::DWORD_  m_TickCount;
};

