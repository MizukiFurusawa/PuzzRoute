#pragma execution_character_set("utf-8")
#include "LogoScene.h"
#include "LevelSelectScene.h"
#include "my_debug.h"
USING_NS_CC;

Scene* LogoScene::createScene()
{
    auto scene = Scene::create();
    auto layer = LogoScene::create();
    scene->addChild(layer);
	my_debug::debug_msg(__FILE__, __LINE__, "LogoScene::createScene()");
    return scene;
}

bool LogoScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255)))
	{
		return false; 
	}

	KiesLogo = Sprite::create("logo/KiesLogo.png");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	KiesLogo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	KiesLogo->setOpacity(0);
	KiesLogo->setScale(1.2f);
	this->addChild(KiesLogo);

	auto wait =		CCDelayTime::create(1.0f);
	auto fadeIn =	CCFadeIn::create(0.8f);
	auto scale =	CCScaleTo::create(0.8f, 1.0f);
	auto spawn =	CCSpawn::create(fadeIn, scale, NULL);
	auto endcall =	CCCallFunc::create(this, callfunc_selector(LogoScene::logofinish));
	auto action =	CCSequence::create(wait, spawn, endcall, NULL);

	KiesLogo->runAction(action);

	my_debug::debug_msg(__FILE__, __LINE__, "LogoScene::init()");
	return true;
}

void LogoScene::logofinish() {
	my_debug::debug_msg(__FILE__, __LINE__, "LogoScene::logofinish()");
	Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8f, LevelSelectScene::createScene()));
}
