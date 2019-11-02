#pragma once
#include <boost/winapi/basic_types.hpp>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

static const char* SQL_MSG_SUCCESS = "Success.";
static const char* SQL_MSG_NO_DATA = "No Data";

static const char* SQL_DB_TYPE_MS_SQL = "mssql";
static const char* SQL_DB_TYPE_ORACLE = "oracle";
static const char* SQL_DB_TYPE_MY_SQL = "mysql";

static const char* SQL_STATE_MS_SQL_08S01 = "08S01";

class ODBC
{
	SQLHENV m_hEnv;
	SQLHDBC m_hDBC;
	SQLHSTMT m_hStmt;
	std::string m_StrCaller;

	void FreeConnection ();
	bool IsDisconnect (const char *pSqlState, int NativeCode);
	void CheckError (int retSQL, const char *pExecute = nullptr);
	void ExtractError (char *fn, SQLHANDLE handle, SQLSMALLINT type);

protected:
	bool m_bUsing;

	char m_szDBType[SQL_MAX_MESSAGE_LENGTH];
	char m_szDriver[SQL_MAX_MESSAGE_LENGTH];
	char m_szDBName[SQL_MAX_MESSAGE_LENGTH];
	char m_szDSN[SQL_MAX_MESSAGE_LENGTH];

	char m_szID[SQL_MAX_MESSAGE_LENGTH];
	char m_szPW[SQL_MAX_MESSAGE_LENGTH];

	char m_szIP[SQL_MAX_MESSAGE_LENGTH];
	int m_nPort;

	char m_szErrorMessage[SQL_MAX_MESSAGE_LENGTH];

public:
	ODBC (void);
	~ODBC (void);

	bool IsOnUsing ();
	void SetOnUsing (bool bUsing);

	int Connect (const char *pDBType, const char *pDriver, const char *pIP, int nPort, const char *pDSN, const char *pDBName, const char *pID, const char *pPW);
	int CheckConnection ();

	int Query (const char *pQuery);

	int Fetch ();
	int CloseCursor ();
	int GetNumOfResultRows ();
	int GetNumOfResultCols ();

	int GetData (int nColNum, bool &nResult);
	int GetData (int nColNum, short &nResult);
	int GetData (int nColNum, int &nResult);
	int GetData (int nColNum, long long &nResult);
	int GetData (int nColNum, float &fResult);
	int GetData (int nColNum, double &fResult);
	int GetData (int nColNum, char* &pResult);
	int GetData (int nColNum, char *szResult, int nSize, int *pIndex);

	int DescribeCol (int nColNum, char *pColName, int nColNameSize, int *pColSize);

	int Commit ();
	int Rollback ();
	int SetOffAutoCommit ();
	int SetOnAutoCommit ();

	char* GetLastError ();

	void SetCaller (const std::string& strCaller);

	void DisplayError ();
	bool FreeStmt (SQLUSMALLINT option);
	bool BindParameter (SQLUSMALLINT ipar, SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType, SQLULEN cbColDef, SQLSMALLINT ibScale, SQLPOINTER rgbValue, SQLLEN cbValueMax, SQLLEN *pcbValue);
	bool Prepare (SQLCHAR* statementText);
	bool Execute (SQLCHAR* statementText);
	bool Execute ();
};
