#pragma once

#include <Session.h>

class LogonHandler
{
public:
	static bool __stdcall WrongCommand (Session* pSession, std::string param);
	static bool __stdcall QuitServer (Session* pSession, std::string param);
	static bool __stdcall ChangeNickName (Session* pSession, std::string param);
};

