#pragma once

#include "Monitor.h"

class ODBC;

class ScopedODBC
{
private:
	ODBC* m_pODBC;			
	bool m_bIsQuerySucceed;
	unsigned long mlStartSec;

	void Init ();
public:
	ScopedODBC ();
	~ScopedODBC ();

	ODBC* operator->() { return m_pODBC; }

	int Execute (const char* szQuery);
};

