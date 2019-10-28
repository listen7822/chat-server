#pragma once

#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#endif
#include <string>
#include <list>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "ODBC.h"
#include "Monitor.h"
#include "Define.h"

static const std::string HEALTH_CHECK_QUERY_STRING = "select 1;";

// ���� �α� ��� �ֱ� (��)
static const int LOG_TERM = 600;

class GCritSec;

class ODBCDatabase : public Monitor
{
	std::list<ODBC*> mPool;
	Monitor m_csODBCDatabase;

public:
	ODBCDatabase ();
	~ODBCDatabase ();

	// �ν��Ͻ� ���� �� �ʱ�ȭ
	bool Initialize ();
	// ODBC ���� pool ���� �����Ǵ� ODBC �ϳ� ����
	ODBC* GetConnection ();
	// pool ���� �����Ǵ� ��� ODBC Connection üũ
	void CheckConnection ();

protected:
	// ODBC ����� ����ϴ� DB Type ��ȯ �Լ� (mysql, oracle, mssql)
	virtual const std::string GetConfigDBType () = 0;
	// ODBC ����� ����ϴ� DB Driver ��ȯ �Լ�
	virtual const std::string GetConfigDBDriver () = 0;
	// ODBC ����� ����ϴ� DB IP ��ȯ �Լ�
	virtual const std::string GetConfigDBIP () = 0;
	// ODBC ����� ����ϴ� DB Name ��ȯ �Լ�
	virtual const std::string GetConfigDBName () = 0;
	// ODBC ����� ����ϴ� DB ���� ID ��ȯ �Լ�
	virtual const std::string GetConfigDBID () = 0;
	// ODBC ����� ����ϴ� DB ���� Pwd ��ȯ �Լ�
	virtual const std::string GetConfigDBPwd () = 0;
	// ODBC ����� ����ϴ� DB ��Ʈ ��ȯ �Լ�
	virtual const int GetConfigDBPort () = 0;
	// ODBC Connection ���� ���� ��ȯ �Լ�
	virtual const int GetConfigDBConnCnt () = 0;
};
