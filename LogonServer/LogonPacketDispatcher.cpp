#include "pch.h"
#include "Handler.h"
#include "LogonPacketDispatcher.h"

LogonPacketDispatcher::LogonPacketDispatcher ()
{
	AddHandler (COMMAND_TYPE::WRONG_COMMAND, &Handler::WrongCommand);
	AddHandler (COMMAND_TYPE::QUIT, &Handler::QuitServer);
	AddHandler (COMMAND_TYPE::CAHNGE_NICKNAME, &Handler::QuitServer);
}

LogonPacketDispatcher::~LogonPacketDispatcher ()
{
}

void LogonPacketDispatcher::GetCommandTypeAndParam (COMMAND_TYPE& commandType, std::string& param, const char* pData)
{
	if ('/' != pData[0]) {
		commandType = COMMAND_TYPE::WRONG_COMMAND;
		param = pData;
	}

	std::vector<std::string> splits;
	std::string input = pData;
	input.erase (std::remove (input.begin (), input.end (), '/'), input.end ());
	input.erase (std::remove (input.begin (), input.end (), '\r'), input.end ());
	input.erase (std::remove (input.begin (), input.end (), '\n'), input.end ());	char delimiter = ' ';
	std::size_t i = 0;
	std::size_t pos = input.find (delimiter);
	splits.push_back (input.substr (0, pos));
	splits.push_back (input.substr (pos + 1, input.length()));

	const std::string QUIT = "quit";
	const std::string CHANGE_NICKNAME = "nick";
	if (QUIT == splits[0]) {
		commandType = COMMAND_TYPE::QUIT;
	}
	else if (CHANGE_NICKNAME == splits[0]) {
		commandType = COMMAND_TYPE::CAHNGE_NICKNAME;
	}
	else {
		commandType = COMMAND_TYPE::WRONG_COMMAND;
	}

	param = splits[1];
}
