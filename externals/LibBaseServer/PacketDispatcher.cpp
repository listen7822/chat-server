#include "stdafx.h"
#include <vector>
#include "PacketDispatcher.h"

PacketDispatcher::PacketDispatcher ()
{
}

PacketDispatcher::~PacketDispatcher ()
{
}

bool PacketDispatcher::DoDispatch (boost::shared_ptr<Session> pSession, const char * pData)
{
	COMMAND_TYPE commandType = COMMAND_TYPE::NONE;
	std::string param;
	GetCommandTypeAndParam (commandType, param, pData);
	auto iter = m_HandlerMap.find (commandType);
	if (iter == m_HandlerMap.end ()) {
		return false;
	}

	const TFunctor& functor = iter->second;
	
	return (*functor.callback)(pSession, param);
}

void PacketDispatcher::AddHandler (COMMAND_TYPE commandType, void* handler)
{
	auto iter = m_HandlerMap.find (commandType);
	if (m_HandlerMap.end () != iter)
	{
		BOOST_LOG_TRIVIAL (info) << "Command is already exists.";
		return;
	}

	m_HandlerMap.insert (TMessageMap::value_type (commandType, TFunctor ((const TCallback&)handler))).second;
}
