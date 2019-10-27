#include "pch.h"
#include "ODBC.h"

ODBC::ODBC (void)
{
	m_hEnv = NULL;
	m_hStmt = NULL;
	m_hDBC = NULL;
	m_StrCaller.clear ();

	m_bUsing = false;

	memset (m_szDBType, 0, sizeof (m_szDBType));
	memset (m_szDriver, 0, sizeof (m_szDriver));
	memset (m_szDBName, 0, sizeof (m_szDBName));
	memset (m_szDSN, 0, sizeof (m_szDSN));

	memset (m_szID, 0, sizeof (m_szID));
	memset (m_szPW, 0, sizeof (m_szPW));

	memset (m_szIP, 0, sizeof (m_szIP));
	m_nPort = 0;

	memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
}

ODBC::~ODBC (void)
{
	FreeConnection ();
}

void ODBC::FreeConnection ()
{
	// ��� �ڵ� ����
	if (m_hStmt != NULL)
	{
		SQLFreeStmt (m_hStmt, SQL_CLOSE);
	}

	// ���� �ڵ� ����
	if (m_hDBC != NULL)
	{
		// ���� ����
		SQLDisconnect (m_hDBC);
		// �ڵ� ����
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
	}

	// ȯ�� �ڵ� ����
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

	//_Logf (GLog::LL_ERROR, "[ODBC] The driver reported the following diagnostics whilst running [%s]", fn);
	do
	{
		ret = SQLGetDiagRec (type, handle, ++i, state, &native, text, sizeof (text), &len);
		if (SQL_SUCCEEDED (ret))
		{
			//_Logf (GLog::LL_ERROR, "[ODBC] %s:%ld:%ld:%s", state, i, native, text);
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
		memset (szMessage, 0, sizeof (szMessage));
		SQLRETURN ret1 = SQLGetDiagRec (SQL_HANDLE_STMT, m_hStmt, i, szSqlState, &nNativeError, szMessage, SQL_MAX_MESSAGE_LENGTH, &nMessageLen);
		if (ret1 == SQL_NO_DATA)
			return;

		//_Logf (GLog::LL_ERROR, "[ODBC] SQLGetDiagRec() != SQL_NO_DATA, ret=%d, i=%d, szSqlState=%s, nNativeError=%d, szMessage = %s", ret1, i, szSqlState, nNativeError, szMessage);
	}
}

void ODBC::CheckError (int retSQL, const char *pExecute)
{
	if (pExecute == NULL)
	{
		return;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// SQLRETURN �� ����
	// SQL_SUCCESS            0        ����.
	// SQL_SUCCESS_WITH_INFO  1        ��� �߻��Ͽ����� ����
	// SQL_ERROR              -1       ����
	// SQL_INVALID_HANDLE     -2       ��ȿ
	// SQL_NO_DATA            100      �����Ͱ� ����(�����͸� �� �о��ų� ���� �����Ͱ� ���ų�)
	// SQL_NEED_DATA          99       �߰� ������ �ʿ�
	// SQL_STILL_EXECUTING    2        ���� (�񵿱�)������
	/////////////////////////////////////////////////////////////////////////////////////////////////
	memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
	if (retSQL == SQL_SUCCESS)
	{
		strncpy (m_szErrorMessage, SQL_MSG_SUCCESS, SQL_MAX_MESSAGE_LENGTH);
		return;
	}
	else if (retSQL == SQL_SUCCESS_WITH_INFO)
	{
		strncpy (m_szErrorMessage, SQL_MSG_SUCCESS, SQL_MAX_MESSAGE_LENGTH);
		return;
	}
	else if (retSQL == SQL_NO_DATA)
	{
		strncpy (m_szErrorMessage, SQL_MSG_NO_DATA, SQL_MAX_MESSAGE_LENGTH);
		return;
	}

	// ������ ���
	//_Logf (GLog::LL_ERROR, "[ODBC] SQL_ERROR retSQL(%d) pExecute(Query)=%s", retSQL, pExecute);

	// ��� ����(�ڼ���)�� ����Ѵ�.
	SQLCHAR szSqlstate[SQL_MAX_MESSAGE_LENGTH] = { 0, };
	SQLCHAR szMessageText[SQL_MAX_MESSAGE_LENGTH] = { 0, };
	SQLSMALLINT nTextLength = 0;
	SQLINTEGER nNativeError = 0;
	SQLRETURN retSQL2 = SQL_INVALID_HANDLE;
	// SQLGetDiagRec ����
	// 1. SQLSMALLINT	HandleType		�ڵ� Ÿ��
	// 2. SQLINTEGER	Handle			������ �ڵ�
	// 3. SQLSMALLINT	RecNumber		�������� ���� �� �˻��� ���� ����(1���� ����)
	// 4. SQLCHAR *		SqlState		5���� SQL ���� �ڵ� (ù 2�ڴ� Ŭ����, ���� 3�ڴ� ���� Ŭ������ ��Ÿ��)
	// 5. SQLINTEGER *	NativeErrorPtr	������ ���� �ڵ�
	// 6. SQLCHAR *		MessageText		���� ��� �ؽ�Ʈ
	// 7. SQLSMALLINT	BufferLength	MessageText ������ ũ��(SQL_MAX_MESSAGE_LENGTH����)
	// 8. SQLSMALLINT * TextLengthPtr	MessageText ���ۿ� ���ϵǴ� ����Ʈ ���� ������ ������
	// ���� : SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_ERROR, or SQL_INVALID_HANDLE
	for (int i = 1; i < 10; i++)
	{
		memset (szMessageText, 0, sizeof (szMessageText));
		// ����
		//retSQL2 = SQLGetDiagRec(SQL_HANDLE_ENV, m_hEnv, i, szSqlstate, &nNativeError, szMessageText, SQL_MAX_MESSAGE_LENGTH, &nTextLength);
		//_Logf(GLog::LL_ERROR, "[ODBC] SQL_HANDLE_TYPE = SQL_HANDLE_ENV,  RecNumber = %d, SQLState = %s, NativeError = %d, ErrorMessage = %s", i, szSqlstate, nNativeError, szMessageText);

		//retSQL2 = SQLGetDiagRec(SQL_HANDLE_DBC, m_hDBC, i, szSqlstate, &nNativeError, szMessageText, SQL_MAX_MESSAGE_LENGTH, &nTextLength);
		//_Logf(GLog::LL_ERROR, "[ODBC] SQL_HANDLE_TYPE = SQL_HANDLE_DBC,  RecNumber = %d, SQLState = %s, NativeError = %d, ErrorMessage = %s", i, szSqlstate, nNativeError, szMessageText);

		retSQL2 = SQLGetDiagRec (SQL_HANDLE_STMT, m_hStmt, i, szSqlstate, &nNativeError, szMessageText, SQL_MAX_MESSAGE_LENGTH, &nTextLength);
		if (retSQL2 == SQL_NO_DATA)
		{
			break;
		}

		//_Logf (GLog::LL_ERROR, "[ODBC] SQL_HANDLE_TYPE = SQL_HANDLE_STMT, RecNumber = %d, SQLState = %s, NativeError = %d, ErrorMessage = %s", i, szSqlstate, nNativeError, szMessageText);

		// ù��° �����޽����� �����Ѵ�.
		if (i == 1)
		{
			memset (m_szErrorMessage, 0, sizeof (m_szErrorMessage));
			strcpy (m_szErrorMessage, (char*)szMessageText);
			// ������ �߻��� ������ ������ �α׷� �������� Client ���Դ� "DB Error" ��� �޽����� �����Ѵ�.
			//strcpy(m_szErrorMessage, "DB Error");
		}

		// ���� ��� DB ������ ���������� �ٽ� �����ϱ�
		if (IsDisconnect ((char*)szSqlstate, nNativeError))
		{
			// Ȯ���� ������ ���� �ٽ� �����ϱ�
			FreeConnection ();
			retSQL2 = Connect (m_szDBType, m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
			if (retSQL2 != SQL_SUCCESS && retSQL2 != SQL_SUCCESS_WITH_INFO)
			{
				//_Logf (GLog::LL_ERROR, "[ODBC] fail to reconnect db retSQL2(%d)", retSQL2);
			}
			else
			{
				// ���� �����ϱ�
				retSQL2 = SQLExecDirect (m_hStmt, (SQLCHAR *)pExecute, (SQLINTEGER)strlen (pExecute));
				//_Logf (GLog::LL_INFO, "[ODBC] %x, DB Reconnected and Query ReExecute retSQL2(%d)", m_hDBC, retSQL2);
				//CheckError(retSQL2, pExecute);
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
	strncpy (m_szDBType, pDBType, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szDriver, pDriver, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szIP, pIP, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	m_nPort = nPort;
	strncpy (m_szDSN, pDSN, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szDBName, pDBName, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szID, pID, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szPW, pPW, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// �ʱ� NULL �� ODBC ȯ�� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hEnv SQLAllocHandle retSQL(%d)", retSQL);
		return retSQL;
	}

	// ODBC ȯ�� ����
	retSQL = SQLSetEnvAttr (m_hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hEnv SQLSetEnvAttr retSQL(%d)", retSQL);
		return retSQL;
	}

	// ODBC ���� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_DBC, m_hEnv, &m_hDBC);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLAllocHandle retSQL(%d)", retSQL);
		return retSQL;
	}

	// SQLDriverConnect�� ����Ͽ� ������ ���
	// DB Ÿ�Կ� ���� ����̹� ���� ���ڿ� �����
	char szDriverConnect[SQL_MAX_MESSAGE_LENGTH];
	memset (szDriverConnect, 0, sizeof (char) * 512);
	if (strncmp (m_szDBType, SQL_DB_TYPE_ORACLE, sizeof (SQL_DB_TYPE_ORACLE)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Uid=%s;Pwd=%s;", m_szDriver, m_szDSN, m_nPort, m_szID, m_szPW);
	}
	else if (strncmp (m_szDBType, SQL_DB_TYPE_MY_SQL, sizeof (SQL_DB_TYPE_MY_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;stmt=set names euckr;", m_szDriver, m_szIP, m_nPort, m_szDBName, m_szID, m_szPW);
	}
	else if (strncmp (m_szDBType, SQL_DB_TYPE_MS_SQL, sizeof (SQL_DB_TYPE_MS_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s,%d;DSN=%s;Database=%s;Uid=%s;Pwd=%s;", m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
	}
	else
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to match DB Type(%s, %s, %s), dbtype(%s)", SQL_DB_TYPE_ORACLE, SQL_DB_TYPE_MY_SQL, SQL_DB_TYPE_MS_SQL, m_szDBType);
		return -1;
	}

	// ���� ����� ������ ���ڿ�
	SQLCHAR SqlConnStrOut[SQL_MAX_MESSAGE_LENGTH];
	memset (SqlConnStrOut, 0, sizeof (SQLCHAR)*SQL_MAX_MESSAGE_LENGTH);
	// ���� ����� ������ ���ڿ� ����
	SQLSMALLINT	SqlcbConnStrOut = 0;
	// ODBC DB ����̹� ����
	retSQL = SQLDriverConnect (m_hDBC, NULL, (SQLCHAR*)szDriverConnect, SQL_NTS, SqlConnStrOut, SQL_MAX_MESSAGE_LENGTH, &SqlcbConnStrOut, SQL_DRIVER_NOPROMPT);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//ExtractError (__FUNCTION__, m_hDBC, SQL_HANDLE_DBC);
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLDriverConnect : %s retSQL(%d)", szDriverConnect, retSQL);
		return retSQL;
	}

	// ODBC ���� �Ӽ� ����
	// ���� ������ �����̸� �ڵ� Ŀ�Եǵ���
	//retSQL = SQLSetConnectAttr(m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	// SQLEndTran() �� ����Ͽ� Ʈ����� ó����
	retSQL = SQLSetConnectAttr (m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLSetConnectAttr retSQL(%d)", retSQL);
		return retSQL;
	}

	// ODBC ��� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_STMT, m_hDBC, &m_hStmt);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hStmt allocation retSQL(%d)", retSQL);
		return retSQL;
	}

	//_Logf (GLog::LL_INFO, "[ODBC] Connect Database Success [DBType:%s][Driver:%s][IP:%s][Port:%d][DSN:%s][Name:%s][retSQL:%d]", pDBType, pDriver, pIP, nPort, pDSN, pDBName, retSQL);

	return retSQL;
}

//-- [ORACLE FADE-OUT]
int ODBC::Connect (const char *pDBType, const char *pDriver, const char *pIP, int nPort, const char *pDSN, const char *pDBName, const char *pID, const char *pPW, bool bAutoCommit)
{
	strncpy (m_szDBType, pDBType, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szDriver, pDriver, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szIP, pIP, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	m_nPort = nPort;
	strncpy (m_szDSN, pDSN, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szDBName, pDBName, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szID, pID, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);
	strncpy (m_szPW, pPW, sizeof (char)*SQL_MAX_MESSAGE_LENGTH);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// �ʱ� NULL �� ODBC ȯ�� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hEnv SQLAllocHandle retSQL(%d)", retSQL);
		return retSQL;
	}

	// ODBC ȯ�� ����
	retSQL = SQLSetEnvAttr (m_hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hEnv SQLSetEnvAttr retSQL(%d)", retSQL);
		return retSQL;
	}

	// ODBC ���� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_DBC, m_hEnv, &m_hDBC);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLAllocHandle retSQL(%d)", retSQL);
		return retSQL;
	}

	// SQLDriverConnect�� ����Ͽ� ������ ���
	// DB Ÿ�Կ� ���� ����̹� ���� ���ڿ� �����
	char szDriverConnect[SQL_MAX_MESSAGE_LENGTH];
	memset (szDriverConnect, 0, sizeof (char) * 512);
	if (strncmp (m_szDBType, SQL_DB_TYPE_ORACLE, sizeof (SQL_DB_TYPE_ORACLE)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Uid=%s;Pwd=%s;", m_szDriver, m_szDSN, m_nPort, m_szID, m_szPW);
	}
	else if (strncmp (m_szDBType, SQL_DB_TYPE_MY_SQL, sizeof (SQL_DB_TYPE_MY_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s;Port=%d;Database=%s;User=%s;Password=%s;stmt=set names euckr;", m_szDriver, m_szIP, m_nPort, m_szDBName, m_szID, m_szPW);
	}
	else if (strncmp (m_szDBType, SQL_DB_TYPE_MS_SQL, sizeof (SQL_DB_TYPE_MS_SQL)) == 0)
	{
		sprintf_s (szDriverConnect, sizeof (szDriverConnect), "Driver={%s};Server=%s,%d;DSN=%s;Database=%s;Uid=%s;Pwd=%s;", m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
	}
	else
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to match DB Type(%s, %s, %s), dbtype(%s)", SQL_DB_TYPE_ORACLE, SQL_DB_TYPE_MY_SQL, SQL_DB_TYPE_MS_SQL, m_szDBType);
		return -1;
	}

	// ���� ����� ������ ���ڿ�
	SQLCHAR SqlConnStrOut[SQL_MAX_MESSAGE_LENGTH];
	memset (SqlConnStrOut, 0, sizeof (SQLCHAR)*SQL_MAX_MESSAGE_LENGTH);
	// ���� ����� ������ ���ڿ� ����
	SQLSMALLINT	SqlcbConnStrOut = 0;
	// ODBC DB ����̹� ����
	retSQL = SQLDriverConnect (m_hDBC, NULL, (SQLCHAR*)szDriverConnect, SQL_NTS, SqlConnStrOut, SQL_MAX_MESSAGE_LENGTH, &SqlcbConnStrOut, SQL_DRIVER_NOPROMPT);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		ExtractError (__FUNCTION__, m_hDBC, SQL_HANDLE_DBC);
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLDriverConnect : %s retSQL(%d)", szDriverConnect, retSQL);
		return retSQL;
	}


	if (!bAutoCommit)
	{
		// ODBC ���� �Ӽ� ����
		// ���� ������ �����̸� �ڵ� Ŀ�Եǵ���
		//retSQL = SQLSetConnectAttr(m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
		// SQLEndTran() �� ����Ͽ� Ʈ����� ó����
		retSQL = SQLSetConnectAttr (m_hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
		if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
		{
			SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
			m_hDBC = NULL;
			//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hDBC SQLSetConnectAttr retSQL(%d)", retSQL);
			return retSQL;
		}
	}

	// ODBC ��� �ڵ� ����
	retSQL = SQLAllocHandle (SQL_HANDLE_STMT, m_hDBC, &m_hStmt);
	if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle (SQL_HANDLE_DBC, m_hDBC);
		m_hDBC = NULL;
		//_Logf (GLog::LL_ERROR, "[ODBC] fail to m_hStmt allocation retSQL(%d)", retSQL);
		return retSQL;
	}

	//_Logf (GLog::LL_INFO, "[ODBC] Connect Database Success [DBType:%s][Driver:%s][IP:%s][Port:%d][DSN:%s][Name:%s][retSQL:%d]", pDBType, pDriver, pIP, nPort, pDSN, pDBName, retSQL);

	return retSQL;
}



bool ODBC::FreeStmt (SQLUSMALLINT option)
{
	if (m_hStmt == NULL)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] m_hStmt == NULL");
		return false;
	}

	SQLRETURN ret = SQLFreeStmt (m_hStmt, option);
	if (ret != SQL_SUCCESS)
	{
		//_Logf (GLog::LL_ERROR, "[ODBC] ret != SQL_SUCCESS, ret=%d", ret);
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
		_Logf (GLog::LL_ERROR, "[ODBC] m_hStmt == NULL");
		return false;
	}

	SQLRETURN ret = SQLBindParameter (m_hStmt, ipar, fParamType, fCType, fSqlType, cbColDef, ibScale, rgbValue, cbValueMax, pcbValue);
	if (ret != SQL_SUCCESS)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] ret != SQL_SUCCESS, ret=%d", ret);
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Execute (SQLCHAR* statementText)
{
	if (m_hStmt == NULL)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] m_hStmt == NULL");
		return false;
	}

	SQLRETURN ret = SQLExecDirect (m_hStmt, statementText, SQL_NTS);
	if (ret != SQL_SUCCESS)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] ret != SQL_SUCCESS, ret=%d", ret);
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Prepare (SQLCHAR* statementText)
{
	if (m_hStmt == NULL)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] m_hStmt == NULL");
		return false;
	}

	SQLRETURN ret = SQLPrepare (m_hStmt, statementText, SQL_NTS);
	if (ret != SQL_SUCCESS)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] ret != SQL_SUCCESS, ret=%d", ret);
		DisplayError ();
		return false;
	}

	return true;
}

bool ODBC::Execute ()
{
	if (m_hStmt == NULL)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] m_hStmt == NULL");
		return false;
	}

	SQLRETURN ret = SQLExecute (m_hStmt);
	if (ret != SQL_SUCCESS)
	{
		_Logf (GLog::LL_ERROR, "[ODBC] ret != SQL_SUCCESS, ret=%d", ret);
		DisplayError ();
		return false;
	}

	return true;
}

