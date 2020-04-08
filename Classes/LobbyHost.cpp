#include "LobbyHost.h"
#include "GameScene.h"
#include <string>
#include <vector>
#include "DataTransfer.h"

USING_NS_CC;
// using namespace std;

using namespace cocos2d::ui;

Scene* LobbyHost::createScene()
{
	auto scene = Scene::create();
	auto layer = LobbyHost::create();
	scene->addChild(layer);
	return scene;
}

bool LobbyHost::init()
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

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0)
		throw network_error(WSAGetLastError(), " WSAStartup() failed ");

	/////////////////////////////////
	// Interface
	
	auto titlelabel = Text::create("Lobby", "calibri.ttf", 64);

	auto containerSize = Size(visibleSize.width / 2, visibleSize.height / 1.4);

	this->clientConnected = false;
	this->opponentIsReady = false;
	this->lobbyInitialized = false;

	this->readyStatusIndex = 0;

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
	MainLayout->setPosition(Vec2(origin.x + visibleSize.width / 2 - containerSize.width / 2, origin.y + visibleSize.height / 2 - containerSize.height / 2));
	MainLayout->setContentSize(containerSize);
	MainLayout->setBackGroundColor(BLUE_COLOR);
	MainLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	this->addChild(MainLayout, 1);

	//////////////////////////////////////
	// Menu

	auto startButton = MenuItemImage::create(IMG_START_INACTIVE, IMG_START_ACTIVE, CC_CALLBACK_1(LobbyHost::Start, this));
	auto cancelButton = MenuItemImage::create(IMG_CANCEL_INACTIVE, IMG_CANCEL_ACTIVE, CC_CALLBACK_1(LobbyHost::GoBack, this));

	auto LeftMenu = Menu::create(startButton, cancelButton, NULL);

	auto BtnSize = startButton->getContentSize();

	LeftMenu->setPosition(Vec2(BtnSize.width / 2, containerSize.height / 2));
	LeftMenu->alignItemsVerticallyWithPadding(0);
	MainLayout->addChild(LeftMenu, 0);

	//////////////////////////////////////
	// Right menu + chat

	auto OptionsList = Layout::create();
	OptionsList->setContentSize(Size(containerSize.width - BtnSize.width, containerSize.height));
	OptionsList->setBackGroundColor(Color3B::BLACK);
	OptionsList->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	OptionsList->setPosition(Vec2(BtnSize.width, 0));

	MainLayout->addChild(OptionsList, 3);

	/////////////////////////////////////
	// Input window

	// This sprite holds a TextField inside
	auto TextFieldHolder = Sprite::create(IMG_WIDGET_BG);
	TextFieldHolder->setContentSize(Size(OptionsList->getContentSize().width, OptionsList->getContentSize().height / 12));
	TextFieldHolder->setPosition(Vec2(OptionsList->getContentSize().width / 2, TextFieldHolder->getContentSize().height / 2));
	OptionsList->addChild(TextFieldHolder, 2);

	auto textHolderSize = TextFieldHolder->getContentSize();

	auto ButtonSprite = Sprite::create(IMG_BUTTON_INACTIVE);
	ButtonSprite->setContentSize(Size(textHolderSize.width / 5, textHolderSize.height));
	ButtonSprite->setPosition(Vec2(textHolderSize.width * 9 / 10, textHolderSize.height / 2));
	TextFieldHolder->addChild(ButtonSprite, 10);

	auto SendButton = Button::create(IMG_SEND_INACTIVE, IMG_SEND_ACTIVE);
	SendButton->setTitleColor(Color3B::WHITE);
	SendButton->setContentSize(ButtonSprite->getContentSize());
	SendButton->setPosition(Vec2(ButtonSprite->getContentSize().width / 2, ButtonSprite->getContentSize().height / 2));
	SendButton->setScaleX(ButtonSprite->getContentSize().width / SendButton->getContentSize().width);
	SendButton->setScaleY(ButtonSprite->getContentSize().height / SendButton->getContentSize().height);
	SendButton->addClickEventListener(CC_CALLBACK_1(LobbyHost::buttonClickEventListener, this));

	ButtonSprite->addChild(SendButton, 1);

	auto textFieldSize = Size(textHolderSize.width - ButtonSprite->getContentSize().width, textHolderSize.height);

	InputWindow = TextField::create(">> Your message", "calibri.ttf", Utility::getMinorFontSize());
	InputWindow->setMaxLengthEnabled(true);
	InputWindow->setMaxLength(40);
	InputWindow->setContentSize(Size(textFieldSize.width, textFieldSize.height));
	InputWindow->setPosition(Vec2(textFieldSize.width / 2, textFieldSize.height / 2));
	InputWindow->setTextVerticalAlignment(TextVAlignment::CENTER);
	InputWindow->setTextHorizontalAlignment(TextHAlignment::CENTER);
	InputWindow->ignoreContentAdaptWithSize(false);
	InputWindow->addClickEventListener(
		[](Ref* pSender)
		{
			((TextField*)pSender)->setString("");
		}
	);

	TextFieldHolder->addChild(InputWindow, 0);

	// LogView is a member of LobbyHost
	LogView = ListView::create();
	LogView->setContentSize(Size(containerSize.width - BtnSize.width, containerSize.height - InputWindow->getContentSize().height));
	LogView->setBackGroundColor(Color3B(25, 25, 25));
	LogView->setPosition(Vec2(0, InputWindow->getContentSize().height));
	LogView->setScrollBarAutoHideEnabled(true);
	LogView->setScrollBarAutoHideTime(1);
	LogView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	LogView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);

	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = CC_CALLBACK_2(LobbyHost::keyboardEventListener, this, SendButton);
	this->button_onClick = CC_CALLBACK_1(LobbyHost::enterLobbyName, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, InputWindow);

	OptionsList->addChild(LogView, 0);

	PrintToLog("Enter your lobby name: ", NONE, Color3B::WHITE);

	return true;
}

