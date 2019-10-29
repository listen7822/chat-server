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
	// 앞서 쿼리가 실패했으면 롤백한다.
	if (false == m_bIsQuerySucceed)
	{
		m_pODBC->Rollback ();
	}
	else
	{
		m_pODBC->Commit ();
	}

	// 사용한 커넥션을 반환해준다.
	m_pODBC->SetOnUsing (false);
}

/// <summary>
/// ODBC의 Query를 한번 감싼 함수.
/// </summary>
/// <param name="szQuery">쿼리 문자열</param>
/// <returns>SQL 에러 코드</returns>
int ScopedODBC::Execute (const char* szQuery)
{
	// 생성자에서 커넥션을 받아오지못한 경우.
	if (NULL == m_pODBC)
	{
		//_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. m_pODBC is NULL");
		return SQL_ERROR;
	}

	int retSQL = m_pODBC->Query (szQuery);

	// 커밋 또는 롤백하기 위해 성공 여부를 저장한다.
	m_bIsQuerySucceed = ODBCObject::Instance ()->IsSuccessQuery (retSQL);

	if (false == m_bIsQuerySucceed)
	{
		//_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. Query[%s] is failed.", szQuery);
	}

	return retSQL;
}