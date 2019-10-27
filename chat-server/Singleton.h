#pragma once
#include <list>

template <typename T>
class Singleton
{
public:
		typedef std::list<Singleton* > SINGLETON_LIST;

		Singleton ();
		virtual ~Singleton ();

		static T* Instance () {
			if(nullptr == m_Instance)
			{
				m_Instance = new T;
			}
			return m_Instance;
		}

		virtual void releaseInstance () = 0;
		static void releaseAll ();

private:
	static T* m_Instance;
	static SINGLETON_LIST m_listSigleton;

};

