#include "pch.h"
#if defined(WIN32) || defined(WIN64)
//#include <Windows.h>
#endif
#include "LogonODBCObject.h"
#include "ODBC.h"
#include "ScopedODBC.h"


LogonODBCObject::LogonODBCObject ()
{
}


LogonODBCObject::~LogonODBCObject ()
{
}

bool LogonODBCObject::ChangeUserNickname (std::string preNickname, std::string postNikcname)
{
	std::string strQuery = 
		"UPDATE userinfo SET nickname = \"" + postNikcname + "\" WHERE nickname = \"" + preNickname + "\"";

	ScopedODBC scopedODBC;
	int nRet = scopedODBC.Execute (strQuery.c_str ());
	if (false == IsSuccessQuery (nRet))
	{
		return false;
	}

	return true;
}

bool LogonODBCObject::CreateUser (std::string& nickname, std::string& token)
{
	std::string strQuery =
		"INSERT INTO userinfo(nickname, auth_token) \
			VALUES(concat(\"player\", cast(rand()*1000 as unsigned)), \
			concat(date_format(now(), '%Y%m%d%H%i%s'),\
			cast( cast( rand()*1000 as unsigned) as char) ))";

	ScopedODBC scopedODBC;
	int nRet = scopedODBC.Execute (strQuery.c_str ());
	if (false == IsSuccessQuery (nRet))
	{
		return false;
	}

	strQuery = "SELECT nickname, auth_token FROM userinfo WHERE seq = last_insert_id()";
	nRet = scopedODBC.Execute (strQuery.c_str ());
	if (false == IsSuccessQuery (nRet))
	{
		return false;
	}

	while (SQL_SUCCESS == scopedODBC->Fetch ())
	{
		int idx = 0;
		int length = 0;

		static const int MAX_NICKNAME_LEN = 12;
		static const int TOKEN_LEN = 17;
		char tmpNickname[MAX_NICKNAME_LEN + 1];
		char tmpToken[TOKEN_LEN + 1];
		scopedODBC->GetData (++idx, tmpNickname, MAX_NICKNAME_LEN + 1, &length);
		scopedODBC->GetData (++idx, tmpToken, TOKEN_LEN + 1, &length);

		nickname = tmpNickname;
		token = tmpToken;
	}

	return true;
}
