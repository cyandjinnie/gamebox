#include "ConnectionScene.h"
#include <string>
#include <vector>
#include <future>

USING_NS_CC;
// using namespace std;

Scene* ConnectionScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ConnectionScene::create();
	scene->addChild(layer);
	return scene;
}

bool ConnectionScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	// Background

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bgimage = Sprite::create("menutabbg.png");
	bgimage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(bgimage, -1);

	float rX = visibleSize.width / bgimage->getContentSize().width;
	float rY = visibleSize.height / bgimage->getContentSize().height;

	bgimage->setScaleY(rY);
	bgimage->setScaleX(rX);


	////////////////////////////////////////////
	// Winsock init

	WSAData wsaData;
	GameSocket = INVALID_SOCKET;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0)
		throw network_error(WSAGetLastError(), " WSAStartup() failed ");

	////////////////////////////////

	auto UserStatus = UserDefault::getInstance()->getIntegerForKey("UserStatus");
	auto RoomName = UserDefault::getInstance()->getStringForKey("RoomName");

	/////////////////////////////////////////////
	// Thread init

	if (UserStatus == ConnectionScene::_UserStatus::HOST)
	{
		quit_notify = false;
		quit_accept = false;

		server_thread = std::thread(Utility::notify_clients, std::ref(quit_notify));
		accept_thread = std::thread(Utility::accept_connections, std::ref(quit_accept), std::ref(sock));
	}
	else if (UserStatus == ConnectionScene::_UserStatus::CLIENT)
	{

	}

	/////////////////////////////////
	// Interface

	std::string LabelText;

	cocos2d::Label* TitleLabel;

	if (UserStatus == ConnectionScene::_UserStatus::CLIENT)
	{
		LabelText = "Search for games?";
		TitleLabel = Label::create(LabelText.c_str(), "DF Font.ttf", 64);
	}
	else if (UserStatus == ConnectionScene::_UserStatus::HOST)
	{
		LabelText = "Waiting for connections on '" + RoomName + "'";
		TitleLabel = Label::create(LabelText.c_str(), "DF Font.ttf", 64);
	}

	TitleLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 1 * visibleSize.height / 2 + 100));
	this->addChild(TitleLabel, 0);

	cocos2d::MenuItemImage* MainButton;

	if (UserStatus == ConnectionScene::_UserStatus::HOST)
	{
		MainButton = MenuItemImage::create("cancel_inactive.png", "cancel_active.png", CC_CALLBACK_1(ConnectionScene::CancelHost, this));
	}
	else if (UserStatus == ConnectionScene::_UserStatus::CLIENT)
	{
		MainButton = MenuItemImage::create("refresh_inactive.png", "refresh_active.png", std::bind(&ConnectionScene::SearchForServers, this, std::placeholders::_1, TitleLabel));
	}

	auto menu = Menu::create(MainButton, NULL);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

	this->addChild(menu, 1);

	/*if (UserStatus == ConnectionScene::_UserStatus::CLIENT)
	{
		MainButton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
		{
			std::vector<server_entry> server_list;
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:

				MainButton->setTitleText("Searching...");
				server_list = std::async(std::launch::async, Utility::refresh_list).get();

				if (server_list.size() == 0)
					MainButton->setTitleText("Failed. Repeat?");
				else
					MainButton->setTitleText("Ready to connect");
				break;
			default:
				break;
			}
		});
	}
	else if (UserStatus == ConnectionScene::_UserStatus::HOST)
	{
		MainButton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
		{
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				
				break;
			default:
				break;
			}
		});
	}*/
	//if (UserStatus == _UserStatus::HOST) {
	//	std::atomic<bool> quit = false;
	//	auto server_thread = std::thread(Utility::notify_clients, std::ref(quit), RoomName.c_str());
	//
	//	GameSocket = Utility::accept_connections();
	//	if (send(GameSocket, "kek", sizeof("kek"), 0) < 0)
	//	throw network_error(WSAGetLastError(), " send() failed ");
	//
	//	quit = true;
	//	server_thread.join();
	//
	//}
	//else if (UserStatus == _UserStatus::CLIENT) {
	//	//...
	//
	//	
	//	auto server_list = std::vector<server_entry>(0);
	//
	//	SearchButton->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 1 * visibleSize.height / 2));
	//	this->addChild(SearchButton, 1);
	//
	//}

	return true;
}

void ConnectionScene::CancelHost(cocos2d::Ref* pSender)
{

	quit_accept = true;
	quit_notify = true;

	server_thread.join();
	accept_thread.join();
	

	Director::getInstance()->end();

	///////////////////////////////
	//       Former code         //
	///////////////////////////////

	//label->setString("Waiting for connections...");
	//
	//auto RoomName = UserDefault::getInstance()->getStringForKey("RoomName");
	//
	//std::atomic<bool> quit_notify = false;
	//std::atomic<bool> quit_accept = false;
	//std::atomic<SOCKET> sock = INVALID_SOCKET;
	//
	//auto server_thread = std::thread(Utility::notify_clients, std::ref(quit_notify), RoomName.c_str());
	//auto accept_thread = std::thread(Utility::accept_connections, std::ref(quit_accept), std::ref(sock));
	//
	//if (send(GameSocket, "kek", sizeof("kek"), 0) < 0)
	//	throw network_error(WSAGetLastError(), " send() failed ");
	//
	//Director::getInstance()->replaceScene(MainMenu::createScene());
	//
	//quit = true;
	//server_thread.join();
	//
	//// TODO
	//// Create a new scene, ...
}

void ConnectionScene::SearchForServers(cocos2d::Ref* pSender, cocos2d::Label* label)
{
	SOCKET GameSocket = INVALID_SOCKET;

	//label->setString("Searching...");

	auto RoomName = UserDefault::getInstance()->getStringForKey("RoomName");

	auto server_list = std::async(std::launch::async, Utility::refresh_list).get();

	for (auto it = server_list.begin(); it != server_list.end(); it++)
	{
		if (it->name == RoomName)
		{
			sockaddr_in connection_addr;

			inet_pton(AF_INET, it->address.c_str(), &connection_addr.sin_addr);

			connection_addr.sin_family = AF_INET;
			connection_addr.sin_port = htons(DEFAULT_TCP_PORT);

			GameSocket = Utility::send_connection_request(connection_addr);

			if (GameSocket == INVALID_SOCKET)
				throw network_error(WSAGetLastError(), " ConnectionScene.cpp :: GameSocket is INVALID_SOCKET ");

			Director::getInstance()->replaceScene(MainMenu::createScene());

			break;
		}
	}
}