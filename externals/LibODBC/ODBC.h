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

// SQL DB Ÿ��
static const char* SQL_DB_TYPE_MS_SQL = "mssql";
static const char* SQL_DB_TYPE_ORACLE = "oracle";
static const char* SQL_DB_TYPE_MY_SQL = "mysql";

// SQL ���� �޼���
static const char* SQL_STATE_MS_SQL_08S01 = "08S01";

class ODBC
{
	// ODBC ȯ�� �ڵ�
	SQLHENV m_hEnv;
	// ODBC ���� �ڵ�
	SQLHDBC m_hDBC;
	// ODBC ��� �ڵ�
	SQLHSTMT m_hStmt;
	// ODBC ������� ��ġ
	std::string m_StrCaller;

	// ODBC ��� ���ҽ� ����
	void FreeConnection ();
	// �Է¹��� �����ڵ尡 DB ������ ���������� �Ǵ�
	bool IsDisconnect (const char *pSqlState, int NativeCode);
	// ������ �����ϰ� ������ �������� �翬���ϰ� ������ �ٽ� ����
	void CheckError (int retSQL, const char *pExecute = nullptr);
	// ������ ����
	void ExtractError (char *fn, SQLHANDLE handle, SQLSMALLINT type);

protected:
	// DB ��� ����
	bool m_bUsing;

	// DB Ÿ�� (oracle, mysql, mssql)
	char m_szDBType[SQL_MAX_MESSAGE_LENGTH];
	// DB ����̹� ����
	char m_szDriver[SQL_MAX_MESSAGE_LENGTH];
	// DB �̸�
	char m_szDBName[SQL_MAX_MESSAGE_LENGTH];
	// Database Source Name
	char m_szDSN[SQL_MAX_MESSAGE_LENGTH];

	// DB ���� ���� ����
	char m_szID[SQL_MAX_MESSAGE_LENGTH];
	char m_szPW[SQL_MAX_MESSAGE_LENGTH];

	// DB ���� ���
	char m_szIP[SQL_MAX_MESSAGE_LENGTH];
	int m_nPort;

	// db ó�� last error
	char m_szErrorMessage[SQL_MAX_MESSAGE_LENGTH];

public:
	ODBC (void);
	~ODBC (void);

	// DB ��������� üũ
	bool IsOnUsing ();
	// DB ������ ����
	void SetOnUsing (bool bUsing);

	// DB ���� �õ�
	int Connect (const char *pDBType, const char *pDriver, const char *pIP, int nPort, const char *pDSN, const char *pDBName, const char *pID, const char *pPW);
	// DB ������� üũ �� �翬�� ��û
	int CheckConnection ();

	// ���� ��û
	int Query (const char *pQuery);

	// DB ���� ��� ������ ���������� Ŀ�� ����
	int Fetch ();
	// Fetch �� ������ ���� �ݱ�
	int CloseCursor ();
	// DB ���� ��� ������ �ο� ���� �����´�.
	int GetNumOfResultRows ();
	// DB ���� ��� ������ �÷� ���� �����´�.
	int GetNumOfResultCols ();

	// DB ���� ��� ������ ������ �÷��� Ÿ�Կ� �´� ������ �����Ѵ�.
	int GetData (int nColNum, bool &nResult);
	int GetData (int nColNum, short &nResult);
	int GetData (int nColNum, int &nResult);
	int GetData (int nColNum, long long &nResult);
	int GetData (int nColNum, float &fResult);
	int GetData (int nColNum, double &fResult);
	int GetData (int nColNum, char* &pResult);
	int GetData (int nColNum, char *szResult, int nSize, int *pIndex);

	// DB ��� ���տ��� �ش��ϴ� �÷��� �̸�, ũ��, ��������, ���� ����, nullable ���� �ľ��Ѵ�.
	int DescribeCol (int nColNum, char *pColName, int nColNameSize, int *pColSize);

	// ����� ���� Commit
	int Commit ();
	// ����� ���� Rollback
	int Rollback ();
	int SetOffAutoCommit ();
	int SetOnAutoCommit ();

	// ������ �����޼��� ��������
	char* GetLastError ();

	// Caller ����
	void SetCaller (const std::string& strCaller);

	// ljh
	void DisplayError ();
	bool FreeStmt (SQLUSMALLINT option);
	bool BindParameter (SQLUSMALLINT ipar, SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType, SQLULEN cbColDef, SQLSMALLINT ibScale, SQLPOINTER rgbValue, SQLLEN cbValueMax, SQLLEN *pcbValue);
	bool Prepare (SQLCHAR* statementText);
	bool Execute (SQLCHAR* statementText);
	bool Execute ();
};
