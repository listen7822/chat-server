#pragma once
#include "pch.h"
#include <boost/asio.hpp>
#include "ChatServer.h"
#include "Config.h"


int main ()
{
	boost::asio::io_service io_service;
	boost::asio::io_service::work woker (io_service);
	ChatServer server (io_service, LOBBY::SERVER_PORT);
	server.Init (io_service, LOBBY::MAX_SESSION_COUNT, LOBBY::MAX_THREAD_POOL_SIZE, LOBBY::MAX_ROOM_COUNT);
	BOOST_LOG_TRIVIAL (info) << "Chat server start ...";
	server.Start ();

	BOOST_LOG_TRIVIAL (info) << "Closed server.";

	getchar ();
	return 0;
}
