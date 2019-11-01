#include "pch.h"
#include <vector>
#include "RoomPacketDispatcher.h"
#include "ChatHandler.h"


RoomPacketDispatcher::RoomPacketDispatcher ()
{
	AddHandler (COMMAND_TYPE::WRONG_COMMAND, &ChatHandler::WrongCommand);
	AddHandler (COMMAND_TYPE::SEND_MESSAGE, &ChatHandler::SendMessage);
	AddHandler (COMMAND_TYPE::EXIT_ROOM, &ChatHandler::ExitRoom);
	AddHandler (COMMAND_TYPE::BROADCASE, &ChatHandler::BroadcastMessage);
}


RoomPacketDispatcher::~RoomPacketDispatcher ()
{
}

void RoomPacketDispatcher::GetCommandTypeAndParam (COMMAND_TYPE & commandType, std::string & param, const char * pData)
{
	if ('/' != pData[0]) {
		commandType = COMMAND_TYPE::WRONG_COMMAND;
		param = pData;
		return;
	}

	std::vector<std::string> splits;
	std::string input = pData;
	input.erase (std::remove (input.begin (), input.end (), '/'), input.end ());
	input.erase (std::remove (input.begin (), input.end (), '\r'), input.end ());
	input.erase (std::remove (input.begin (), input.end (), '\n'), input.end ());	char delimiter = ' ';
	std::size_t i = 0;
	std::size_t pos = input.find (delimiter);
	if (std::string::npos == pos) {
		splits.push_back (input.substr (0, pos));
		splits.push_back ("");
	} else {
		splits.push_back (input.substr (0, pos));
		splits.push_back (input.substr (pos + 1, input.length ()));
	}

	const std::string AUTH_TOKEN = "auth";
	const std::string ROOM_LIST = "list";
	const std::string JOIN_ROOM = "join";
	const std::string EXIT_ROOM = "exit";
	const std::string BROADCASE = "gb";

	if (AUTH_TOKEN == splits[0]) {
		commandType = COMMAND_TYPE::AUTH_TOKEN;
	} else if (ROOM_LIST == splits[0]) {
		commandType = COMMAND_TYPE::ROOM_LIST;
	} else if (JOIN_ROOM == splits[0]) {
		commandType = COMMAND_TYPE::JOIN_ROOM;
	} else if (EXIT_ROOM == splits[0]) {
		commandType = COMMAND_TYPE::EXIT_ROOM;
	} else if (BROADCASE == splits[0]) {
		commandType = COMMAND_TYPE::BROADCASE;
	} else {
		commandType = COMMAND_TYPE::WRONG_COMMAND;
	}

	param = splits[1];
}
