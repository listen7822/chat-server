#include "stdafx.h"
#include <boost/winapi/basic_types.hpp>
#include <sql.h>
#include "ODBCObject.h"
#include "ODBC.h"
#include "ODBCDatabase.h"

ODBCObject::ODBCObject (void)
{
	m_Database = nullptr;
}

ODBCObject::~ODBCObject (void)
{
	delete m_Database;
}

ODBC* ODBCObject::GetODBC ()
{
	int waitCnt = 0;
	ODBC *pCon = nullptr;
	do
	{
		// connection pool null check
		if (nullptr == m_Database)
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
			return nullptr;
		}

		pCon = m_Database->GetConnection ();
	} while (!pCon);

	return pCon;
}

void ODBCObject::CheckDatabase ()
{
	// database null check
	if (nullptr == m_Database)
	{
		return;
	}

	// health check connection 
	m_Database->CheckConnection ();
}

bool ODBCObject::IsSuccessQuery (int retSQL)
{
	if (SQL_SUCCESS == retSQL || SQL_SUCCESS_WITH_INFO == retSQL || SQL_NO_DATA == retSQL)
	{
		return true;
	}

	return false;
}