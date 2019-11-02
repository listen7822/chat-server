#pragma once

#include "stdafx.h"
#include "Singleton.h"

static const int MS_WAIT_PER_TICK = 10;
static const int MAX_DATABASE_COUNT = 10;

class ODBC;
class ODBCDatabase;

class ODBCObject : public Singleton<ODBCObject>
{
	ODBCDatabase* m_Database;

public:
	ODBCObject (void);
	~ODBCObject (void);

	ODBC* GetODBC ();

	template <typename T>
	bool AddDatabase ()
	{
		if (nullptr != m_Database)
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
			return false;
		}

		T* pDatabase = new T;
		if (!pDatabase->Initialize ())
		{
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
			delete pDatabase;
			return false;
		}

		m_Database = pDatabase;
		return true;
	}

	void CheckDatabase ();
	bool IsSuccessQuery (int retSQL);
};
