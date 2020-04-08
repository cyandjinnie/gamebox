#include "NewGameTab.h"

using namespace cocos2d::ui;
using namespace cocos2d;

Scene* NewGameTab::createScene()
{
	auto scene = Scene::create();
	auto layer = NewGameTab::create();
	scene->addChild(layer);
	return scene;
}

bool NewGameTab::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	///////////////////////////
	// Background

	auto bgimage = Sprite::create(IMG_MAIN_MENU_BG);
	bgimage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(bgimage, -1);

	float rX = visibleSize.width / bgimage->getContentSize().width;
	float rY = visibleSize.height / bgimage->getContentSize().height;

	bgimage->setScaleY(rY);
	bgimage->setScaleX(rX);

	/////////////////////////////////////

	auto widgetSize = Size(300, 450);

	// Status (HOST or CLIENT)
	auto UserStatus = UserDefault::getInstance()->getIntegerForKey("UserStatus");

	//////////////////////////////////////////
	// Textfield

	auto textFieldHolder = Sprite::create(IMG_TEXTFIELD_BG);
	textFieldHolder->setColor(Color3B(255, 255, 255));
	textFieldHolder->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(textFieldHolder);
	auto textFieldSize = textFieldHolder->getContentSize();

	auto textField = TextField::create(">> Your room name", "calibri.ttf", 30);
	textField->setMaxLengthEnabled(true);
	textField->setMaxLength(20);
	textField->setPlaceHolderColor(Color3B(255, 255, 255));
	textField->setContentSize(Size(textFieldSize.width, textFieldSize.width));
	textField->setTextHorizontalAlignment(TextHAlignment::CENTER);
	textField->setTextVerticalAlignment(TextVAlignment::CENTER);
	textField->setPosition(Vec2(textFieldSize.width / 2, textFieldSize.height / 2));
	textFieldHolder->addChild(textField);

	auto titleButton = MenuItemImage::create(IMG_CONTINUE_INACTIVE, IMG_CONTINUE_ACTIVE, CC_CALLBACK_1(NewGameTab::buttonPressed, this, textField));
	auto Menu = Menu::create(titleButton, NULL);
	Menu->setPosition(Vec2(
		textFieldHolder->getPosition().x,
		textFieldHolder->getPosition().y - textFieldHolder->getContentSize().height / 2 - titleButton->getContentSize().height / 2
		));
	this->addChild(Menu);

	return true;
}

void NewGameTab::cancelChoice(cocos2d::Ref* pSender) {
	Director::getInstance()->replaceScene(MainMenu::createScene());
}

void NewGameTab::buttonPressed(cocos2d::Ref* pSender, TextField* textField) {
	// Проверка на ввод имени в 'textField'
	auto buffer = textField->getString();
	if (buffer == "") {
		textField->setPlaceHolder("There is no name!");
	}
	else {
		// RoomName <-> имя комнаты
		UserDefault::getInstance()->setStringForKey("RoomName", textField->getString());
		Director::getInstance()->replaceScene(LobbyHost::createScene());
	}
}
