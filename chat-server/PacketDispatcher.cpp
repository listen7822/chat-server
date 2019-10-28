#include "pch.h"
#include "PacketDispatcher.h"
#include "Handler.h"


PacketDispatcher::PacketDispatcher ()
{
	AddHandler (COMMAND_TYPE::WRONG_COMMAND, &Handler::WrongCommand);
}


PacketDispatcher::~PacketDispatcher ()
{
}

bool PacketDispatcher::DoDispatch (Session* pSession, const char * pData)
{
	COMMAND_TYPE commandType = GetCommandType (pData);
	auto iter = m_HandlerMap.find (commandType);
	if (iter == m_HandlerMap.end ()) {
		return false;
	}

	const TFunctor& functor = iter->second;
	
	return (*functor.callback)(pSession, pData);
}


PacketDispatcher::COMMAND_TYPE PacketDispatcher::GetCommandType (const char * pData)
{
	return PacketDispatcher::NONE;
}

void PacketDispatcher::AddHandler (COMMAND_TYPE commandType, void* handler)
{
}
