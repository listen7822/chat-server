#pragma once

#include "Singleton.h"

static const int MS_WAIT_PER_TICK = 10;
static const int MAX_DATABASE_COUNT = 10;

class ODBC;
class ODBCDatabase;

class ODBCObject : public Singleton<ODBCObject>
{
	// 연결 Database 목록
	ODBCDatabase* m_Database;

public:
	ODBCObject (void);
	~ODBCObject (void);

	// ODBC 인스턴스 가져오기
	ODBC* GetODBC ();

	// 데이터베이스 추가하는 템플릿함수로써, ODBCPool 을 
	// 사용자 레벨에서 상속받아 구현한 클래스의 타입을 넘겨준다.
	// lib 에서  템플릿 함수를 사용하기 위해 헤더에만 선언.
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

	// 관리하는 데이터베이스 연결 체크
	void CheckDatabase ();
	// 성공한 쿼리인지 판단
	bool IsSuccessQuery (int retSQL);
};
