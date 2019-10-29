#pragma once

#include "Monitor.h"

class ODBC;

class ScopedODBC
{
private:
	ODBC* m_pODBC;			// ODBC 目池记
	bool m_bIsQuerySucceed;	// 孽府 己傍 咯何
	unsigned long mlStartSec;

	void Init ();
public:
	ScopedODBC ();
	~ScopedODBC ();

	ODBC* operator->() { return m_pODBC; }

	int Execute (const char* szQuery);
};

