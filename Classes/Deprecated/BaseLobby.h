#ifndef __BASE_LOBBY_H__
#define __BASE_LOBBY_H__

#include "NetworkUtility.h"

// This is the base class for both LobbyUser and LobbyHost

class BaseLobby
{
public:
	void virtual MessageHandler(MessageType type, const char* message);
};

#endif // __BASE_LOBBY_H__