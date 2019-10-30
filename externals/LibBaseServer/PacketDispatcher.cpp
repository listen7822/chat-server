#include "stdafx.h"
#include "PacketDispatcher.h"
#include "Handler.h"


PacketDispatcher::PacketDispatcher ()
{
	AddHandler (COMMAND_TYPE::WRONG_COMMAND, &Handler::WrongCommand);
}

PacketDispatcher::~PacketDispatcher ()
{
}

bool PacketDispatcher::DoDispatch (int sessionId, const char * pData)
{
	COMMAND_TYPE commandType = GetCommandType (pData);
	auto iter = m_HandlerMap.find (commandType);
	if (iter == m_HandlerMap.end ()) {
		return false;
	}

	const TFunctor& functor = iter->second;
	
	return (*functor.callback)(sessionId, pData);
}

PacketDispatcher::COMMAND_TYPE PacketDispatcher::GetCommandType (const char * pData)
{
	if ('/' == pData[0]) {
	}

	return WRONG_COMMAND;
}

void PacketDispatcher::AddHandler (COMMAND_TYPE commandType, void* handler)
{
	auto iter = m_HandlerMap.find (commandType);
	if (m_HandlerMap.end () != iter)
	{
		std::cout << "Command is already exists." << std::endl;
		return;
	}

	m_HandlerMap.insert (TMessageMap::value_type (commandType, TFunctor ((const TCallback&)handler))).second;
}
