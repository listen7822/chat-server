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
		CAHNGE_NAME
	};
	bool DoDispatch (int sessionId, const char* pData);

private:
	typedef bool (__stdcall *TCallback)(int sessionId, const char* pData);
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
	virtual COMMAND_TYPE GetCommandType (const char* pData);
	void AddHandler(COMMAND_TYPE commandType, void* handler);
};

