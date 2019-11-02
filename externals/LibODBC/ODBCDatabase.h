#pragma once

#if defined(WIN32) || defined(WIN64)
//#include <Windows.h>
#endif
#include <boost/winapi/basic_types.hpp>
#include <string>
#include <list>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "ODBC.h"
#include "Monitor.h"

static const std::string HEALTH_CHECK_QUERY_STRING = "select 1;";

static const int LOG_TERM = 600;

class GCritSec;

class ODBCDatabase : public Monitor
{
	std::list<ODBC*> mPool;
	Monitor m_csODBCDatabase;

public:
	ODBCDatabase ();
	~ODBCDatabase ();

	bool Initialize ();
	ODBC* GetConnection ();
	void CheckConnection ();

protected:
	// Database type(MySql,Oracle ...)
	virtual const std::string GetConfigDBType () = 0;
	virtual const std::string GetConfigDBDriver () = 0;
	virtual const std::string GetConfigDBIP () = 0;
	virtual const std::string GetConfigDBName () = 0;
	virtual const std::string GetConfigDBID () = 0;
	virtual const std::string GetConfigDBPwd () = 0;
	virtual int GetConfigDBPort () = 0;
	virtual int GetConfigDBConnCnt () = 0;
};
