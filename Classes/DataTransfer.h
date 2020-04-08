#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "GameScene.h"
#include "cocos2d.h"

class DataTransfer
{
public:
	static SOCKET Socket;
	static GameScene::UserStatus Status;
	static bool MyTurnFirst;
	static std::string RoomName;
	static std::string NetMask;
};

#endif // __DATA_TRANSFER_H__