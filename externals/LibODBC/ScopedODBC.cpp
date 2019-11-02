#include "stdafx.h"
#include "ScopedODBC.h"
#include "ODBCObject.h"
#include "ODBC.h"

ScopedODBC::ScopedODBC () : m_bIsQuerySucceed (false)
{
	Init ();
}

void ScopedODBC::Init ()
{
	m_pODBC = ODBCObject::Instance ()->GetODBC ();
}

ScopedODBC::~ScopedODBC ()
{
	// rollback query.
	if (false == m_bIsQuerySucceed)
	{
		m_pODBC->Rollback ();
	}
	else
	{
		m_pODBC->Commit ();
	}

	// Returning connection to pool.
	m_pODBC->SetOnUsing (false);
}

int ScopedODBC::Execute (const char* szQuery)
{
	if (NULL == m_pODBC)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		return SQL_ERROR;
	}

	int retSQL = m_pODBC->Query (szQuery);

	m_bIsQuerySucceed = ODBCObject::Instance ()->IsSuccessQuery (retSQL);

	if (false == m_bIsQuerySucceed)
	{
		BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
	}

	return retSQL;
}