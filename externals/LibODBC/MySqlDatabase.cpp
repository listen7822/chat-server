#include "stdafx.h"
#include "MySqlDatabase.h"


MySqlDatabase::MySqlDatabase ()
{
}


MySqlDatabase::~MySqlDatabase ()
{
}

const std::string MySqlDatabase::GetConfigDBType ()
{
	return "mysql";
}

const std::string MySqlDatabase::GetConfigDBDriver ()
{
	return "MySQL ODBC 5.3 ANSI Driver";
}

const std::string MySqlDatabase::GetConfigDBIP ()
{
	return "localhost";
}

const std::string MySqlDatabase::GetConfigDBName ()
{
	return "chatserver";
}

const std::string MySqlDatabase::GetConfigDBID ()
{
	return "root";
}

const std::string MySqlDatabase::GetConfigDBPwd ()
{
	return "123!@#";
}

int MySqlDatabase::GetConfigDBPort ()
{
	return 13306;
}

int MySqlDatabase::GetConfigDBConnCnt ()
{
	return 10;
}
