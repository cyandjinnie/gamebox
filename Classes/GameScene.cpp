#include "GameScene.h"
#include "Composition.h"
#include "NetMessage.h"
#include "NetworkUtility.h"
#include "time.h"
#include "MainMenuScene.h"
#include "DataTransfer.h"

using namespace cocos2d;
using namespace cocos2d::ui;

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();

	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	this->MyTurn = false;

	this->GameMatrix = std::vector<std::vector<Tile>>(3, std::vector<Tile>(3));

	linkCommunication.init(
		DataTransfer::Socket,
		CC_CALLBACK_2(GameScene::messageHandler, this)
	);

	this->currentUserStatus = DataTransfer::Status;

	if (DataTransfer::MyTurnFirst)
	{
		this->MyTurn = true;
		this->MyTile = CROSS;
		this->OpponentTile = CIRCLE;
	}
	else
	{
		this->MyTurn = false;
		this->MyTile = CIRCLE;
		this->OpponentTile = CROSS;
	}

	this->turnNum = 0;

	/////////////////////////////////
	// Background 

	auto bgimage = Sprite::create(IMG_MAIN_MENU_BG);
	bgimage->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(bgimage, -1);

	float rX = visibleSize.width / bgimage->getContentSize().width;
	float rY = visibleSize.height / bgimage->getContentSize().height;

	bgimage->setScaleY(rY);
	bgimage->setScaleX(rX);

	/////////////////////////////////

	auto GameField = Layout::create();
	GameField->setContentSize(Size(visibleSize.height * 2 / 3, visibleSize.height * 2 / 3));
	GameField->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	GameField->setBackGroundColor(DARK_BLUE_COLOR);
	GameField->setPosition(Vec2(visibleSize.width / 2 - GameField->getContentSize().width / 2, visibleSize.height / 2 - GameField->getContentSize().height / 2));

	this->addChild(GameField, 0);

	this->statusLabel = Text::create("Waiting...", "calibri.ttf", Utility::getTitleFontSize());
	this->statusLabel->setPosition(Vec2(
		visibleSize.width / 2,
		GameField->getPosition().y + GameField->getContentSize().height + statusLabel->getContentSize().height
	));

	auto ResetButton = MenuItemImage::create(IMG_REFRESH_INACTIVE, IMG_REFRESH_ACTIVE, CC_CALLBACK_1(GameScene::resetField, this));

	auto CancelButton = MenuItemImage::create(IMG_CANCEL_INACTIVE, IMG_CANCEL_ACTIVE, CC_CALLBACK_1(GameScene::goBack, this));

	auto Menu = Menu::create(CancelButton, ResetButton, NULL);
	Menu->alignItemsHorizontallyWithPadding(0);
	Menu->setPosition(Vec2(
		visibleSize.width / 2,
		ResetButton->getContentSize().height / 2
	));
	this->addChild(Menu, 4);


	if (this->MyTurn)
	{
		statusLabel->setString("Your turn");
	}
	else
	{
		statusLabel->setString("Your opponent's turn");
	}

	this->addChild(statusLabel, 2);

	// A gap between tiles
	double gap_size = GameField->getContentSize().width * 0.02;
	double tile_size = (GameField->getContentSize().width - 2 * gap_size) / 3;

	for (int i = 0; i < GameMatrix.size(); i++)
		for (int j = 0; j < GameMatrix.size(); j++)
		{
			GameMatrix[i][j].status = TileStatus::BLANK;
			auto button = Button::create(IMG_BLANK_TILE, IMG_BLANK_TILE);
			button->setPosition(Vec2(
				(i)*tile_size + tile_size / 2 + i*gap_size,
				(j)*tile_size + tile_size / 2 + j*gap_size
			));
			button->setScale(tile_size / button->getContentSize().width);
			button->addClickEventListener(CC_CALLBACK_1(GameScene::tileClickListener, this, i, j));
			button->setContentSize(Size(tile_size, tile_size));

			GameField->addChild(button, 1);

			GameMatrix[i][j].ptr = button;
		}

	return true;
}