void LobbyHost::GoBack(cocos2d::Ref* pSender)
{
	if (!clientConnected && this->lobbyInitialized)
	{
		quit_accept = true;
		quit_notify = true;

		notify_thread.get();
		SOCKET res = accept_socket.get();
	}

	if (linkCommunication.initialized())
	{
		NetMessage msg;
		msg.msg_text = "";
		msg.msg_type = MessageType::CONNECTION_CLOSED;

		std::string buffer;
		Utility::encodeMessage(msg, buffer);

		linkCommunication.sendMsg(buffer.c_str());
		linkCommunication.destroy();
	}

	WSACleanup();
	UserDefault::getInstance()->setIntegerForKey("TCP_SOCKET", INVALID_SOCKET);
	Director::getInstance()->replaceScene(MainMenu::createScene());
}

void LobbyHost::PrintToLog(const char* message, PrintMsgType type, cocos2d::Color3B color)
{
	std::string msg = message;
	msg.insert(0, " ");


	auto Label = Text::create();
	Label->setFontName("calibri.ttf");
	Label->setColor(color);
	Label->setFontSize(Utility::getMinorFontSize());

	if (type == PrintMsgType::DISCONNECTION)
		Label->setFontSize(Utility::getTitleFontSize());

	Label->setString(msg);
	Label->ignoreContentAdaptWithSize(false);
	Label->setTextHorizontalAlignment(TextHAlignment::LEFT);

	LogView->pushBackCustomItem(Label);

	if (type == PrintMsgType::READY_STATUS)
	{
		if (readyStatusIndex != 0)
			LogView->removeItem(readyStatusIndex);

		readyStatusIndex = LogView->getIndex(Label);
	}


	LogView->jumpToBottom();
}

void LobbyHost::SendChatMessage(Ref* pSender)
{
	if (linkCommunication.initialized())
	{
		if (InputWindow->getString() != "")
		{
			// Send a message to host

			std::string buffer;
			Utility::encodeMessage(MessageType::CHAT_MESSAGE, InputWindow->getString().c_str(), buffer);

			if (linkCommunication.sendMsg(buffer.c_str()) < 0)
				throw network_error(WSAGetLastError(), " send() in LobbyHost::SendChatMessage failed ");

			// Print your message to log

			buffer = InputWindow->getString();

			buffer.insert(0, "[You] ");
			PrintToLog(buffer.c_str(), PrintMsgType::NONE, GOLDEN_COLOR);

			InputWindow->setString("");
		}
	}
	else
	{
		
		InputWindow->setString("There is no connection!");
	}
}

