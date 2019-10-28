#pragma once

#include <map>
#include "ServerSession.h"

class PacketDispatcher
{
	typedef bool (__stdcall *TCallback)(Session* session, const char*);
	struct TFunctor
	{
		TFunctor () : callback (NULL), pContext (NULL) {}
		TFunctor (const TCallback& c, void*& p)
			: callback (c), pContext (p) {}

		TCallback callback;
		void* pContext;
	};
	typedef std::map<int, TFunctor>	TMessageMap;
public:
	enum COMMAND_TYPE {
		NONE = 0,
		WRONG_COMMAND,
		QUIT,
		CAHNGE_NAME
	};

	PacketDispatcher ();
	virtual ~PacketDispatcher ();

	virtual bool DoDispatch (Session* pSession, const char* pData);
	virtual COMMAND_TYPE GetCommandType (const char* pData);
	virtual void AddHandler(COMMAND_TYPE commandType, void* handler);

protected:
	std::map<COMMAND_TYPE, TFunctor> m_HandlerMap;
};

