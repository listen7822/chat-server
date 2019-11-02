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
	return "database-1.cj8upqmhj1ia.ap-northeast-2.rds.amazonaws.com";
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
	return "dkf2qkqk";
}

int MySqlDatabase::GetConfigDBPort ()
{
	return 13306;
}

int MySqlDatabase::GetConfigDBConnCnt ()
{
	return 10;
}
