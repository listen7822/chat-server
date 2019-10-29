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
	// �ռ� ������ ���������� �ѹ��Ѵ�.
	if (false == m_bIsQuerySucceed)
	{
		m_pODBC->Rollback ();
	}
	else
	{
		m_pODBC->Commit ();
	}

	// ����� Ŀ�ؼ��� ��ȯ���ش�.
	m_pODBC->SetOnUsing (false);
}

/// <summary>
/// ODBC�� Query�� �ѹ� ���� �Լ�.
/// </summary>
/// <param name="szQuery">���� ���ڿ�</param>
/// <returns>SQL ���� �ڵ�</returns>
int ScopedODBC::Execute (const char* szQuery)
{
	// �����ڿ��� Ŀ�ؼ��� �޾ƿ������� ���.
	if (NULL == m_pODBC)
	{
		//_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. m_pODBC is NULL");
		return SQL_ERROR;
	}

	int retSQL = m_pODBC->Query (szQuery);

	// Ŀ�� �Ǵ� �ѹ��ϱ� ���� ���� ���θ� �����Ѵ�.
	m_bIsQuerySucceed = ODBCObject::Instance ()->IsSuccessQuery (retSQL);

	if (false == m_bIsQuerySucceed)
	{
		//_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. Query[%s] is failed.", szQuery);
	}

	return retSQL;
}