#pragma once

#include <map>
#include <boost/shared_ptr.hpp>

class Session;

class PacketDispatcher
{
public:
	PacketDispatcher ();
	virtual ~PacketDispatcher ();

	enum COMMAND_TYPE {
		NONE = 0,
		WRONG_COMMAND,
		QUIT,
		CAHNGE_NICKNAME
	};
	bool DoDispatch (Session* pSession, const char* pData);

private:
	typedef bool (__stdcall *TCallback)(Session* pSession, std::string param);
	struct TFunctor
	{
		TFunctor () : callback (NULL) {}
		TFunctor (const TCallback& c)
			: callback (c) {}

		TCallback callback;
	};
	typedef std::map<COMMAND_TYPE, TFunctor> TMessageMap;
	TMessageMap m_HandlerMap;

protected:
	virtual void GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& command, const char* pData) = 0;
	void AddHandler(COMMAND_TYPE commandType, void* handler);
};

