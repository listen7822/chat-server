#pragma once
#include "ODBCDatabase.h"
#include <string>

class MySqlDatabase : public ODBCDatabase
{
public:
	MySqlDatabase ();
	virtual ~MySqlDatabase ();

protected: 
	const std::string GetConfigDBType () override;
	const std::string GetConfigDBDriver () override;
	const std::string GetConfigDBIP () override;
	const std::string GetConfigDBName () override;
	const std::string GetConfigDBID () override;
	const std::string GetConfigDBPwd () override;
	const int GetConfigDBPort () override;
	const int GetConfigDBConnCnt () override;
};

