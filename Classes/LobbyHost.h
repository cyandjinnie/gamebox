#ifndef __LOBBY_HOST_H__
#define __LOBBY_HOST_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "NetworkError.h"
#include "NetworkUtility.h"
#include "LinkUnit.h"

using namespace cocos2d;

class LobbyHost : public cocos2d::Layer
{
public:
	// exit 'server_thread' and 'accept_socket'
	std::atomic<bool> quit_notify;
	std::atomic<bool> quit_accept;
	std::future<SOCKET> accept_socket;
	std::future<void> notify_thread;

	cocos2d::ui::ListView* LogView;
	cocos2d::ui::TextField* InputWindow;
	ssize_t readyStatusIndex;
	bool clientConnected;
	bool opponentIsReady;
	bool lobbyInitialized;

	LinkUnit linkCommunication;

	std::function<void(Ref* pSender)> button_onClick;

	// Callbacks
	void SendChatMessage(cocos2d::Ref* pSender);
	void GoBack(cocos2d::Ref* pSender);
	void Start(cocos2d::Ref* pSender);
	void MessageHandler(MessageType type, const char* message);
	void initLobby(cocos2d::Ref* pSender);
	void setGamemode(cocos2d::Ref* pSender);
	void buttonClickEventListener(cocos2d::Ref* pSender);
	void keyboardEventListener(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event, cocos2d::Ref* _button);
	void enterLobbyName(cocos2d::Ref* pSender);

	// This function is called inside Utility::accept_connections
	void AcceptUser();

	enum PrintMsgType
	{
		NONE,
		READY_STATUS,
		DISCONNECTION
	};

	// Prints to LogView
	void PrintToLog(const char* message, PrintMsgType type, cocos2d::Color3B color = cocos2d::Color3B::WHITE);
	static cocos2d::Scene* createScene();
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(LobbyHost);
};


#endif // __LOBBY_HOST_H__