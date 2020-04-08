#include "LobbyUser.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "DataTransfer.h"

using namespace cocos2d;
using namespace cocos2d::ui;

Scene* LobbyUser::createScene()
{
	auto scene = Scene::create();
	auto layer = LobbyUser::create();
	scene->addChild(layer);
	return scene;
}

bool LobbyUser::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	////////////////////////////////////
	// Background	

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto GameSocket = UserDefault::getInstance()->getIntegerForKey("TCP_SOCKET");

	linkCommunication.init(
		GameSocket,
		CC_CALLBACK_2(LobbyUser::MessageHandler, this)
		);

	auto bgimage = Sprite::create(IMG_MAIN_MENU_BG);
	bgimage->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(bgimage, -1);

	float rX = visibleSize.width / bgimage->getContentSize().width;
	float rY = visibleSize.height / bgimage->getContentSize().height;

	bgimage->setScaleY(rY);
	bgimage->setScaleX(rX);

	/////////////////////////////////
	// Interface

	auto titlelabel = Text::create("Lobby", "calibri.ttf", 64);

	auto containerSize = Size(visibleSize.width / 2, visibleSize.height / 1.4);
	readyStatusIndex = 0;

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

	auto readyButton = MenuItemImage::create(IMG_READY_INACTIVE, IMG_READY_ACTIVE, CC_CALLBACK_1(LobbyUser::Ready, this));
	auto cancelButton = MenuItemImage::create(IMG_CANCEL_INACTIVE, IMG_CANCEL_ACTIVE, CC_CALLBACK_1(LobbyUser::GoBack, this));

	auto LeftMenu = Menu::create(readyButton, cancelButton, NULL);

	auto BtnSize = readyButton->getContentSize();

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

	/*auto InputSprite = Sprite::create(IMG_WIDGET_BG);
	InputSprite->setContentSize(Size(OptionsList->getContentSize().width, OptionsList->getContentSize().height / 12));
	InputSprite->setPosition(Vec2(OptionsList->getContentSize().width / 2, InputSprite->getContentSize().height / 2));

	OptionsList->addChild(InputSprite, 2);*/

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
	SendButton->setScaleX(ButtonSprite->getContentSize().width / SendButton->getContentSize().width);
	SendButton->setScaleY(ButtonSprite->getContentSize().height / SendButton->getContentSize().height);
	SendButton->setPosition(Vec2(ButtonSprite->getContentSize().width / 2, ButtonSprite->getContentSize().height / 2));

	this->button_onClick = CC_CALLBACK_1(LobbyUser::SendChatMessage, this);

	SendButton->addClickEventListener(this->button_onClick);
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
	
	// LogView is a member of LobbyUser
	LogView = ListView::create();
	LogView->setContentSize(Size(containerSize.width - BtnSize.width, containerSize.height - InputWindow->getContentSize().height));
	LogView->setBackGroundColor(Color3B(25, 25, 25));
	LogView->setPosition(Vec2(0, InputWindow->getContentSize().height));
	LogView->setScrollBarAutoHideEnabled(true);
	LogView->setScrollBarAutoHideTime(1);
	LogView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	LogView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);

	PrintToLog("Successfully connected", NONE, Color3B::WHITE);

	OptionsList->addChild(LogView, 0);

	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = CC_CALLBACK_2(LobbyUser::keyboardEventListener, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, InputWindow);


	return true;
}

void LobbyUser::GoBack(Ref* pSender)
{
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

void LobbyUser::Ready(Ref* pSender)
{
	if (linkCommunication.initialized())
	{
		Sprite* btn_active;
		Sprite* btn_inactive;
		if (!readyToPlay)
		{
			btn_active = Sprite::create(IMG_READY_INACTIVE);
			btn_inactive = Sprite::create(IMG_READY_ACTIVE);

			readyToPlay = true;

			int iResult = linkCommunication.sendMsg(MessageType::READY, "");

			PrintToLog(">>> You are ready now!", READY_STATUS, AZURE_COLOR);
		}
		else
		{
			btn_active = Sprite::create(IMG_READY_ACTIVE);
			btn_inactive = Sprite::create(IMG_READY_INACTIVE);

			readyToPlay = false;

			int iResult = linkCommunication.sendMsg(MessageType::NOT_READY, "");

			PrintToLog(">>> You are not ready now!", READY_STATUS, DARK_RED_COLOR);
		}


		((MenuItemImage*)pSender)->setSelectedImage(btn_active);
		((MenuItemImage*)pSender)->setNormalImage(btn_inactive);
	}
}

void LobbyUser::PrintToLog(const char* message, PrintMsgType type, cocos2d::Color3B color)
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

void LobbyUser::SendChatMessage(Ref* pSender)
{
	if (linkCommunication.initialized())
	{
		if (InputWindow->getString() != "")
		{
			// Send a message to host

			std::string buffer;
			Utility::encodeMessage(MessageType::CHAT_MESSAGE, InputWindow->getString().c_str(), buffer);

			if (linkCommunication.sendMsg(buffer.c_str()) < 0)
				throw network_error(WSAGetLastError(), " send() in LobbyUser::SendChatMessage failed ");

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

void LobbyUser::MessageHandler(MessageType type, const char* message)
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
	case MessageType::GAME_INIT:
		std::string buf = message;
		if (buf == "1")
		{
			DataTransfer::MyTurnFirst = true;
		}
		else if (buf == "0")
		{
			DataTransfer::MyTurnFirst = false;
		}

		DataTransfer::Socket = linkCommunication.getSocket();
		DataTransfer::Status = GameScene::UserStatus::CLIENT;
		linkCommunication.destroy();
		Director::getInstance()->replaceScene(GameScene::createScene());
		break;
	}
}

void LobbyUser::keyboardEventListener(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event)
{
	switch (_code)
	{
	case EventKeyboard::KeyCode::KEY_ENTER:
		this->button_onClick(NULL);
		break;
	}
}