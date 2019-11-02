#pragma once

#include <Session.h>

class LogonHandler
{
public:
	static bool __stdcall WrongCommand (boost::shared_ptr <Session> pSession, std::string param);
	static bool __stdcall ChangeNickName (boost::shared_ptr <Session> pSession, std::string param);
};

