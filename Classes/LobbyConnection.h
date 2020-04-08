#ifndef __LOBBY_CONNECTION_H__
#define __LOBBY_CONNECTION_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "NetworkError.h"
#include "NetworkUtility.h"
#include <future>
#include <string>
#include "LobbyUser.h"
#include "MainMenuScene.h"
#include <vector>

class LobbyConnection : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	SOCKET GameSocket;
	std::future<void> refresh_thread;
	bool search_running;
	std::vector<struct server_info> servers;

	cocos2d::ui::Text* StatusLabel;
	cocos2d::Size containerSize;
	
	void RefreshOnClick(cocos2d::Ref* pSender, cocos2d::ui::ScrollView* scrollview);
	void RefreshList(std::vector<server_info> list, cocos2d::ui::ScrollView* scrollview);
	void GoBack(cocos2d::Ref* pSender);
	void Connect(cocos2d::Ref* pSender, int id);

	CREATE_FUNC(LobbyConnection);
};


#endif // __LOBBY_CONNECTION_H__