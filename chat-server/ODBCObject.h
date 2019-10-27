#ifndef __ODBC_OBJECT__
#define __ODBC_OBJECT__

static const int MS_WAIT_PER_TICK = 10;
static const byte MAX_DATABASE_COUNT = 10;

class ODBC;
class ODBCDatabase;

class ODBCObject
{
	// 연결 Database 목록
	ODBCDatabase* mDatabases[MAX_DATABASE_COUNT];

protected:
	// 싱글톤 인스턴스 포인터
	static ODBCObject* mpThis;

public:
	ODBCObject (void);
	~ODBCObject (void);

	// ODBC 인스턴스 가져오기
	ODBC* GetODBC (DB_TYPE eType, int msTimeOut);

	// 데이터베이스 추가하는 템플릿함수로써, ODBCPool 을 
	// 사용자 레벨에서 상속받아 구현한 클래스의 타입을 넘겨준다.
	// lib 에서  템플릿 함수를 사용하기 위해 헤더에만 선언.
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


	// 관리하는 데이터베이스 연결 체크
	void CheckDatabase ();
	// 성공한 쿼리인지 판단
	bool IsSuccessQuery (int retSQL);

	// 싱글톤 인스턴스 가져오기
	static ODBCObject* GetInstance ()
	{
		return mpThis;
	}
	// 싱글톤 인스턴스 세팅
	static ODBCObject* SetInstance (ODBCObject* pObject)
	{
		mpThis = pObject;
		return mpThis;
	}

};

#endif