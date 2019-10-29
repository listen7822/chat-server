#pragma once

#include "Singleton.h"

static const int MS_WAIT_PER_TICK = 10;
static const int MAX_DATABASE_COUNT = 10;

class ODBC;
class ODBCDatabase;

class ODBCObject : public Singleton<ODBCObject>
{
	// ���� Database ���
	ODBCDatabase* m_Database;

public:
	ODBCObject (void);
	~ODBCObject (void);

	// ODBC �ν��Ͻ� ��������
	ODBC* GetODBC ();

	// �����ͺ��̽� �߰��ϴ� ���ø��Լ��ν�, ODBCPool �� 
	// ����� �������� ��ӹ޾� ������ Ŭ������ Ÿ���� �Ѱ��ش�.
	// lib ����  ���ø� �Լ��� ����ϱ� ���� ������� ����.
	template <typename T>
	bool AddDatabase ()
	{
		if (nullptr != m_Database)
		{
			//_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is already exists.", eType);
			return false;
		}

		T* pDatabase = new T;
		if (!pDatabase->Initialize ())
		{
			//_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is failed initialize.", eType);
			delete pDatabase;
			return false;
		}

		m_Database = pDatabase;
		return true;
	}

	// �����ϴ� �����ͺ��̽� ���� üũ
	void CheckDatabase ();
	// ������ �������� �Ǵ�
	bool IsSuccessQuery (int retSQL);
};
