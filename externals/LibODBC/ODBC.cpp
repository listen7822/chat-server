#include "stdafx.h"
#include "ODBC.h"

ODBC::ODBC::ODBC (void)
{
	m_hEnv = NULL;
	m_hStmt = NULL;
	m_hDBC = NULL;
	m_StrCaller.clear ();

	m_bUsing = false;

	std::memset (m_szDBType, 0, sizeof (m_szDBType));
	std::memset (m_szDriver, 0, sizeof (m_szDriver));
	std::memset (m_szDBName, 0, sizeof (m_szDBName));
	std::memset (m_szDSN, 0, sizeof (m_szDSN));

	std::memset (m_szID, 0, sizeof (m_szID));
	std::memset (m_szPW, 0, sizeof (m_szPW));

	std::memset (m_szIP, 0, sizeof (m_szIP));
	m_nPort = 0;

	std::memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
}

ODBC::~ODBC (void)
{
	FreeConnection ();
}

void ODBC::FreeConnection ()
{
	if (m_hStmt != NULL)
	{
		SQLFreeStmt (m_hStmt, SQL_CLOSE);
	}

	if (m_hDBC != NULL)
	{
		SQLDisconnect (m_hDBC);
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
	}

	if (m_hEnv != NULL)
	{
		SQLFreeEnv (m_hEnv);
	}
}

bool ODBC::IsDisconnect (const char *pSqlState, int NativeCode)
{
	bool bDisconnect = false;

	if (strcmp (m_szDBType, SQL_DB_TYPE_MS_SQL) == 0)
	{
		if (strcmp (pSqlState, SQL_STATE_MS_SQL_08S01) == 0)
		{
			bDisconnect = true;
		}

		switch (NativeCode)
		{
			// 10054 : Connection Reset By peer
			// 10061 : Network Error
		case 10054:
		case 10061:
			bDisconnect = true;
			break;
		}
	}
	else if (strcmp (m_szDBType, SQL_DB_TYPE_ORACLE) == 0)
	{
		// ORA-00028: your session has been killed
		// ORA-01012: not logged on
		// ORA-01033: ORACLE initialization or shutdown in progress
		// ORA-01034: ORACLE not available
		// ORA-01089: immediate shutdown in progress - no operations are permitted 
		// ORA-03113: end-of-file on communication channel
		// ORA-03114: not connected to ORACLE
		// ORA-03127: no new operations allowed until the active operation ends
		// ORA-12571: TNS:packet writer failure
		switch (NativeCode)
		{
		case 28:
		case 1012:
		case 1033:
		case 1034:
		case 1089:
		case 3113:
		case 3114:
		case 3127:
		case 12571:
			bDisconnect = true;
			break;
		}
	}
	else if (strcmp (m_szDBType, SQL_DB_TYPE_MY_SQL) == 0)
	{
		// 2003 : Can't connect to MySQL server
		// 2013 : Lost connection to MySQL server during query
		// 2006 : MySQL server has gone away 
		switch (NativeCode)
		{
		case 2003:
		case 2006:
		case 2013:
			bDisconnect = true;
			break;
		}
	}

	return bDisconnect;
}

void ODBC::ExtractError (char *fn, SQLHANDLE handle, SQLSMALLINT type)
{
	SQLSMALLINT	 i = 0;
	SQLINTEGER	 native;
	SQLCHAR	 state[7];
	SQLCHAR	 text[256];
	SQLSMALLINT	 len;
	SQLRETURN	 ret;

	BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
	do
	{
		ret = SQLGetDiagRec (type, handle, ++i, state, &native, text, sizeof (text), &len);
		if (SQL_SUCCEEDED (ret))
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		}
	} while (ret == SQL_SUCCESS);
}

