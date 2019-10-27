#include "pch.h"
#include "Singleton.h"

template<typename T>
Singleton<T>::Singleton ()
{
	m_listSigleton.push_back (this);
}


template<typename T>
Singleton<T>::~Singleton ()
{
	auto iter = m_listSigleton.begin ();
	while (iter != m_listSigleton.end ())
	{
		if ((*iter) == this)
		{
			break;
		}

		++iter;
	}

	m_listSigleton.erase (iter);
}

template<typename T>
void Singleton<T>::releaseAll ()
{
	auto riter = m_listSigleton.rbegin ();
	while (riter != m_listSigleton.rend ())
	{
		(*riter)->releaseInstance ();
		++riter;
	}

	m_listSigleton.clear ();
}