int ODBC::Query (const char *pQuery)
{
	_Logf (GLog::LL_DEBUG, "[ODBCQueryTrace] %s", pQuery);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// ��� �ڵ� �ʱ�ȭ
	if (m_hStmt != NULL)
	{
		SQLFreeStmt (m_hStmt, SQL_CLOSE);
	}

	// ���� �����ϱ�
	retSQL = SQLExecDirect (m_hStmt, (SQLCHAR *)pQuery, (SQLINTEGER)strlen (pQuery));

	CheckError (retSQL, pQuery);

	return retSQL;
}

int ODBC::CheckConnection ()
{
	SQLRETURN  retSQL;
	SQLINTEGER isConnected;

#ifdef _SQLNCLI_ODBC_
	// SQL_COPT_SS_CONNECTION_DEAD --> SQL Server Native Client driver �̻��� �������� ��� ����
	retSQL = SQLGetConnectAttr (m_hDBC, SQL_COPT_SS_CONNECTION_DEAD, &isConnected, 0, NULL);
#else
	// SQL_ATTR_CONNECTION_DEAD --> ǥ�� ODBC ���� Ư�� SQL_ATTR_CONNECTION_DEAD�� ���� �ֱ� ���� ���¸� ��ȯ�մϴ�. �� ���´� ���� ���� ���°� �ƴ� ���� �ֽ��ϴ�.
	retSQL = SQLGetConnectAttr (m_hDBC, SQL_ATTR_CONNECTION_DEAD, &isConnected, 0, NULL);
#endif
	// ������ ����������
	if (isConnected == SQL_CD_TRUE)
	{
		// Ȯ���� ������ ���� �ٽ� �����ϱ�
		FreeConnection ();
		retSQL = Connect (m_szDBType, m_szDriver, m_szIP, m_nPort, m_szDSN, m_szDBName, m_szID, m_szPW);
		if (retSQL != SQL_SUCCESS && retSQL != SQL_SUCCESS_WITH_INFO)
		{
			_Logf (GLog::LL_ERROR, "[ODBC] fail to reconnect db");
		}
		else
		{
			_Logf (GLog::LL_INFO, "[ODBC] %x, DB Reconnected.", m_hDBC);
		}
	}

	return retSQL;
}

