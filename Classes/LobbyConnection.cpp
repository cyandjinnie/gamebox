#include "LobbyConnection.h"
#include "LobbyUser.h"

using namespace cocos2d;
using namespace cocos2d::ui;

Scene* LobbyConnection::createScene()
{
	auto scene = Scene::create();
	auto layer = LobbyConnection::create();
	scene->addChild(layer);
	return scene;
}

bool LobbyConnection::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	//////////////////////////////
	// Background

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bgimage = Sprite::create(IMG_MAIN_MENU_BG);
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

	// A var to stop user from abusing 'refresh' button
	this->search_running = false;

	auto UserStatus = UserDefault::getInstance()->getIntegerForKey("UserStatus");
	auto RoomName = UserDefault::getInstance()->getStringForKey("RoomName");


	/////////////////////////////////
	// Interface

	auto titlelabel = Text::create("Connect to lobby", "calibri.ttf", 64);

	containerSize = Size(visibleSize.width / 2, visibleSize.height / 1.4);

	if (visibleSize.width / 2 < 900)
	{
		containerSize.width = visibleSize.width;
		containerSize.height = visibleSize.height;
	}

	titlelabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + containerSize.height / 2 + 50));
	this->addChild(titlelabel, 2);

	//////////////////////////////////////////////////////
	// Container to hold menu and scrollview
	auto MainLayout = Layout::create();
	MainLayout->setPosition(Vec2(origin.x + visibleSize.width / 2 - containerSize.width / 2, origin.y +  visibleSize.height / 2 - containerSize.height / 2));
	MainLayout->setContentSize(containerSize);
	MainLayout->setBackGroundColor(BLUE_COLOR);
	MainLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	this->addChild(MainLayout, 1);

	///////////////////////////////////////////////////////////////
	// Menu

	// This button is used as the example size
	auto connectButton = MenuItemImage::create(IMG_CONNECT_INACTIVE, IMG_CONNECT_ACTIVE);

	// A button size ^^^
	auto BtnSize = connectButton->getContentSize();
	

	////////////////////////////////////////////////////////
	// A text above the LobbyList and its container sprite

	auto LobbyTitleSprite = Sprite::create(IMG_WIDGET_BG);
	LobbyTitleSprite->setContentSize(Size(containerSize.width - BtnSize.width, containerSize.height * 0.15));
	LobbyTitleSprite->setPosition(Vec2((BtnSize.width + containerSize.width) / 2, containerSize.height * 0.925));
	MainLayout->addChild(LobbyTitleSprite, 1);

	auto LobbyTitle = Text::create("Lobby list", "calibri.ttf", Utility::getTitleFontSize());
	LobbyTitle->setPosition(Vec2(LobbyTitleSprite->getContentSize().width / 2,  LobbyTitleSprite->getContentSize().height / 2));
	LobbyTitleSprite->addChild(LobbyTitle, 1);

	//////////////////////////////////////
	// Server list

	auto LobbyList = ScrollView::create();
	LobbyList->setContentSize(Size(containerSize.width - BtnSize.width, containerSize.height * 0.85));
	LobbyList->setBackGroundColor(CYAN_COLOR);
	LobbyList->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	LobbyList->setPosition(Vec2(BtnSize.width, 0));

	MainLayout->addChild(LobbyList, 3);

	/*auto StatusSprite = Sprite::create();
	StatusSprite->setPosition(Vec2(containerSize.width / 4, containerSize.width / 3));
	LobbyList->addChild(StatusSprite, 0);*/

	StatusLabel = Text::create(/*"Lobby list will \n appear here"*/"", "calibri.ttf", Utility::getTitleFontSize());
	StatusLabel->setPosition(Vec2((BtnSize.width  + containerSize.width) / 2, LobbyList->getContentSize().height / 2));
	MainLayout->addChild(StatusLabel, 100);

	//////////////////////////////////////
	// Menu

	auto refreshButton = MenuItemImage::create(IMG_REFRESH_INACTIVE, IMG_REFRESH_ACTIVE, CC_CALLBACK_1(LobbyConnection::RefreshOnClick, this, LobbyList));
	auto cancelButton = MenuItemImage::create(IMG_CANCEL_INACTIVE, IMG_CANCEL_ACTIVE, CC_CALLBACK_1(LobbyConnection::GoBack, this));

	auto LeftMenu = Menu::create(refreshButton, cancelButton, NULL);
	LeftMenu->setPosition(Vec2(BtnSize.width / 2, containerSize.height / 2));
	LeftMenu->alignItemsVerticallyWithPadding(0);
	MainLayout->addChild(LeftMenu, 0);

	this->RefreshOnClick(NULL, LobbyList);

	return true;
}

