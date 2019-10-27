#ifndef __ODBC_OBJECT__
#define __ODBC_OBJECT__

static const int MS_WAIT_PER_TICK = 10;
static const byte MAX_DATABASE_COUNT = 10;

class ODBC;
class ODBCDatabase;

class ODBCObject
{
	// ���� Database ���
	ODBCDatabase* mDatabases[MAX_DATABASE_COUNT];

protected:
	// �̱��� �ν��Ͻ� ������
	static ODBCObject* mpThis;

public:
	ODBCObject (void);
	~ODBCObject (void);

	// ODBC �ν��Ͻ� ��������
	ODBC* GetODBC (DB_TYPE eType, int msTimeOut);

	// �����ͺ��̽� �߰��ϴ� ���ø��Լ��ν�, ODBCPool �� 
	// ����� �������� ��ӹ޾� ������ Ŭ������ Ÿ���� �Ѱ��ش�.
	// lib ����  ���ø� �Լ��� ����ϱ� ���� ������� ����.
	template <typename T>
	bool AddDatabase (DB_TYPE eType)
	{
		if (mDatabases[eType] != NULL)
		{
			_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is already exists.", eType);
			return false;
		}

		T* pDatabase = new T (eType);
		if (!pDatabase->Initialize ())
		{
			_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is failed initialize.", eType);
			delete pDatabase;
			return false;
		}

		mDatabases[eType] = pDatabase;
		return true;
	}

	//-- [ORACLE FADE-OUT]
	template <typename T>
	bool AddDatabase (DB_TYPE eType, bool bAutoCommit)
	{
		if (mDatabases[eType] != NULL)
		{
			_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is already exists.", eType);
			return false;
		}


		T* pDatabase = new T (eType);
		if (!pDatabase->Initialize (bAutoCommit))
		{
			_Logf (GLog::LL_ERROR, "[ODBC] DB Type(%d) is failed initialize.", eType);
			delete pDatabase;
			return false;
		}

		mDatabases[eType] = pDatabase;
		return true;
	}


	// �����ϴ� �����ͺ��̽� ���� üũ
	void CheckDatabase ();
	// ������ �������� �Ǵ�
	bool IsSuccessQuery (int retSQL);

	// �̱��� �ν��Ͻ� ��������
	static ODBCObject* GetInstance ()
	{
		return mpThis;
	}
	// �̱��� �ν��Ͻ� ����
	static ODBCObject* SetInstance (ODBCObject* pObject)
	{
		mpThis = pObject;
		return mpThis;
	}

};

#endif