#include "MainMenuScene.h"
#include "NewGameTab.h"
#include "LobbyConnection.h"
#include "LobbyHost.h"
#include <vector>

USING_NS_CC;
// using namespace std;

Scene* MainMenu::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenu::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	////////////////////////////////////
	// Background	

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bgimage = Sprite::create(IMG_MAIN_MENU_BG);
	bgimage->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(bgimage, -1);

	float rX = visibleSize.width / bgimage->getContentSize().width;
	float rY = visibleSize.height / bgimage->getContentSize().height;

	bgimage->setScaleY(rY);
	bgimage->setScaleX(rX);

	///////////////////////////////////////////////////

	// Game title
	auto GameTitle = MenuItemImage::create(IMG_GAME_TITLE, IMG_GAME_TITLE);

	// New game
	auto NewGame_Option = MenuItemImage::create(IMG_NEWGAME_INACTIVE, IMG_NEWGAME_ACTIVE, CC_CALLBACK_1(MainMenu::createNewGame, this));

	// Search
	auto FindGame_Option = MenuItemImage::create(IMG_SEARCH_INACTIVE, IMG_SEARCH_ACTIVE, CC_CALLBACK_1(MainMenu::searchForGames, this));

	// Exit
	auto Exit_Option = MenuItemImage::create(IMG_EXIT_INACTIVE, IMG_EXIT_ACTIVE, CC_CALLBACK_1(MainMenu::menuCloseCallback, this));

	auto menu = Menu::create(GameTitle, NewGame_Option, FindGame_Option, Exit_Option, NULL);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 1 * visibleSize.height / 2));
	menu->alignItemsVerticallyWithPadding(0);

    this->addChild(menu, 1);

	return true;
}


void MainMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end(); 
}

void MainMenu::createNewGame(Ref* pSender) 
{
	UserDefault::getInstance()->setIntegerForKey("UserStatus", Utility::UserStatus::HOST);
	auto newGameScene = LobbyHost::createScene();
	Director::getInstance()->replaceScene(newGameScene);
}

void MainMenu::searchForGames(Ref* pSender)
{
	UserDefault::getInstance()->setIntegerForKey("UserStatus", Utility::UserStatus::CLIENT);
	auto searchGameScene = LobbyConnection::createScene();
	Director::getInstance()->replaceScene(searchGameScene);
}

