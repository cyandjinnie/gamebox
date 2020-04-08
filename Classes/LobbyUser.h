#ifndef __LOBBY_USER_H__
#define __LOBBY_USER_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "NetworkError.h"
#include "NetworkUtility.h"
#include "GameScene.h"
#include <future>
#include <string>
#include "LinkUnit.h"
#include "NetworkUtility.h"

class LobbyUser : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	cocos2d::ui::ListView* LogView;
	cocos2d::ui::TextField* InputWindow;
	ssize_t readyStatusIndex;
	bool readyToPlay;

	LinkUnit linkCommunication;
	std::function<void(Ref* pSender)> button_onClick;
	
	// Callbacks
	void SendChatMessage(cocos2d::Ref* pSender);
	void GoBack(cocos2d::Ref* pSender);
	void Ready(cocos2d::Ref* pSender);
	void MessageHandler(MessageType type, const char* message);
	void keyboardEventListener(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event);

	enum PrintMsgType
	{
		NONE,
		READY_STATUS,
		DISCONNECTION
	};

	// Prints to LogView
	void PrintToLog(const char* message, PrintMsgType type, cocos2d::Color3B color = cocos2d::Color3B::WHITE);

	CREATE_FUNC(LobbyUser);
};



#endif // __LOBBY_USER_H__