////////////////////////////////////////////////////////////////////
// The function below is called inside "Utility::refresh_list" thread 
// initialized by RefreshOnClick callback 

void LobbyConnection::RefreshList(std::vector<server_info> list, cocos2d::ui::ScrollView* scrollview)
{
	/*scrollview->setBackGroundColor(Color3B(255, 255, 255), Color3B(100, 100, 100));
	scrollview->setBackGroundColorType(Layout::BackGroundColorType::GRADIENT);*/

	this->search_running = false;
	this->servers = list;

	if (this->servers.size() == 0)
	{
		this->StatusLabel->setString("No lobbies found");
	}
	else
	{
		scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, this->servers.size() * 1200));

		this->StatusLabel->setString("");
		for (u_int i = 0; i < this->servers.size(); i++)
		{
			auto select_button = Button::create(IMG_TEXTFIELD_BG, IMG_WIDGET_BG);
			select_button->addClickEventListener(std::bind(&LobbyConnection::Connect, this, std::placeholders::_1, i));
			select_button->setPosition(Vec2(
				/* Width */ scrollview->getInnerContainerSize().width / 2,
				/* Height */ scrollview->getInnerContainerSize().height - select_button->getContentSize().height * 2 / 3 - i * select_button->getContentSize().height
			));

			if (this->servers[i].name.size() > 10)
			{
				this->servers[i].name.erase(8);
				this->servers[i].name.insert(8, "...");
			}

			select_button->setTitleText(this->servers[i].name);
			select_button->setTitleFontName("calibri.ttf");
			select_button->setTitleFontSize(Utility::getTitleFontSize());
			
			scrollview->addChild(select_button, 0);
		}
	}
}

void LobbyConnection::RefreshOnClick(cocos2d::Ref* pSender, cocos2d::ui::ScrollView* scrollview)
{
	if (!search_running)
	{
		scrollview->removeAllChildren();
		StatusLabel->setString("Searching...");

		search_running = true;
		refresh_thread = std::async(Utility::refresh_list, scrollview, this);
	}
}

void LobbyConnection::GoBack(cocos2d::Ref* pSender)
{
	// Self-explanatory
	if (!search_running)
	{
		WSACleanup();
		UserDefault::getInstance()->setIntegerForKey("TCP_SOCKET", INVALID_SOCKET);
		Director::getInstance()->replaceScene(MainMenu::createScene());
	}
	else
	{
		this->StatusLabel->setString("Search is in \nthe progress!");
	}
}

void LobbyConnection::Connect(cocos2d::Ref* pSender, int id)
{
	this->GameSocket = Utility::send_connection_request(this->servers[id].address);
	
	if (GameSocket != INVALID_SOCKET)
	{
		if (this->search_running)
			this->refresh_thread.get();

		UserDefault::getInstance()->setIntegerForKey("TCP_SOCKET", GameSocket);

		StatusLabel->setString("Waiting for response...");

		//// At this moment client should request information from host
		//// (recv in the next scene)
		//std::string init_message;
		//Utility::encodeMessage(MessageType::INIT_DATA_REQUEST, "///", init_message);
		//send(GameSocket, init_message.c_str(), init_message.size(), 0);

		Director::getInstance()->replaceScene(LobbyUser::createScene());
	}
	else
	{
		this->StatusLabel->setString("!!!ERROR!!!");
	}
}