void LobbyHost::Start(Ref* pSender)
{
	if (this->linkCommunication.initialized())
	{
		if (this->opponentIsReady)
		{
			DataTransfer::Socket = linkCommunication.getSocket();
			DataTransfer::Status = GameScene::UserStatus::HOST;

			srand(time(nullptr));
			int Res = rand() % 2;

			if (Res == 0)
			{
				linkCommunication.sendMsg(GAME_INIT, "0");
				DataTransfer::MyTurnFirst = true;
			}
			else
			{
				linkCommunication.sendMsg(GAME_INIT, "1");
				DataTransfer::MyTurnFirst = false;
			}
			linkCommunication.destroy();
			Director::getInstance()->replaceScene(GameScene::createScene());
		}
		else
		{
			PrintToLog(">>> Your opponent isn't ready", PrintMsgType::READY_STATUS, DARK_RED_COLOR);
		}
	}
	
}

void LobbyHost::AcceptUser()
{
	auto GameSocket = accept_socket.get();

	quit_accept = true;
	quit_notify = true;
	notify_thread.get();

	if (GameSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " accept_socket thread failed in Utility::accept_connections && LobbyHost ");

	PrintToLog(">> Client has connected!", PrintMsgType::NONE);

	this->clientConnected = true;

	linkCommunication.init(
		GameSocket,
		CC_CALLBACK_2(LobbyHost::MessageHandler, this)
		);
}

void LobbyHost::MessageHandler(MessageType type, const char* message)
{
	std::string text = message;

	switch (type)
	{
	case MessageType::CHAT_MESSAGE:
		text.insert(0, "[Opponent] ");
		PrintToLog(text.c_str(), NONE, CYAN_COLOR);
		break;
	case MessageType::CONNECTION_CLOSED:
		PrintToLog(" >>> Your opponent has left", DISCONNECTION, DARK_RED_COLOR);
		closesocket(linkCommunication.getSocket());
		WSACleanup();
		linkCommunication.destroy();
		//this->GoBack(this);
		break;
	case MessageType::READY:
		PrintToLog(">>> Your opponent is ready", PrintMsgType::READY_STATUS, AZURE_COLOR);
		this->opponentIsReady = true;
		break;
	case MessageType::NOT_READY:
		PrintToLog(">>> Your opponent isn't ready", PrintMsgType::READY_STATUS, DARK_RED_COLOR);
		this->opponentIsReady = false;
		break;
	}
}

void LobbyHost::initLobby(Ref* pSender)
{
	quit_notify = false;
	quit_accept = false;

	notify_thread = std::async(Utility::notify_clients, std::ref(quit_notify));
	accept_socket = std::async(Utility::accept_connections, std::ref(quit_accept), this);

	this->lobbyInitialized = true;
}

void LobbyHost::keyboardEventListener(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event, cocos2d::Ref* _button)
{
	switch (_code)
	{
	case EventKeyboard::KeyCode::KEY_ENTER:
		this->button_onClick(this);
		break;
	}
}

void LobbyHost::enterLobbyName(Ref* pSender)
{
	if (InputWindow->getString() != "")
	{
		std::string buf = InputWindow->getString();

		PrintToLog(buf.c_str(), NONE, Color3B::WHITE);

		UserDefault::getInstance()->setStringForKey("RoomName", buf);
		buf = "Initialized lobby as '" + buf + "'.";
		PrintToLog(buf.c_str(), NONE, Color3B::WHITE);
		PrintToLog("Waiting for connections...", NONE, Color3B::WHITE);
		
		this->button_onClick = 0;
		this->button_onClick = CC_CALLBACK_1(LobbyHost::SendChatMessage, this);
		this->initLobby(this);

		this->InputWindow->setString("");
	}
}

void LobbyHost::buttonClickEventListener(Ref* pSender)
{
	this->keyboardEventListener(EventKeyboard::KeyCode::KEY_ENTER, NULL, NULL);
}

void LobbyHost::setGamemode(Ref* pSender)
{

}