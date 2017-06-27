#pragma execution_character_set("utf-8")
#include "LevelSelectScene.h"
#include "MainGame.h"
#include "my_debug.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
USING_NS_CC;

Scene* LevelSelectScene::createScene()
{
    auto scene = Scene::create();
    auto layer = LevelSelectScene::create();
    scene->addChild(layer);
	my_debug::debug_msg(__FILE__, __LINE__, "LevelSelectScene::createScene()");
    return scene;
}

#define BRIGHT	200
bool LevelSelectScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 255)))
	{
		return false; 
	}

	//画面サイズ取得
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//back
	{
		auto back = Sprite::create("img/back.png");
		back->setAnchorPoint(Vec2(0, 1));
		back->setPosition(0, visibleSize.height);
		this->addChild(back, 0);
	}
	
	//expert
	{
		auto pNormalSprite = Sprite::create("img/expert.png");
		auto pSelectedSprite = Sprite::create("img/expert.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto easy = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[=](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "expertButton was pressed.");
			SimpleAudioEngine::sharedEngine()->playEffect("se/blockmove.wav");
			Director::getInstance()->replaceScene(
				CCTransitionFade::create(
					0.5f,
					MainGame::createScene(3000, LV_T::LV_EXPERT)));
		}
		);
		auto menu = Menu::create(easy, NULL);
		easy->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(0, 330 + 180 * 3));
		this->addChild(menu, 1);
	}

	//hard
	{
		auto pNormalSprite = Sprite::create("img/hard.png");
		auto pSelectedSprite = Sprite::create("img/hard.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto easy = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[=](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "hardButton was pressed.");
			SimpleAudioEngine::sharedEngine()->playEffect("se/blockmove.wav");
			Director::getInstance()->replaceScene(
				CCTransitionFade::create(
					0.5f, 
					MainGame::createScene(1000, LV_T::LV_HARD)));
		}
		);
		auto menu = Menu::create(easy, NULL);
		easy->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(0, 330 + 180 * 2));
		this->addChild(menu, 1);
	}

	//normal
	{
		auto pNormalSprite = Sprite::create("img/normal.png");
		auto pSelectedSprite = Sprite::create("img/normal.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto easy = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[=](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "normalButton was pressed.");
			SimpleAudioEngine::sharedEngine()->playEffect("se/blockmove.wav");
			Director::getInstance()->replaceScene(
				CCTransitionFade::create(
					0.5f, 
					MainGame::createScene(500, LV_T::LV_NORMAL)));
		}
		);
		auto menu = Menu::create(easy, NULL);
		easy->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(0, 330 + 180 * 1));
		this->addChild(menu, 1);
	}

	//easy
	{
		auto pNormalSprite = Sprite::create("img/easy.png");
		auto pSelectedSprite = Sprite::create("img/easy.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto easy = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[=](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "easyButton was pressed.");
			SimpleAudioEngine::sharedEngine()->playEffect("se/blockmove.wav");
			Director::getInstance()->replaceScene(
				CCTransitionFade::create(
					0.5f, 
					MainGame::createScene(150, LV_T::LV_EASY)));
		}
		);
		auto menu = Menu::create(easy, NULL);
		easy->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(0, 330 + 180 * 0));
		this->addChild(menu, 1);
	}


	//title
	{
		auto title = Sprite::create("img/title.png");
		title->setAnchorPoint(Vec2(0,1));
		title->setPosition(Vec2(0,1280));
		this->addChild(title);
	}


	//ranking
	{
		auto pNormalSprite = Sprite::create("img/ranking.png");
		auto pSelectedSprite = Sprite::create("img/ranking.png");
		pNormalSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto redo = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "rankingButton was pressed.");
		}
		);
		auto menu = Menu::create(redo, NULL);
		redo->setAnchorPoint(Vec2(1, 0));
		menu->setPosition(Vec2(visibleSize.width / 2, 140));
		this->addChild(menu, 1);
	}


	//settings
	{
		auto pNormalSprite = Sprite::create("img/settings.png");
		auto pSelectedSprite = Sprite::create("img/settings.png");
		pNormalSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto analyseButton = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "settingsButton was pressed.");
		}
		);
		auto menu = Menu::create(analyseButton, NULL);
		analyseButton->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(visibleSize.width / 2, 140));
		this->addChild(menu, 1);
	}
	my_debug::debug_msg(__FILE__, __LINE__, "LevelSelectScene::init()");
	return true;
}
