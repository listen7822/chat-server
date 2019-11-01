#include "pch.h"
#include <vector>
#include "LobbyPacketDispatcher.h"
#include "ChatHandler.h"


LobbyPacketDispatcher::LobbyPacketDispatcher ()
{
	AddHandler (COMMAND_TYPE::WRONG_COMMAND, &ChatHandler::WrongCommand);
	AddHandler (COMMAND_TYPE::AUTH_TOKEN, &ChatHandler::AuthToken);
	AddHandler (COMMAND_TYPE::ROOM_LIST, &ChatHandler::GetRoomList);
	AddHandler (COMMAND_TYPE::JOIN_ROOM, &ChatHandler::JoinRoomList);
}

LobbyPacketDispatcher::~LobbyPacketDispatcher ()
{
}

void LobbyPacketDispatcher::GetCommandTypeAndParam (COMMAND_TYPE & commandType, std::string & param, const char * pData)
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

	if (AUTH_TOKEN == splits[0]) {
		commandType = COMMAND_TYPE::AUTH_TOKEN;
	}
	else if (ROOM_LIST == splits[0]) {
		commandType = COMMAND_TYPE::ROOM_LIST;
	}
	else if (JOIN_ROOM == splits[0]) {
		commandType = COMMAND_TYPE::JOIN_ROOM;
	}
	else {
		commandType = COMMAND_TYPE::WRONG_COMMAND;
	}

	param = splits[1];
}
