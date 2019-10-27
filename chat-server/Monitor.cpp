#include "pch.h"
#include "Monitor.h"


Monitor::Monitor ()
{
	// Create CriticalSection.
}


Monitor::~Monitor ()
{
	// Delete CriticalSection.
}

bool Monitor::TryEnter ()
{
	// Make Spin Lock.
	return false;
}

void Monitor::Enter ()
{
	// Enter Lock.
	m_SyncOject.lock ();
}

void Monitor::Leave ()
{
	// Leave Lock.
	m_SyncOject.unlock ();
}

Monitor::Owner::Owner (Monitor & monitor)
	: m_Monitor(monitor)
{
	m_Monitor.Enter ();
}

Monitor::Owner::~Owner ()
{
	m_Monitor.Leave ();
}