void ODBC::DisplayError ()
{
	SQLCHAR szSqlState[6] = { 0, };
	SQLINTEGER nNativeError = 0;
	SQLCHAR szMessage[SQL_MAX_MESSAGE_LENGTH] = { 0, };
	SQLSMALLINT nMessageLen = 0;

	for (int i = 1; i < 10/*no whileloop*/; ++i)
	{
		std::memset (szMessage, 0, sizeof (szMessage));
		SQLRETURN ret1 = SQLGetDiagRec (SQL_HANDLE_STMT, m_hStmt, i, szSqlState, &nNativeError, szMessage, SQL_MAX_MESSAGE_LENGTH, &nMessageLen);
		if (ret1 == SQL_NO_DATA)
			return;

		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
	}
}

void ODBC::CheckError (int retSQL, const char *pExecute)
{
	if (pExecute == NULL)
	{
		return;
	}

	std::memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
	if (retSQL == SQL_SUCCESS)
	{
		strncpy_s (m_szErrorMessage, SQL_MSG_SUCCESS, SQL_MAX_MESSAGE_LENGTH);
		return;
	}
	else if (retSQL == SQL_SUCCESS_WITH_INFO)
	{
		strncpy_s (m_szErrorMessage, SQL_MSG_SUCCESS, SQL_MAX_MESSAGE_LENGTH);
		return;
	}
	else if (retSQL == SQL_NO_DATA)
	{
		strncpy_s (m_szErrorMessage, SQL_MSG_NO_DATA, SQL_MAX_MESSAGE_LENGTH);
		return;
	}

	SQLCHAR szSqlstate[SQL_MAX_MESSAGE_LENGTH] = { 0, };
	SQLCHAR szMessageText[SQL_MAX_MESSAGE_LENGTH] = { 0, };
	SQLSMALLINT nTextLength = 0;
	SQLINTEGER nNativeError = 0;
	SQLRETURN retSQL2 = SQL_INVALID_HANDLE;
	for (int i = 1; i < 10; i++)
	{
		std::memset (szMessageText, 0, sizeof (szMessageText));

		retSQL2 = SQLGetDiagRec (SQL_HANDLE_STMT, m_hStmt, i, szSqlstate, &nNativeError, szMessageText, SQL_MAX_MESSAGE_LENGTH, &nTextLength);
		if (retSQL2 == SQL_NO_DATA)
		{
			break;
		}

		if (i == 1)
		{
			std::memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
			strncpy_s (m_szErrorMessage, (char*)szMessageText, SQL_MAX_MESSAGE_LENGTH);
		}

		if (IsDisconnect ((char*)szSqlstate, nNativeError))
		{
			FreeConnection ();
			retSQL2 = Connect (m_szDBType, m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
			if (retSQL2 != SQL_SUCCESS && retSQL2 != SQL_SUCCESS_WITH_INFO)
			{
				BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
			}
			else
			{
				retSQL2 = SQLExecDirect (m_hStmt, (SQLCHAR *)pExecute, (SQLINTEGER)strlen (pExecute));
				BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
			}
		}
	}
}

bool ODBC::IsOnUsing ()
{
	return m_bUsing;
}

void ODBC::SetOnUsing (bool bUsing)
{
	m_bUsing = bUsing;
}

int ODBC::Connect (const char *pDBType, const char *pDriver, const char *pIP, int nPort, const char *pDSN, const char *pDBName, const char *pID, const char *pPW)
{
	strncpy_s (m_szDBType, pDBType, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy_s (m_szDriver, pDriver, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy_s (m_szIP, pIP, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	m_nPort = nPort;
	strncpy_s (m_szDSN, pDSN, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy_s (m_szDBName, pDBName, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy_s (m_szID, pID, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy_s (m_szPW, pPW, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	retSQL = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	retSQL = SQLSetEnvAttr (m_hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	retSQL = SQLAllocHandle (SQL_HANDLE_DBC, m_hEnv, &m_hDBC);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	char szDriverConnect[SQL_MAX_MESSAGE_LENGTH];
	std::memset (szDriverConnect, 0, sizeof (char) * 512);
	if (std::strncmp (m_szDBType, SQL_DB_TYPE_ORACLE, sizeof (SQL_DB_TYPE_ORACLE)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Uid=%s;Pwd=%s;", m_szDriver, m_szDSN, m_nPort, m_szID, m_szPW);
	}
	else if (std::strncmp (m_szDBType, SQL_DB_TYPE_MY_SQL, sizeof (SQL_DB_TYPE_MY_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;stmt=set names euckr;", m_szDriver, m_szIP, m_nPort, m_szDBName, m_szID, m_szPW);
	}
	else if (std::strncmp (m_szDBType, SQL_DB_TYPE_MS_SQL, sizeof (SQL_DB_TYPE_MS_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s,%d;DSN=%s;Database=%s;Uid=%s;Pwd=%s;", m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
	}
	else
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return -1;
	}

	SQLCHAR SqlConnStrOut[SQL_MAX_MESSAGE_LENGTH];
	std::memset (SqlConnStrOut, 0, sizeof (SQLCHAR)*SQL_MAX_MESSAGE_LENGTH);
	SQLSMALLINT	SqlcbConnStrOut = 0;
	retSQL = SQLDriverConnect (m_hDBC, NULL, (SQLCHAR*)szDriverConnect, SQL_NTS, SqlConnStrOut, SQL_MAX_MESSAGE_LENGTH, &SqlcbConnStrOut, SQL_DRIVER_NOPROMPT);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	retSQL = SQLSetConnectAttr (m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	retSQL = SQLAllocHandle (SQL_HANDLE_STMT, m_hDBC, &m_hStmt);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return retSQL;
	}

	return retSQL;
}

bool ODBC::FreeStmt (SQLUSMALLINT option)
{
	if (m_hStmt == NULL)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return false;
	}

	SQLRETURN ret = SQLFreeStmt (m_hStmt, option);
	if (ret != SQL_SUCCESS)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::BindParameter (SQLUSMALLINT ipar, SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType,
	SQLULEN cbColDef, SQLSMALLINT ibScale, SQLPOINTER rgbValue, SQLLEN cbValueMax,
	SQLLEN *pcbValue)
{
	if (m_hStmt == NULL)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return false;
	}

	SQLRETURN ret = SQLBindParameter (m_hStmt, ipar, fParamType, fCType, fSqlType, cbColDef, ibScale, rgbValue, cbValueMax, pcbValue);
	if (ret != SQL_SUCCESS)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Execute (SQLCHAR* statementText)
{
	if (m_hStmt == NULL)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return false;
	}

	SQLRETURN ret = SQLExecDirect (m_hStmt, statementText, SQL_NTS);
	if (ret != SQL_SUCCESS)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Prepare (SQLCHAR* statementText)
{
	if (m_hStmt == NULL)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return false;
	}

	SQLRETURN ret = SQLPrepare (m_hStmt, statementText, SQL_NTS);
	if (ret != SQL_SUCCESS)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Execute ()
{
	if (m_hStmt == NULL)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return false;
	}

	SQLRETURN ret = SQLExecute (m_hStmt);
	if (ret != SQL_SUCCESS)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		DisplayError ();
		return false;
	}

	return true;
}

int ODBC::Query (const char *pQuery)
{
	BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	if (m_hStmt != NULL)
	{
		SQLFreeStmt (m_hStmt, SQL_CLOSE);
	}

	retSQL = SQLExecDirect (m_hStmt, (SQLCHAR *)pQuery, (SQLINTEGER)strlen (pQuery));

	CheckError (retSQL, pQuery);

	return retSQL;
}

int ODBC::CheckConnection ()
{
	SQLRETURN  retSQL;
	SQLINTEGER isConnected;

#ifdef _SQLNCLI_ODBC_
	retSQL = SQLGetConnectAttr (m_hDBC, SQL_COPT_SS_CONNECTION_DEAD, &isConnected, 0, NULL);
#else
	retSQL = SQLGetConnectAttr (m_hDBC, SQL_ATTR_CONNECTION_DEAD, &isConnected, 0, NULL);
#endif
	if (isConnected == SQL_CD_TRUE)
	{
		FreeConnection ();
		retSQL = Connect (m_szDBType, m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
		if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		}
		else
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		}
	}

	return retSQL;
}

int ODBC::GetNumOfResultRows ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}
	SQLLEN cnt = 0;
	SQLRowCount (m_hStmt, &cnt);

	return cnt;
}

int ODBC::GetNumOfResultCols ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}
	SQLSMALLINT cnt = 0;
	SQLNumResultCols (m_hStmt, &cnt);

	return cnt;
}

int ODBC::Fetch ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLINTEGER ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLFetch (m_hStmt);

	if (retSQL != SQL_SUCCESS && retSQL != SQL_NO_DATA)
	{
		CheckError (retSQL, "Fetch()");
	}

	return retSQL;
}

int ODBC::CloseCursor ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLINTEGER ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLCloseCursor (m_hStmt);

	CheckError (retSQL, "CloseCursor()");

	return retSQL;
}

int ODBC::GetData (int nColNum, bool &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_TINYINT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, short &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_SHORT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, int &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_LONG, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, long long &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_SBIGINT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, float &fResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_FLOAT, &fResult, sizeof (fResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, double &fResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_DOUBLE, &fResult, sizeof (fResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, char* &pResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	char dummy[10] = { 0, };
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, dummy, 0, &ind);

	char *pTemp = NULL;
	if (retSQL == SQL_SUCCESS_WITH_INFO && ind > 0)
	{
		pTemp = new char[ind + 1];
		std::memset (pTemp, 0, sizeof (pTemp));
		retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, pTemp, ind + 1, &ind);
	}
	else if (ind == SQL_NO_TOTAL)
	{
		pTemp = new char[1024 * 10];
		std::memset (pTemp, 0, sizeof (char)*(1024 * 10));
		retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, pTemp, 1024 * 10, &ind);
	}
	pResult = pTemp;
	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, char *szResult, int nSize, int *pIndex)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, szResult, nSize, &ind);
	(*pIndex) = ind;

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::DescribeCol (int nColNum, char *pColName, int nColNameSize, int *pColSize)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	SQLSMALLINT ColNameReturnSize = 0;	
	SQLSMALLINT DataType = 0;			
	SQLSMALLINT DecimalCnt = 0;			
	SQLSMALLINT Nullable = 0;		

	retSQL = SQLDescribeCol (m_hStmt, (SQLUSMALLINT)nColNum, (SQLCHAR*)pColName, nColNameSize, &ColNameReturnSize, &DataType, (SQLULEN*)pColSize, &DecimalCnt, &Nullable);

	CheckError (retSQL, "DBDescribeCol()");

	return retSQL;
}

int ODBC::Commit ()
{
	SQLRETURN retSQL = SQLEndTran (SQL_HANDLE_DBC, m_hDBC, SQL_COMMIT);
	CheckError (retSQL, "Commit");
	return retSQL;
}

int ODBC::Rollback ()
{
	SQLRETURN retSQL = SQLEndTran (SQL_HANDLE_DBC, m_hDBC, SQL_ROLLBACK);
	CheckError (retSQL, "Rollback");
	return retSQL;
}

int ODBC::SetOffAutoCommit ()
{
	SQLRETURN retSQL = SQLSetConnectAttr (m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
	return retSQL;
}

int ODBC::SetOnAutoCommit ()
{
	SQLRETURN retSQL = SQLSetConnectAttr (m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	return retSQL;
}

char* ODBC::GetLastError ()
{
	return m_szErrorMessage;
}

void ODBC::SetCaller (const std::string& strCaller)
{
	m_StrCaller.clear ();
	m_StrCaller = strCaller;
}

