#pragma once
#include <Session.h>

class ChatHandler
{
public:
	static bool __stdcall WrongCommand (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall AuthToken (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall GetRoomList (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall JoinRoomList (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall ExitRoom (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall BroadcastMessage (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall SendMessage (boost::shared_ptr <Session> pSession, std::string param);
};

