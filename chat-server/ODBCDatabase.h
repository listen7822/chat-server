#ifndef __ODBC_DATABASE__
#define __ODBC_DATABASE__

static const string HEALTH_CHECK_QUERY_STRING = "select 1;";

// ���� �α� ��� �ֱ� (��)
static const int LOG_TERM = 600;

class GCritSec;

class ODBCDatabase
{
	DB_TYPE mType;
	list<ODBC*> mPool;
	XSystem::Threading::CriticalSection mCS;
public:
	ODBCDatabase (DB_TYPE eType);
	~ODBCDatabase (void);

	// �ν��Ͻ� ���� �� �ʱ�ȭ
	bool Initialize ();
	// ODBC ���� pool ���� �����Ǵ� ODBC �ϳ� ����
	ODBC* GetConnection ();
	// pool ���� �����Ǵ� ��� ODBC Connection üũ
	void CheckConnection ();

	//-- [ORACLE FADE-OUT]
	bool Initialize (bool bAutoCommit);

protected:
	// ODBC ����� ����ϴ� DB Type ��ȯ �Լ� (mysql, oracle, mssql)
	virtual const string GetConfigDBType () = 0;
	// ODBC ����� ����ϴ� DB Driver ��ȯ �Լ�
	virtual const string GetConfigDBDriver () = 0;
	// ODBC ����� ����ϴ� DB IP ��ȯ �Լ�
	virtual const string GetConfigDBIP () = 0;
	// ODBC ����� ����ϴ� DB Name ��ȯ �Լ�
	virtual const string GetConfigDBName () = 0;
	// ODBC ����� ����ϴ� DB ���� ID ��ȯ �Լ�
	virtual const string GetConfigDBID () = 0;
	// ODBC ����� ����ϴ� DB ���� Pwd ��ȯ �Լ�
	virtual const string GetConfigDBPwd () = 0;
	// ODBC ����� ����ϴ� DB ��Ʈ ��ȯ �Լ�
	virtual const int GetConfigDBPort () = 0;
	// ODBC Connection ���� ���� ��ȯ �Լ�
	virtual const int GetConfigDBConnCnt () = 0;
};

#endif