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

	unsigned long ulEndSec = GetTickCount ();
	if ((ulEndSec - mlStartSec) > 1000)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper][DelayedQuery] [Caller:%s][Sec:%d]", mCaller.c_str (), ulEndSec - mlStartSec);
	}
}

/// <summary>
/// ODBC�� Query�� �ѹ� ���� �Լ�.
/// </summary>
/// <param name="szQuery">���� ���ڿ�</param>
/// <returns>SQL ���� �ڵ�</returns>
int ScopedODBC::Excute (const char* szQuery)
{
	// �����ڿ��� Ŀ�ؼ��� �޾ƿ������� ���.
	if (NULL == m_pODBC)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. m_pODBC is NULL");
		return SQL_ERROR;
	}

	int retSQL = m_pODBC->Query (szQuery);

	// Ŀ�� �Ǵ� �ѹ��ϱ� ���� ���� ���θ� �����Ѵ�.
	m_bIsQuerySucceed = ODBCObject::GetInstance ()->IsSuccessQuery (retSQL);

	if (false == m_bIsQuerySucceed)
	{
		_Logf (GLog::LL_ERROR, "[ODBCWrapper] ODBCWrapper::Execute. Query[%s] is failed.", szQuery);
	}

	return retSQL;
}