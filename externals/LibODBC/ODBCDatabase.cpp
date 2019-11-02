#include "stdafx.h"
#include "ODBCDatabase.h"

ODBCDatabase::ODBCDatabase ()
{
}

ODBCDatabase::~ODBCDatabase ()
{
	Monitor::Owner lock (m_csODBCDatabase);
	{
		for (std::list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
		{
			ODBC* pODBC = *iter;
			delete pODBC;
		}
		mPool.clear ();
	}
}


// Get Database.
bool ODBCDatabase::Initialize ()
{
	std::string strDBType = GetConfigDBType ();
	if (strDBType == "")
	{
		return false;
	}

	std::string strDBDriver = GetConfigDBDriver ();
	if (strDBDriver == "")
	{
		return false;
	}

	std::string strDBIP = GetConfigDBIP ();
	if (strDBIP == "")
	{
		return false;
	}

	std::string strDBName = GetConfigDBName ();
	if (strDBName == "")
	{
		return false;
	}

	std::string strDBID = GetConfigDBID ();
	if (strDBID == "")
	{
		return false;
	}

	std::string strDBPWD = GetConfigDBPwd ();
	if (strDBPWD == "")
	{
		return false;
	}

	int nDBPort = GetConfigDBPort ();
	if (nDBPort <= 0)
	{
		return false;
	}

	int nDBCnt = GetConfigDBConnCnt ();
	if (nDBCnt <= 0)
	{
		return false;
	}

	// Make connection.
	for (int i = 0; i < nDBCnt; ++i)
	{
		ODBC* pODBC = new ODBC ();
		int dwRet = pODBC->Connect (strDBType.c_str (), strDBDriver.c_str (), strDBIP.c_str (), nDBPort, "DummyDSN", strDBName.c_str (), strDBID.c_str (), strDBPWD.c_str ());
		if (dwRet != SQL_SUCCESS)
		{
			break;
			BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
		}

		mPool.push_back (pODBC);
	}

	return true;
}

ODBC* ODBCDatabase::GetConnection ()
{
	static int nMaxTerm = 0, nMaxTotal = 0;
	static time_t tmLog = 0;
	int nUsing = 0;
	ODBC* pODBCCon = NULL;

	Monitor::Owner lock (m_csODBCDatabase);
	{
		for (std::list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
		{
			pODBCCon = *iter;

			if (pODBCCon->IsOnUsing ())
			{
				pODBCCon = NULL;
				++nUsing;
			}
			else
			{
				if (nMaxTerm < nUsing + 1)
				{
					nMaxTerm = nUsing + 1;
				}
				if (nMaxTotal < nUsing + 1)
				{
					nMaxTotal = nUsing + 1;
				}
				if (tmLog + LOG_TERM <= time (NULL))
				{
					tmLog = time (NULL);
					// TODO: occurred error!
					nMaxTerm = 0;
				}
				pODBCCon->SetOnUsing (true);
				break;
			}
		}
	}

	return pODBCCon;
}

void ODBCDatabase::CheckConnection ()
{
	ODBC *pODBCCon = NULL;

	Monitor::Owner lock (m_csODBCDatabase);
	{
		for (std::list<ODBC*>::iterator iter = mPool.begin (); iter != mPool.end (); ++iter)
		{
			pODBCCon = *iter;

			if (pODBCCon->IsOnUsing ())
			{
				pODBCCon = NULL;
			}
			else
			{
				// Excute query.
				pODBCCon->SetOnUsing (true);
				int retSQL = pODBCCon->Query (HEALTH_CHECK_QUERY_STRING.c_str ());
				if (retSQL == SQL_SUCCESS)
				{
				}
				else
				{
					BOOST_LOG_TRIVIAL (error) << "Func: "<< __FUNCTION__ << " Line: " <<__LINE__;
				}
				pODBCCon->Commit ();
				pODBCCon->SetOnUsing (false);
			}
		}
	}
}
