#include "LibODBCHeader.h"

std::set<std::string> ScopedODBC::mSet;
GCritSec ScopedODBC::mCS;

ScopedODBC::ScopedODBC (DB_TYPE eType, int msTimeOut, char* szFuncName) : m_bIsQuerySucceed (false)
{
	mCaller.clear ();
	mCaller = szFuncName;
	Init (eType, msTimeOut);

	if (RuntimeFlag::ODBCTrace == TRUE)
	{
		mCS.Lock ();
		if (mSet.find (szFuncName) == mSet.end ())
		{
			mSet.insert (szFuncName);
			_Logf (GLog::LL_ERROR, "[ODBCWrapper][TraceQuery] [Func:%s]", szFuncName);
		}
		mCS.Unlock ();
	}
}

void ScopedODBC::Init (DB_TYPE eType, int msTimeOut)
{
	mlStartSec = GetTickCount ();

	m_pODBC = ODBCObject::GetInstance ()->GetODBC (eType, msTimeOut);
	if (m_pODBC != NULL)
	{
		m_pODBC->SetCaller (mCaller);
	}
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

	unsigned long ulEndSec = GetTickCount ();
	if ((ulEndSec - mlStartSec) > 1000)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper][DelayedQuery] [Caller:%s][Sec:%d]", mCaller.c_str (), ulEndSec - mlStartSec);
	}
}

/// <summary>
/// ODBC의 Query를 한번 감싼 함수.
/// </summary>
/// <param name="szQuery">쿼리 문자열</param>
/// <returns>SQL 에러 코드</returns>
int ScopedODBC::Excute (const char* szQuery)
{
	// 생성자에서 커넥션을 받아오지못한 경우.
	if (NULL == m_pODBC)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. m_pODBC is NULL");
		return SQL_ERROR;
	}

	int retSQL = m_pODBC->Query (szQuery);

	// 커밋 또는 롤백하기 위해 성공 여부를 저장한다.
	m_bIsQuerySucceed = ODBCObject::GetInstance ()->IsSuccessQuery (retSQL);

	if (false == m_bIsQuerySucceed)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. Query[%s] is failed.", szQuery);
	}

	return retSQL;
}