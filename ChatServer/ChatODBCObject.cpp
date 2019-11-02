#include "pch.h"
#include "ChatODBCObject.h"
#include "ODBC.h"
#include "ScopedODBC.h"


ChatODBCObject::ChatODBCObject ()
{
}


ChatODBCObject::~ChatODBCObject ()
{
}

bool ChatODBCObject::GetUserNameUsingToken (std::string& nickname, std::string token)
{
	std::string strQuery = "SELECT nickname FROM userinfo WHERE auth_token = \""+ token +"\"";
	ScopedODBC scopedODBC;
	int nRet = scopedODBC.Execute (strQuery.c_str ());
	if (false == IsSuccessQuery (nRet))
	{
		BOOST_LOG_TRIVIAL (error) \
			<< "Func: " \
			<< __FUNCTION__ \
			<< " Line: " \
			<< __LINE__ \
			<< " ErrorCode: " \
			<< scopedODBC->GetLastError ();

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
	}

	return true;
}
