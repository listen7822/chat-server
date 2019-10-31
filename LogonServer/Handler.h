#pragma once

#include <boost/shared_ptr.hpp>
#include "Session.h"

class Handler
{
public:
	static bool __stdcall WrongCommand (Session* pSession, std::string param);
	static bool __stdcall QuitServer (Session* pSession, std::string param);
	static bool __stdcall ChangeNickName (Session* pSession, std::string param);
};

