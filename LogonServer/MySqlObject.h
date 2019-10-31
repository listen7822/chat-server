#pragma once
#include "ODBCObject.h"

class MySqlObject : public ODBCObject
{
public:
	MySqlObject ();
	~MySqlObject ();

	bool GetUserToken (std::string nickname, std::string& token);
	bool ChangeUserNickname (std::string preNickname, std::string postNikcname);
	bool CreateUser (std::string& nickname, std::string& token);
};

