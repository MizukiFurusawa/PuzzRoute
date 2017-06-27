#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class LogoScene : public cocos2d::LayerColor
{
private:
	Sprite *CricketLogo;
	Sprite *KiesLogo;
	void logofinish();

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(LogoScene);
};
