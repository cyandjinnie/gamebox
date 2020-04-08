#ifndef __HOST_SCENE_H__
#define __HOST_SCENE_H__

#include "cocos2d.h"

class OptionsTab : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void goToMenu(Ref* pSender);

	// a selector callback
	//void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(OptionsTab);
};


#endif // __HOST_SCENE_H__