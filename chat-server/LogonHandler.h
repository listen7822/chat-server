#pragma once
#include "Handler.h"
class LogonHandler :
	public Handler
{
public:
	static bool ChangeName (Session* session, const char* pContext);
};