int ODBC::GetNumOfResultRows ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}
	// ��� ������ row �����ľ�
	SQLLEN cnt = 0;
	SQLRowCount (m_hStmt, &cnt);

	// UPDATE, INSERT, DELETE�� ���� ���� �� ���� �ľ�
	//SQLINTEGER cnt2 = 0;
	//SQLRowCount(m_hStmt, &cnt2);

	return cnt;
}

int ODBC::GetNumOfResultCols ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}
	// ��� ������ col �����ľ�
	SQLSMALLINT cnt = 0;
	SQLNumResultCols (m_hStmt, &cnt);

	// UPDATE, INSERT, DELETE�� ���� ���� �� ���� �ľ�
	//SQLINTEGER cnt2 = 0;
	//SQLRowCount(m_hStmt, &cnt2);

	return cnt;
}

int ODBC::Fetch ()
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLINTEGER ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// Ŀ���� ���������� �����Ű�� ���ε�� ���� �����´�.
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

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// Ŀ���� �ݴ´�.
	retSQL = SQLCloseCursor (m_hStmt);

	CheckError (retSQL, "CloseCursor()");

	return retSQL;
}

int ODBC::GetData (int nColNum, bool &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_TINYINT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, byte &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_TINYINT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, short &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_SHORT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, int &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_LONG, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, long long &nResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_SBIGINT, &nResult, sizeof (nResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, float &fResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_FLOAT, &fResult, sizeof (fResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, double &fResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return retSQL;
	}
	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_DOUBLE, &fResult, sizeof (fResult), &ind);

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, char* &pResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	char dummy[10] = { 0, };
	// �켱 ���۱��̸� 0���� �Ͽ� ������ �������� ���̸� ind�� �ľ��� �д�.
	// MSDN ����
	// ������ SQLGetData�� �����ؼ� ȣ���Ͽ� text, ntext �Ǵ� image ������ ���� ������ ���� ���� �����͸� �˻��� �� �ֽ��ϴ�.
	// ū �� ���Ŀ� ���� StrLen_or_IndPtr�� ������ �߸��� �߻��� ��� SQL_NO_TOTAL(-4)�� ��ȯ�մϴ�.
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, dummy, 0, &ind);

	char *pTemp = NULL;
	// ������ ���̰� �ľǵȴٸ� �ش� ũ�⸸ŭ �����Ҵ��Ͽ� ��������
	if (retSQL == SQL_SUCCESS_WITH_INFO && ind > 0)
	{
		_Logf (GLog::LL_INFO, "[ODBC] GetData() Buffer Size : Current-Data Length(%d)", ind);
		// �� �Լ����� �޸� �Ҵ��� �ϱ⶧���� ȣ���� ������ ����� �Ѵ�.
		pTemp = new char[ind + 1];
		memset (pTemp, 0, sizeof (pTemp));
		// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
		retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, pTemp, ind + 1, &ind);
	}
	// �ʹ� Ŀ�� ���̸� �ľ��� �� ���ٸ� ���� ũ��� �����Ҵ��Ͽ� ��������
	else if (ind == SQL_NO_TOTAL)
	{
		_Logf (GLog::LL_INFO, "[ODBC] GetData() Buffer Size : Fix Length(1024*10)");
		pTemp = new char[1024 * 10];
		memset (pTemp, 0, sizeof (char)*(1024 * 10));
		// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
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

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_CHAR, szResult, nSize, &ind);
	(*pIndex) = ind;

	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, tm &tmResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	TIMESTAMP_STRUCT timestamp;

	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof (timestamp), &ind);
	if (retSQL == SQL_SUCCESS)
	{
		tmResult.tm_year = timestamp.year - 1900;
		tmResult.tm_mon = timestamp.month - 1;
		tmResult.tm_mday = timestamp.day;
		tmResult.tm_hour = timestamp.hour;
		tmResult.tm_min = timestamp.minute;
		tmResult.tm_sec = timestamp.second;
		timestamp.fraction;
	}
	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::GetData (int nColNum, SYSTEMTIME& stResult)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	TIMESTAMP_STRUCT timestamp;

	// ������ �÷��� ������ ��������(SQLGetData ���ο��� SQLFetch ���)
	retSQL = SQLGetData (m_hStmt, (SQLUSMALLINT)nColNum, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof (timestamp), &ind);
	if (retSQL == SQL_SUCCESS && ind > 0)
	{
		stResult.wYear = timestamp.year;
		stResult.wMonth = timestamp.month;
		stResult.wDay = timestamp.day;
		stResult.wHour = timestamp.hour;
		stResult.wMinute = timestamp.minute;
		stResult.wSecond = timestamp.second;
		timestamp.fraction;
	}
	CheckError (retSQL, "GetData()");

	return retSQL;
}

