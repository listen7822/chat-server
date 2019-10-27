#pragma once

#include <mutex>

class Monitor
{
public:
	class Owner {
	public:
		Owner (Monitor &monitor);
		~Owner ();

	private:
		Monitor &m_Monitor;
		Owner (const Owner &rhs);
		Owner &operator=(const Owner &rhs);
	};

	Monitor ();
	~Monitor ();

	bool TryEnter ();
	void Enter ();
	void Leave ();

private:
	std::recursive_mutex m_SyncOject;
	Monitor (const Monitor &rhs);
	Monitor &operator=(const Monitor &rhs);
};

