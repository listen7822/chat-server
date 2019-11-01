#pragma once
#include "ODBCObject.h"

class ChatODBCObject : public ODBCObject
{
public:
	ChatODBCObject ();
	virtual ~ChatODBCObject ();

	bool GetUserNameUsingToken (std::string& nickname, std::string token);
};

