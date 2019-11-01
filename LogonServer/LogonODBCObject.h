#pragma once
#include "ODBCObject.h"

class LogonODBCObject : public ODBCObject
{
public:
	LogonODBCObject ();
	~LogonODBCObject ();

	bool ChangeUserNickname (std::string preNickname, std::string postNikcname);
	bool CreateUser (std::string& nickname, std::string& token);
};

