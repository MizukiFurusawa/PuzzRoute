#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class LevelSelectScene : public cocos2d::LayerColor
{
private:

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(LevelSelectScene);
};
