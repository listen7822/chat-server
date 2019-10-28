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

// 부하 로그 출력 주기 (초)
static const int LOG_TERM = 600;

class GCritSec;

class ODBCDatabase : public Monitor
{
	std::list<ODBC*> mPool;
	Monitor m_csODBCDatabase;

public:
	ODBCDatabase ();
	~ODBCDatabase ();

	// 인스턴스 생성 후 초기화
	bool Initialize ();
	// ODBC 사용시 pool 에서 관리되는 ODBC 하나 추출
	ODBC* GetConnection ();
	// pool 에서 관리되는 모든 ODBC Connection 체크
	void CheckConnection ();

protected:
	// ODBC 연결시 사용하는 DB Type 반환 함수 (mysql, oracle, mssql)
	virtual const std::string GetConfigDBType () = 0;
	// ODBC 연결시 사용하는 DB Driver 반환 함수
	virtual const std::string GetConfigDBDriver () = 0;
	// ODBC 연결시 사용하는 DB IP 반환 함수
	virtual const std::string GetConfigDBIP () = 0;
	// ODBC 연결시 사용하는 DB Name 반환 함수
	virtual const std::string GetConfigDBName () = 0;
	// ODBC 연결시 사용하는 DB 계정 ID 반환 함수
	virtual const std::string GetConfigDBID () = 0;
	// ODBC 연결시 사용하는 DB 계정 Pwd 반환 함수
	virtual const std::string GetConfigDBPwd () = 0;
	// ODBC 연결시 사용하는 DB 포트 반환 함수
	virtual const int GetConfigDBPort () = 0;
	// ODBC Connection 생성 개수 반환 함수
	virtual const int GetConfigDBConnCnt () = 0;
};