void GameScene::tileClickListener(Ref* pSender, unsigned short i, unsigned short j)
{
	if (this->MyTurn)
	{
		if (!this->Finished)
		{
			if (this->changeTile(i, j, this->MyTile))
			{
				std::string buf;
				this->turnNum++;
				this->makeCoordinate(i, j, buf);
				linkCommunication.sendMsg(ACTION, buf.c_str());

				this->MyTurn = false;
				statusLabel->setString("Your opponent's turn");

				auto type = this->isComplete();

				switch (type)
				{
				case CROSS:
					this->Finished = true;
					this->statusLabel->setString("Crosses won!");
					break;
				case CIRCLE:
					this->Finished = true;
					this->statusLabel->setString("Circles won!");
					break;
				case BLANK:
					if (this->turnNum >= 9)
					{
						this->statusLabel->setString("Draw!");
						this->Finished = true;
					}
					break;
				}
			}
		}
	}
}

bool GameScene::changeTile(unsigned short i, unsigned short j, TileStatus type)
{
	if (GameMatrix[i][j].status == BLANK)
	{
		switch (type)
		{
		case CROSS:
			GameMatrix[i][j].ptr->loadTextures(IMG_CROSS_TILE, IMG_CROSS_TILE);
			GameMatrix[i][j].status = CROSS;
			break;
		case CIRCLE:
			GameMatrix[i][j].ptr->loadTextures(IMG_CIRCLE_TILE, IMG_CIRCLE_TILE);
			GameMatrix[i][j].status = CIRCLE;
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void GameScene::messageHandler(MessageType type, const char* message)
{
	std::string buf(message);

	switch (type)
	{
	case ACTION:
		if (!this->Finished)
		{
			auto point = this->getCoordinate(message);
			this->changeTile(point.i, point.j, this->OpponentTile);
			this->MyTurn = true;
			this->turnNum++;
			statusLabel->setString("Your turn");

			auto type = this->isComplete();

			switch (type)
			{
			case CROSS:
				this->Finished = true;
				this->statusLabel->setString("Crosses won!");
				break;
			case CIRCLE:
				this->Finished = true;
				this->statusLabel->setString("Circles won!");
				break;
			case BLANK:
				if (this->turnNum >= 9)
				{
					this->statusLabel->setString("Draw!");
					this->Finished = true;
				}
				break;
			}
		}
		break;
	case CONNECTION_CLOSED:
		closesocket(linkCommunication.getSocket());
		linkCommunication.destroy();
		WSACleanup();
		Director::getInstance()->replaceScene(MainMenu::createScene());
		break;
	case RESET_FIELD:
		this->resetField(NULL);
	}
}

GameScene::Coordinate GameScene::getCoordinate(const char* _in)
{
	// example coordinate — "1-1"

	std::string buf = _in;

	u_int field_num = 1;

	std::string i = "", j = "";

	for (auto n : buf)
	{
		if (n == '-')
		{
			field_num++;
		}
		else
		{
			if (field_num == 1)
				i += n;
			else if (field_num == 2)
				j += n;
		}
	}

	if (i == "" || j == "")
		throw network_error(0, " Something went wrong at GameScene::getCoordinate() ");

	return {
		(unsigned int)atoi(i.c_str()),
		(unsigned int)atoi(j.c_str())
	};
}

void GameScene::makeCoordinate(unsigned short i, unsigned short j, std::string& output)
{
	output = std::to_string(i) + '-' + std::to_string(j);
}

GameScene::TileStatus GameScene::isComplete()
{
	if (
		(GameMatrix[0][0].status == TileStatus::CIRCLE && GameMatrix[1][0].status == TileStatus::CIRCLE && GameMatrix[2][0].status == TileStatus::CIRCLE) ||
		(GameMatrix[0][1].status == TileStatus::CIRCLE && GameMatrix[1][1].status == TileStatus::CIRCLE && GameMatrix[2][1].status == TileStatus::CIRCLE) ||
		(GameMatrix[0][2].status == TileStatus::CIRCLE && GameMatrix[1][2].status == TileStatus::CIRCLE && GameMatrix[2][2].status == TileStatus::CIRCLE) ||

		(GameMatrix[0][0].status == TileStatus::CIRCLE && GameMatrix[0][1].status == TileStatus::CIRCLE && GameMatrix[0][2].status == TileStatus::CIRCLE) ||
		(GameMatrix[1][0].status == TileStatus::CIRCLE && GameMatrix[1][1].status == TileStatus::CIRCLE && GameMatrix[1][2].status == TileStatus::CIRCLE) ||
		(GameMatrix[2][0].status == TileStatus::CIRCLE && GameMatrix[2][1].status == TileStatus::CIRCLE && GameMatrix[2][2].status == TileStatus::CIRCLE) ||

		(GameMatrix[0][0].status == TileStatus::CIRCLE && GameMatrix[1][1].status == TileStatus::CIRCLE && GameMatrix[2][2].status == TileStatus::CIRCLE) ||
		(GameMatrix[0][2].status == TileStatus::CIRCLE && GameMatrix[1][1].status == TileStatus::CIRCLE && GameMatrix[2][0].status == TileStatus::CIRCLE))
	{
		return TileStatus::CIRCLE;
	}
	else if (
		(GameMatrix[0][0].status == TileStatus::CROSS && GameMatrix[1][0].status == TileStatus::CROSS && GameMatrix[2][0].status == TileStatus::CROSS) ||
		(GameMatrix[0][1].status == TileStatus::CROSS && GameMatrix[1][1].status == TileStatus::CROSS && GameMatrix[2][1].status == TileStatus::CROSS) ||
		(GameMatrix[0][2].status == TileStatus::CROSS && GameMatrix[1][2].status == TileStatus::CROSS && GameMatrix[2][2].status == TileStatus::CROSS) ||

		(GameMatrix[0][0].status == TileStatus::CROSS && GameMatrix[0][1].status == TileStatus::CROSS && GameMatrix[0][2].status == TileStatus::CROSS) ||
		(GameMatrix[1][0].status == TileStatus::CROSS && GameMatrix[1][1].status == TileStatus::CROSS && GameMatrix[1][2].status == TileStatus::CROSS) ||
		(GameMatrix[2][0].status == TileStatus::CROSS && GameMatrix[2][1].status == TileStatus::CROSS && GameMatrix[2][2].status == TileStatus::CROSS) ||

		(GameMatrix[0][0].status == TileStatus::CROSS && GameMatrix[1][1].status == TileStatus::CROSS && GameMatrix[2][2].status == TileStatus::CROSS) ||
		(GameMatrix[0][2].status == TileStatus::CROSS && GameMatrix[1][1].status == TileStatus::CROSS && GameMatrix[2][0].status == TileStatus::CROSS))
	{
		return TileStatus::CROSS;
	}

	return GameScene::TileStatus::BLANK;
}

void GameScene::resetField(Ref* pSender)
{
	if (this->Finished)
	{
		for (int i = 0; i < GameMatrix.size(); i++)
			for (int j = 0; j < GameMatrix.size(); j++)
			{
				GameMatrix[i][j].ptr->loadTextures(IMG_BLANK_TILE, IMG_BLANK_TILE);
				GameMatrix[i][j].status = BLANK;
			}
		linkCommunication.sendMsg(RESET_FIELD, "RESET");
		if (DataTransfer::MyTurnFirst)
		{
			this->MyTurn = true;
			this->statusLabel->setString("Your turn");
		}
		else
		{
			this->MyTurn = false;
			this->statusLabel->setString("Your opponent's turn");
		}
		this->Finished = false;
		this->turnNum = 0;
	}
}

void GameScene::goBack(Ref* pSender)
{
	closesocket(linkCommunication.getSocket());
	linkCommunication.destroy();
	WSACleanup();
	Director::getInstance()->replaceScene(MainMenu::createScene());
}