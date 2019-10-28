#pragma once
class Session;

class Handler
{
public:
	static bool WrongCommand (Session* session, const char* pContext);
};

