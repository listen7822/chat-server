#pragma once

namespace LOBBY {
	static const int MAX_SESSION_COUNT = 300;
	static const int MAX_ROOM_COUNT = 100;
	static const int MAX_THREAD_POOL_SIZE = 8;
	static const int SERVER_PORT = 31500;
};

namespace ROOM {
	static const int MAX_USER_COUNT = 5;
	static const int MAX_MESSAGE_QUEUE_SIZE = 5;
	static const int CHAT_RESET_TIME = 60;
	static const int MAX_CHAT_COUNT_PER_MIN = 30;
};
