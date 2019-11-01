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
		AUTH_TOKEN,
		CAHNGE_NICKNAME,
		ROOM_LIST,
		JOIN_ROOM,
		EXIT_ROOM,
		SEND_MESSAGE,
		BROADCASE
	};
	bool DoDispatch (boost::shared_ptr < Session> pSession, const char* pData);

private:
	typedef bool (__stdcall *TCallback)(boost::shared_ptr <Session> pSession, std::string param);
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
	virtual void GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& param, const char* pData) = 0;
	void AddHandler(COMMAND_TYPE commandType, void* handler);
};

