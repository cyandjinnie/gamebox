#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "NetworkUtility.h"
#include "cocos2d.h"
#include "LinkUnit.h"

class GameScene : public cocos2d::Layer
{
public:

	enum TileStatus
	{
		BLANK,
		CROSS,
		CIRCLE
	};
	enum UserStatus {
		HOST,
		CLIENT
	};
	struct Tile
	{
		TileStatus status;
		cocos2d::ui::Button* ptr;
		Tile()
		{
			status = BLANK;
			ptr = nullptr;
		}
	};

	bool MyTurn;
	bool Finished;
	TileStatus MyTile;
	TileStatus OpponentTile;
	UserStatus currentUserStatus;
	int turnNum;

	std::vector<std::vector<Tile>> GameMatrix;
	cocos2d::ui::Text* statusLabel;

	LinkUnit linkCommunication;

	static cocos2d::Scene* createScene();

	virtual bool init();
	
	struct Coordinate
	{
		u_int i, j;
	};

	Coordinate getCoordinate(const char* _in);
	void makeCoordinate(u_short i, u_short j, std::string& output);
	void resetField(Ref* pSender);

	TileStatus isComplete();
	bool changeTile(u_short i, u_short j, TileStatus type);

	// Callbacks

	void goBack(Ref* pSender);
	void tileClickListener(cocos2d::Ref* pSender, u_short i, u_short j);
	void messageHandler(MessageType type, const char* message);

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);
};

#endif // __GAME_SCENE_H__