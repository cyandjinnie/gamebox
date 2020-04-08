#ifndef __NEW_GAME_TAB_H__
#define __NEW_GAME_TAB_H__

#include "cocos2d.h"
#include "LobbyHost.h"
#include "LobbyConnection.h"
#include "MainMenuScene.h"
#include "Composition.h"

USING_NS_CC;

using namespace cocos2d::ui;

class NewGameTab : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void cancelChoice(cocos2d::Ref* pSender);

	void buttonPressed(cocos2d::Ref* pSender, TextField* textField);
	// implement the "static create()" method manually
	CREATE_FUNC(NewGameTab);
};

#endif // __NEW_GAME_TAB_H__
