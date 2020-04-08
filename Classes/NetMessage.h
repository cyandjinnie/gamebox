#ifndef __NET_MESSAGE__
#define __NET_MESSAGE__

#include <string>

enum MessageType
{
	UNDEFINED,
	SEARCH,
	CONNECTION_CLOSED,
	REPLY_TO_SEARCH,
	READY,
	NOT_READY,
	CHAT_MESSAGE,
	ACTION,
	GAME_INIT,
	RESET_FIELD
};

struct NetMessage
{
	std::string msg_text;
	MessageType msg_type;
	NetMessage();
	NetMessage(const char*, MessageType);
};

#endif