int ODBC::DescribeCol (int nColNum, char *pColName, int nColNameSize, int *pColSize)
{
	SQLRETURN retSQL = SQL_INVALID_HANDLE;
	SQLLEN ind = SQL_NTS;

	// ��� �ڵ��� ���ٸ� ����
	if (m_hStmt == NULL)
	{
		return SQL_INVALID_HANDLE;
	}

	SQLSMALLINT ColNameReturnSize = 0;	// �÷���ȣ�� ���� �̸��� ������ ���ۿ� ��ȯ�� ����Ʈ��(null-termination ����)
	SQLSMALLINT DataType = 0;			// �÷���ȣ�� ���� ������ Ÿ��
	SQLSMALLINT DecimalCnt = 0;			// �÷���ȣ�� ���� ���� ������ �ڸ���
	SQLSMALLINT Nullable = 0;			// �÷���ȣ�� ���� ���� null �� ����ϴ��� ����

	retSQL = SQLDescribeCol (m_hStmt, (SQLUSMALLINT)nColNum, (SQLCHAR*)pColName, nColNameSize, &ColNameReturnSize, &DataType, (SQLULEN*)pColSize, &DecimalCnt, &Nullable);
	//fprintf(stderr, "retSQL=%d\n", retSQL);
	//fprintf(stderr, "nColNum=%d\n", nColNum);
	//fprintf(stderr, "pColName=%s\n", pColName);
	//fprintf(stderr, "nColNameSize=%d\n", nColNameSize);
	//fprintf(stderr, "ColNameReturnSize=%d\n", ColNameReturnSize);
	//fprintf(stderr, "DataType=%d\n", DataType);
	//fprintf(stderr, "pColSize=%d\n", *pColSize);
	//fprintf(stderr, "DecimalCnt=%d\n", DecimalCnt);
	//fprintf(stderr, "Nullable=%d\n", Nullable);

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

void ODBC::SetCaller (const string& strCaller)
{
	m_StrCaller.clear ();
	m_StrCaller = strCaller;
}

//-- [ORACLE FADE-OUT]
SQLHSTMT ODBC::GetSqlHstmt ()
{
	return m_hStmt;
}

//-- [ORACLE FADE-OUT]
bool ODBC::Fetch (bool *nodata)
{

	SQLRETURN retSQL = SQLFetch (m_hStmt);
	if (nodata) {
		*nodata = (retSQL == SQL_NO_DATA);
		return true;
	}

	if (retSQL == SQL_SUCCESS || retSQL == SQL_SUCCESS_WITH_INFO) {
		return true;
	}
	else {
		if (retSQL != SQL_NO_DATA) {
			CheckError (retSQL, "Fetch()");
		}
		return false;
	}

	return false;
}

//-- [ORACLE FADE-OUT]
int ODBC::ExecuteDirect (const char *pQuery, char *pCaller)
{
	_Logf (GLog::LL_DEBUG, "[ODBC] %s", pQuery);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	unsigned long ulStartSec = ::GetTickCount ();

	// ���� �����ϱ�
	retSQL = SQLExecDirect (m_hStmt, (SQLCHAR *)pQuery, (SQLINTEGER)strlen (pQuery));
	if (!SQL_SUCCEEDED (retSQL)) {
		CheckError (retSQL, pQuery);
	}

	unsigned long ulEndSec = ::GetTickCount ();

	if ((ulEndSec - ulStartSec) > 1000)
	{
		_Logf (GLog::LL_ERROR, "[ODBC][ODBC::ExecuteDirect][Caller:%s][Sec:%d delayed]",
			pCaller, ulEndSec - ulStartSec);
	}

	return retSQL;
}


//-- [ORACLE FADE-OUT]
int ODBC::ExecuteDirect (const char *pQuery)
{
	_Logf (GLog::LL_DEBUG, "[ODBC] %s", pQuery);

	SQLRETURN retSQL = SQL_INVALID_HANDLE;

	// ���� �����ϱ�
	retSQL = SQLExecDirect (m_hStmt, (SQLCHAR *)pQuery, (SQLINTEGER)strlen (pQuery));
	if (!SQL_SUCCEEDED (retSQL)) {
		CheckError (retSQL, pQuery);
	}

	return retSQL;
}

