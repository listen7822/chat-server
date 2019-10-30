#include "pch.h"
#include "MySqlObject.h"
#include "ODBC.h"
#include "ScopedODBC.h"


MySqlObject::MySqlObject ()
{
}


MySqlObject::~MySqlObject ()
{
}

std::string MySqlObject::GetUserToken ()
{
	std::string strQuery = "SELECT auth_token FROM userinfo WHERE user_name = \"yun\"";
	ScopedODBC scopedODBC;
	int nRet = scopedODBC.Execute (strQuery.c_str ());
	if (false == IsSuccessQuery (nRet))
	{
		return "";
	}

	return "";
}
