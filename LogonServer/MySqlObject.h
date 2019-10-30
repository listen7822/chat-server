#pragma once
#include "ODBCObject.h"

class MySqlObject : public ODBCObject
{
public:
	MySqlObject ();
	~MySqlObject ();

	std::string GetUserToken ();
};

