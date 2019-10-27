#ifndef __SCOPEDODBC_H__
#define __SCOPEDODBC_H__

#include "LibODBCHeader.h"

/// <summary>
/// ODBC wrapper class.
/// Ŀ�ؼ��� �ڵ� ��ȯ�ϰ� commit, rollback �ϱ� ���ؼ� ������.
/// </summary>
class ScopedODBC
{
private:
	ODBC* m_pODBC;			// ODBC Ŀ�ؼ�
	bool m_bIsQuerySucceed;	// ���� ���� ����
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
