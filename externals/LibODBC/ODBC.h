#pragma once
#if defined(WIN32) || defined(WIN64)
//#include <Windows.h>
#endif
#include <boost/winapi/basic_types.hpp>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

static const char* SQL_MSG_SUCCESS = "Success.";
static const char* SQL_MSG_NO_DATA = "No Data";

// SQL DB 타입
static const char* SQL_DB_TYPE_MS_SQL = "mssql";
static const char* SQL_DB_TYPE_ORACLE = "oracle";
static const char* SQL_DB_TYPE_MY_SQL = "mysql";

// SQL 상태 메세지
static const char* SQL_STATE_MS_SQL_08S01 = "08S01";

class ODBC
{
	// ODBC 환경 핸들
	SQLHENV m_hEnv;
	// ODBC 연결 핸들
	SQLHDBC m_hDBC;
	// ODBC 명령 핸들
	SQLHSTMT m_hStmt;
	// ODBC 사용중인 위치
	std::string m_StrCaller;

	// ODBC 모든 리소스 해제
	void FreeConnection ();
	// 입력받은 에러코드가 DB 연결이 끊어졌는지 판단
	bool IsDisconnect (const char *pSqlState, int NativeCode);
	// 에러를 진단하고 연결이 끊어지면 재연결하고 쿼리를 다시 수행
	void CheckError (int retSQL, const char *pExecute = nullptr);
	// 에러만 진단
	void ExtractError (char *fn, SQLHANDLE handle, SQLSMALLINT type);

protected:
	// DB 사용 여부
	bool m_bUsing;

	// DB 타입 (oracle, mysql, mssql)
	char m_szDBType[SQL_MAX_MESSAGE_LENGTH];
	// DB 드라이버 버전
	char m_szDriver[SQL_MAX_MESSAGE_LENGTH];
	// DB 이름
	char m_szDBName[SQL_MAX_MESSAGE_LENGTH];
	// Database Source Name
	char m_szDSN[SQL_MAX_MESSAGE_LENGTH];

	// DB 접속 계정 정보
	char m_szID[SQL_MAX_MESSAGE_LENGTH];
	char m_szPW[SQL_MAX_MESSAGE_LENGTH];

	// DB 접속 경로
	char m_szIP[SQL_MAX_MESSAGE_LENGTH];
	int m_nPort;

	// db 처리 last error
	char m_szErrorMessage[SQL_MAX_MESSAGE_LENGTH];

public:
	ODBC (void);
	~ODBC (void);

	// DB 사용중인지 체크
	bool IsOnUsing ();
	// DB 사용상태 저장
	void SetOnUsing (bool bUsing);

	// DB 연결 시도
	int Connect (const char *pDBType, const char *pDriver, const char *pIP, int nPort, const char *pDSN, const char *pDBName, const char *pID, const char *pPW);
	// DB 연결상태 체크 및 재연결 요청
	int CheckConnection ();

	// 쿼리 요청
	int Query (const char *pQuery);

	// DB 쿼리 결과 집합의 다음행으로 커서 진행
	int Fetch ();
	// Fetch 로 진행한 쿼리 닫기
	int CloseCursor ();
	// DB 쿼리 결과 집합의 로우 개수 가져온다.
	int GetNumOfResultRows ();
	// DB 쿼리 결과 집합의 컬럼 개수 가져온다.
	int GetNumOfResultCols ();

	// DB 쿼리 결과 집합의 지정한 컬럼을 타입에 맞는 변수에 복사한다.
	int GetData (int nColNum, bool &nResult);
	int GetData (int nColNum, short &nResult);
	int GetData (int nColNum, int &nResult);
	int GetData (int nColNum, long long &nResult);
	int GetData (int nColNum, float &fResult);
	int GetData (int nColNum, double &fResult);
	int GetData (int nColNum, char* &pResult);
	int GetData (int nColNum, char *szResult, int nSize, int *pIndex);

	// DB 결과 집합에서 해당하는 컬럼의 이름, 크기, 데이터형, 십진 숫자, nullable 등을 파악한다.
	int DescribeCol (int nColNum, char *pColName, int nColNameSize, int *pColSize);

	// 실행된 쿼리 Commit
	int Commit ();
	// 실행된 쿼리 Rollback
	int Rollback ();
	int SetOffAutoCommit ();
	int SetOnAutoCommit ();

	// 마지막 에러메세지 가져오기
	char* GetLastError ();

	// Caller 세팅
	void SetCaller (const std::string& strCaller);

	// ljh
	void DisplayError ();
	bool FreeStmt (SQLUSMALLINT option);
	bool BindParameter (SQLUSMALLINT ipar, SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType, SQLULEN cbColDef, SQLSMALLINT ibScale, SQLPOINTER rgbValue, SQLLEN cbValueMax, SQLLEN *pcbValue);
	bool Prepare (SQLCHAR* statementText);
	bool Execute (SQLCHAR* statementText);
	bool Execute ();
};
