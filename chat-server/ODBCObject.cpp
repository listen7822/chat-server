#include "LibODBCHeader.h"

ODBCObject* ODBCObject::mpThis = NULL;

ODBCObject::ODBCObject (void)
{
	for (int i = 0; i < MAX_DATABASE_COUNT; ++i)
	{
		mDatabases[i] = NULL;
	}
}

ODBCObject::~ODBCObject (void)
{
	for (int i = 0; i < MAX_DATABASE_COUNT; ++i)
	{
		if (mDatabases[i] != NULL)
		{
			delete mDatabases[i];
		}
		mDatabases[i] = NULL;
	}
}

ODBC* ODBCObject::GetODBC (DB_TYPE eType, int msTimeOut)
{
	//for to wait 0.01 sec
	const int subtractionPerTick = msTimeOut > 0 ? 10 : 0;
	int waitCnt = 0;
	ODBC *pCon = NULL;
	do
	{
		// connection pool null check
		if (mDatabases[eType] == NULL)
		{
			_Logf (GLog::LL_FATAL, "[ODBC] connection pool is null type[%d]", eType);
			return NULL;
		}

		pCon = mDatabases[eType]->GetConnection ();
		// if not gain ODBC, wait 0.01 sec.
		if (!pCon)
		{
			if ((msTimeOut -= subtractionPerTick) < 0)
			{
				return NULL;
			}
			_Logf (GLog::LL_DEBUG, "[ODBC] not gain DB connection. wait[%d] sec, remain wating time is [%d]ms.", MS_WAIT_PER_TICK * ++waitCnt, msTimeOut);
			XSystem::Threading::Sleep (MS_WAIT_PER_TICK);
		}
	} while (!pCon);

	return pCon;
}

void ODBCObject::CheckDatabase ()
{
	for (int i = 0; i < MAX_DATABASE_COUNT; ++i)
	{
		// database null check
		if (mDatabases[i] == NULL)
		{
			continue;
		}

		// health check connection 
		mDatabases[i]->CheckConnection ();
	}
}

bool ODBCObject::IsSuccessQuery (int retSQL)
{
	if (SQL_SUCCESS == retSQL || SQL_SUCCESS_WITH_INFO == retSQL || SQL_NO_DATA == retSQL)
	{
		return true;
	}

	return false;
}