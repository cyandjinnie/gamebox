#ifndef __CONNECTION_SCENE_H__
#define __CONNECTION_SCENE_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "NetworkError.h"
#include "NetworkUtility.h"

class ConnectionScene : public cocos2d::Layer
{
public:

	//////////////////////////////////////////////////////
	//   THESE VARIABLES ARE USED IN SERVER SEQUENCE    //
	//////////////////////////////////////////////////////

	// A variable required to exit 'server_thread'
	std::atomic<bool> quit_notify;

	// A variable required to exit 'accept_thread'
	std::atomic<bool> quit_accept;

	// A variable required to return SOCKET from 'accept_thread'
	std::atomic<SOCKET> sock = INVALID_SOCKET;

	std::thread server_thread;
	std::thread accept_thread;

	//////////////////////////////////////////////////////
	//   THESE VARIABLES ARE USED IN CLIENT SEQUENCE    //
	//////////////////////////////////////////////////////






	enum _UserStatus {
		HOST, 
		CLIENT
	};

	static cocos2d::Scene* createScene();

	virtual bool init();

	SOCKET GameSocket;

	//void CancelCallback(cocos2d::Ref* pSender);

	void SearchForServers(cocos2d::Ref* pSender, cocos2d::Label* label);

	void CancelHost(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(ConnectionScene);
};


#endif // __CONNECTION_SCENE_H__