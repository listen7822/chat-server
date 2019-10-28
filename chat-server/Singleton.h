#pragma once
#include "Monitor.h"

template <typename T>
class Singleton : public Monitor
{
private:
	static T* instance_;
	static Monitor m_csSingleton;

public:
	static T* Instance (void)
	{
		if (nullptr == instance_)
		{
			MakeInstance ();
		}

		return instance_;
	}

	static void MakeInstance (void)
	{
		Monitor::Owner lock (m_csSingleton);
		{
			if (nullptr == instance_)
			{
				instance_ = new T;
			}
		}
	}
};

template<typename T>
T* Singleton<T>::instance_;
template<typename T>
Monitor Singleton<T>::m_csSingleton;

