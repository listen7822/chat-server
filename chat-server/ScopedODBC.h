#ifndef __SCOPEDODBC_H__
#define __SCOPEDODBC_H__

#include "LibODBCHeader.h"

/// <summary>
/// ODBC wrapper class.
/// 커넥션을 자동 반환하고 commit, rollback 하기 위해서 구현함.
/// </summary>
class ScopedODBC
{
private:
	ODBC* m_pODBC;			// ODBC 커넥션
	bool m_bIsQuerySucceed;	// 쿼리 성공 여부
	unsigned long mlStartSec;
	string mCaller;

	static std::set<std::string> mSet;
	static GCritSec mCS;

	void Init (DB_TYPE eType, int msTimeOut);
public:
	ScopedODBC (DB_TYPE eType, int msTimeOut, char* szFuncName);
	~ScopedODBC ();

	ODBC* operator->() { return m_pODBC; }

	int Excute (const char* szQuery);
};

#endif // __SCOPEDODBC_H__
