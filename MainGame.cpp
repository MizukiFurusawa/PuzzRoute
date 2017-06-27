#pragma execution_character_set("utf-8")
#include "network/HttpClient.h"
#include "MainGame.h"
#include "LevelSelectScene.h"
#include "my_debug.h"
#include "PlatformUtil.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
using namespace cocos2d::network;
using namespace cocos2d;

LV_T	MainGame::myLv;
SCORE_T MainGame::highScore;
int		MainGame::shuffle_max = 0;

Scene* MainGame::createScene(int shuffle, LV_T lv) {
	UserDefault *_userDef = UserDefault::getInstance();
	shuffle_max = shuffle;
	myLv = lv;

	switch (lv) {
	case LV_T::LV_EASY:
		highScore.milli = _userDef->getIntegerForKey("easy.milli", 99);
		highScore.sec = _userDef->getIntegerForKey("easy.sec", 59);
		highScore.min = _userDef->getIntegerForKey("easy.min", 59);
		highScore.tap = _userDef->getIntegerForKey("easy.tap", 999);
		break;

	case LV_T::LV_NORMAL:
		highScore.milli = _userDef->getIntegerForKey("normal.milli", 99);
		highScore.sec = _userDef->getIntegerForKey("normal.sec", 59);
		highScore.min = _userDef->getIntegerForKey("normal.min", 59);
		highScore.tap = _userDef->getIntegerForKey("normal.tap", 999);
		break;

	case LV_T::LV_HARD:
		highScore.milli = _userDef->getIntegerForKey("hard.milli", 99);
		highScore.sec = _userDef->getIntegerForKey("hard.sec", 59);
		highScore.min = _userDef->getIntegerForKey("hard.min", 59);
		highScore.tap = _userDef->getIntegerForKey("hard.tap", 999);
		break;

	case LV_T::LV_EXPERT:

		highScore.milli = _userDef->getIntegerForKey("expert.milli", 99);
		highScore.sec = _userDef->getIntegerForKey("expert.sec", 59);
		highScore.min = _userDef->getIntegerForKey("expert.min", 59);
		highScore.tap = _userDef->getIntegerForKey("expert.tap", 999);
		break;

	default:
		highScore.milli = 99;
		highScore.sec = 59;
		highScore.min = 59;
		highScore.tap = 999;
		break;
	}

	auto scene = Scene::create();
	auto layer = MainGame::create();
	scene->addChild(layer);
	my_debug::debug_msg(__FILE__, __LINE__, "createScene() was called.");
	return scene;
}

void MainGame::httpRequest() {
	auto request = new HttpRequest();
	std::string url = "http://f-mzk.sakura.ne.jp/puz15sv2.php?id=";
	Initialize();
	for (unsigned int i = 0; i < 16; i++) {
		char id[16];
		sprintf(id, "%02d", BOARD[i]);
		url += id;
	}
	flipLoadingAnimation(true);
	request->setUrl(url.c_str());
	request->setRequestType(HttpRequest::Type::GET);

	request->setResponseCallback([this](HttpClient* client, HttpResponse* response) {

		flipLoadingAnimation(false);
		if (response->isSucceed()) {
			std::vector<char>* data = response->getResponseData();
			std::string result(data->begin(), data->end());
			my_debug::debug_msg(__FILE__, __LINE__, "connecting was succeed.");
			
			log("%s",result.c_str());

			if (result == "impossible!!") {
				MessageBox("このパズル配置は解くことができません.", "error");
				my_debug::debug_msg(__FILE__, __LINE__, "It is not possible to solve this puzzle.");
				flipLoadingAnimation(false);
				return;
			}
			else if (result == "Please set the argument.") {
				MessageBox("引数を指定してください.", "error");
				my_debug::debug_msg(__FILE__, __LINE__, "Please set the argument.");
				flipLoadingAnimation(false);
				return;
			}
			else if (result == "file open error!!") {
				MessageBox("データベースファイルのオープンに失敗しました.", "error");
				my_debug::debug_msg(__FILE__, __LINE__, "file open error!!");
				flipLoadingAnimation(false);
				return;
			}
			else if (result == "Analyzing was timeout.") {
				MessageBox("サーバーとの接続がタイムアウトしました.", "error");
				my_debug::debug_msg(__FILE__, __LINE__, "Analyzing was timeout.");
				flipLoadingAnimation(false);
				return;
			}
			else if (result == "") {
				MessageBox("Analyzerが見つかりませんでした.", "error");
				my_debug::debug_msg(__FILE__, __LINE__, "Analyzer not found.");
				flipLoadingAnimation(false);
				return;
			}
			
			autoAnalyze = true;

			//1回もパネルをタップせずに
			//実行した場合
			if (!isGameStart) {
				startTime = getMilliSec();
				isGameStart = true;
			}

			for (int index = 0,i = 0; ; index++) {
				char str[3];
				str[0] = result[i];
				str[1] = result[i + 1];
				str[2] = '\0';
				RESULT[index] = atoi(str);
				if (RESULT[index] != 0)i += 2;
				else break;
			}
			scheduleUpdate();
		}

		else {
			MessageBox("サーバーとの接続に失敗しました.", "error");
			my_debug::debug_msg(__FILE__, __LINE__, "It failed to connect to the server.");
		}
	});

	auto client = HttpClient::getInstance();
	client->enableCookies(NULL);
	client->send(request);
	my_debug::debug_msg(__FILE__, __LINE__, "httpRequest() was called.");
}

void MainGame::playMoveSE() {
	SimpleAudioEngine::sharedEngine()->playEffect("se/blockmove.wav");
}
bool MainGame::init() {
	if (!Layer::init()) {
		return false;
	}
	

	//画面サイズ取得
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//tap回数表示用
	this->schedule(schedule_selector(MainGame::showTapCount));


	//経過時間表示用
	this->schedule(schedule_selector(MainGame::showMilliSec));


	//back
	{
		auto back = Sprite::create("img/back.png");
		back->setAnchorPoint(Vec2(0,1));
		back->setPosition(0, visibleSize.height);
		this->addChild(back, 0);
	}


#define BRIGHT	200
	//home
	{
		auto pNormalSprite = Sprite::create("img/home.png");
		auto pSelectedSprite = Sprite::create("img/home.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto restartButton = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "restartButton was pressed.");
			playMoveSE();
			Director::getInstance()->replaceScene(
				CCTransitionFade::create(0.5f, LevelSelectScene::createScene()));
		}
		);
		auto menu = Menu::create(restartButton, NULL);
		restartButton->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(180 * 0, 140));
		this->addChild(menu, 1);
	}


	//undo
	{
		auto pNormalSprite = Sprite::create("img/undo.png");
		auto pSelectedSprite = Sprite::create("img/undo.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto undo = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "undoButton was pressed.");
			playMoveSE();
			if (autoAnalyze)return;
			int index = tapCount - 1;
			if (index >= 0 && movePanel(moveHistory[index], false)) {
				tapCount--;
			}
		}
		);
		auto menu = Menu::create(undo, NULL);
		undo->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(180 * 1, 140));
		this->addChild(menu, 1);
	}


	//redo
	{
		auto pNormalSprite = Sprite::create("img/redo.png");
		auto pSelectedSprite = Sprite::create("img/redo.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto redo = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "redoButton was pressed.");
			playMoveSE();
			if (autoAnalyze)return;
			unsigned int index = (unsigned int)tapCount;
			if (index < moveHistory.size() && movePanel(moveHistory[index], false)) {
				tapCount++;
			}
		}
		);
		auto menu = Menu::create(redo, NULL);
		redo->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(180 * 2, 140));
		this->addChild(menu, 1);
	}


	//search
	{
		auto pNormalSprite = Sprite::create("img/search.png");
		auto pSelectedSprite = Sprite::create("img/search.png");
		pSelectedSprite->setColor(Color3B(BRIGHT, BRIGHT, BRIGHT));

		auto analyseButton = MenuItemSprite::create(
			pNormalSprite,
			pSelectedSprite,
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "analyseButton was pressed.");
			SimpleAudioEngine::sharedEngine()->playEffect("se/analyze.wav");

			//todo: step 実行を実装する

			if (isActionFinished && !isCleared && !isAnalyzeing) {
				my_debug::debug_msg(__FILE__, __LINE__, "now analyzing...");
				httpRequest();
			}
		}
		);
		auto menu = Menu::create(analyseButton, NULL);
		analyseButton->setAnchorPoint(Vec2(0, 0));
		menu->setPosition(Vec2(180 * 3, 140));
		this->addChild(menu, 1);
	}


	//timer
	{
		auto timer = Sprite::create("img/timer.png");
		timer->setAnchorPoint(Vec2(0.0, 0.5));
		timer->setPosition(Vec2(60, 1150 + 20));
		timer->setTag(100);
		this->addChild(timer);

		TTFConfig ttfConfig("fonts/funwari-round.ttf", 48, GlyphCollection::ASCII);
		timerText = Label::createWithTTF(ttfConfig, "00:00:00");
		timerText->setAnchorPoint(Vec2(0.0, 0.5));
		timerText->setPosition(Point(180, 1140 + 20));
		timerText->setTag(101);
		this->addChild(timerText, 1);
		setTimerText(
			(my_u_int64)
			(highScore.min * 1000 * 60 +
				highScore.sec * 1000 +
				highScore.milli * 10));
	}


	//tap count
	{
		auto tap = Sprite::create("img/tap.png");
		tap->setAnchorPoint(Vec2(0.0, 0.5));
		tap->setPosition(Vec2(430, 1150 + 20));
		tap->setTag(200);
		this->addChild(tap);

		TTFConfig ttfConfig("fonts/funwari-round.ttf", 48, GlyphCollection::ASCII);
		tapText = Label::createWithTTF(ttfConfig, "000");
		tapText->setAnchorPoint(Vec2(0.0, 0.5));
		tapText->setPosition(Point(555, 1140 + 20));
		tapText->setTag(201);
		this->addChild(tapText, 1);
		setTapText(highScore.tap);
	}


	//ブロックの配置
	{
		int x = 0;
		int y = 0;
		int count = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Rect rect(x, y, panelSize, panelSize);
				Sprite *spr = Sprite::create("img/blocks.png", rect);
				auto menuItem = MenuItemSprite::create(
					spr, NULL, CC_CALLBACK_1(MainGame::panelTapCallback, this));
				menuItem->setTag(count);
				menuItem->setAnchorPoint(Vec2(0.0, 0.0));
				menu.emplace_back();
				menu[menu.size() - 1] = Menu::create(menuItem, NULL);
				menu[menu.size() - 1]->setTag(count);
				menu[menu.size() - 1]->setAnchorPoint(Vec2(0.0, 0.0));
				menu[menu.size() - 1]->setPosition(
					Vec2(
						panelSize * (count % 4),
						(imgSize + 330) - panelSize - panelSize * (count / 4)));

				this->addChild(menu[menu.size() - 1], 1);
				x += panelSize;
				count++;
			}
			x = 0;
			y += panelSize;
		}
	}

	//analyzing表示用スプライトの生成
	{
		auto analyzing = MenuItemSprite::create(
			Sprite::create("img/analyzing.png"),
			Sprite::create("img/analyzing.png"),
			[this](Ref *pSender) {
		}
		);
		analyzing_menu = Menu::create(analyzing, NULL);
		analyzing_menu->setAnchorPoint(Vec2(0.5, 0.5));
		analyzing_menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

		auto loading = Animation::create();
		for (int i = 0; i < 16; i++) {
			char filename[128] = { 0 };
			sprintf(filename, "loading/Preloader_2_%05d.png", i);
			loading->addSpriteFrameWithFileName(filename);
		}
		loading->setDelayPerUnit(0.02f);
		loading->setLoops(-1);
		loading_spr = Sprite::create();
		loading_spr->runAction(Animate::create(loading));
		loading_spr->setAnchorPoint(Vec2::ZERO);
		loading_spr->setPosition(Vec2(520, 545));

		flipLoadingAnimation(false);
		this->addChild(analyzing_menu, 100);
		this->addChild(loading_spr,101);
	}

	//スタート前
	{
		auto start = MenuItemSprite::create(
			Sprite::create("img/start.png"),
			Sprite::create("img/start.png"),
			[this](Ref *pSender) {

			//初めてパネルをタップしたとき
			startTime = getMilliSec();
			isGameStart = true;
			start_menu->setVisible(false);
		}
		);
		start_menu = Menu::create(start, NULL);
		start_menu->setAnchorPoint(Vec2(0.5, 0.5));
		start_menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		this->addChild(start_menu, 100);
	}

	//クリア表示用スプライトの生成
	{
		auto cleared = MenuItemSprite::create(
			Sprite::create("img/cleared.png"),
			Sprite::create("img/cleared.png"),
			[this](Ref *pSender) {
			my_debug::debug_msg(__FILE__, __LINE__, "clearedSprite was pressed.");

			PlatformUtil::showIntersAd();

			//update: 自動解析モードではスコアは記録しない
			if (!autoAnalyze) {
				writeScore();
			}

			Director::getInstance()->replaceScene(
				CCTransitionFade::create(0.5f, LevelSelectScene::createScene()));
		}
		);
		cleared_menu = Menu::create(cleared, NULL);
		cleared_menu->setAnchorPoint(Vec2(0.5, 0.5));
		cleared_menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		cleared_menu->setVisible(false);
		this->addChild(cleared_menu, 100);
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//admob test
	{
		auto adSprite = Sprite::create("img/adTest.png");
		adSprite->setAnchorPoint(Vec2(0.5, 0.0));
		adSprite->setPosition(Vec2(visibleSize.width / 2, 0.0));
		this->addChild(adSprite, 100);
	}
#endif

	//ブロックをshuffle_max回シャッフル
	//この画面に遷移する時に回数を指定してある
	shuffle(shuffle_max);

	//最適ルート解析用に、
	//事前に生成したブロックの配置をロードしておく
	Initialize();

	my_debug::debug_msg(__FILE__, __LINE__, "init() was called.");

	return true;
}

void MainGame::writeScore() {
	UserDefault *_userDef = UserDefault::getInstance();
	my_u_int64 highScoreTime = 
		highScore.min * 1000 * 60 + 
		highScore.sec * 1000 + 
		highScore.milli;

	int milli = ((int)elapsedTime % 1000) / 10;
	int sec = ((int)elapsedTime / 1000) % 60;
	int min = ((int)elapsedTime / 1000) / 60;

	CCLOG("highScore.tap   = %d", highScore.tap);
	CCLOG("highScore.min   = %d", highScore.min);
	CCLOG("highScore.sec   = %d", highScore.sec);
	CCLOG("highScore.milli = %d", highScore.milli);
	CCLOG("highScoreTime   = %llu", highScoreTime);
	
	CCLOG("");
	CCLOG("this.tap        = %d", tapCount);
	CCLOG("this.min        = %d", min);
	CCLOG("this.sec        = %d", sec);
	CCLOG("this.milli      = %d", milli);
	CCLOG("elapsedTime     = %llu", elapsedTime);


	switch (myLv) {
	case LV_T::LV_EASY:
		if (tapCount < highScore.tap) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_EASY, tap count new score.");
			_userDef->setIntegerForKey("easy.tap", tapCount);
			this->getChildByTag(200)->setZOrder(101);
			this->getChildByTag(201)->setZOrder(101);
		}
		if (elapsedTime < highScoreTime) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_EASY, time new score.");
			_userDef->setIntegerForKey("easy.milli", milli);
			_userDef->setIntegerForKey("easy.sec", sec);
			_userDef->setIntegerForKey("easy.min", min);
			this->getChildByTag(100)->setZOrder(101);
			this->getChildByTag(101)->setZOrder(101);
		}
		break;

	case LV_T::LV_NORMAL:
		if (tapCount < highScore.tap) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_NORMAL, tap count new score.");
			_userDef->setIntegerForKey("normal.tap", tapCount);
			this->getChildByTag(200)->setZOrder(101);
			this->getChildByTag(201)->setZOrder(101);
		}
		if (elapsedTime < highScoreTime) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_NORMAL, time new score.");
			_userDef->setIntegerForKey("normal.milli", milli);
			_userDef->setIntegerForKey("normal.sec", sec);
			_userDef->setIntegerForKey("normal.min", min);
			this->getChildByTag(100)->setZOrder(101);
			this->getChildByTag(101)->setZOrder(101);
		}
		break;

	case LV_T::LV_HARD:
		if (tapCount < highScore.tap) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_HARD, tap count new score.");
			_userDef->setIntegerForKey("hard.tap", tapCount);
			this->getChildByTag(200)->setZOrder(101);
			this->getChildByTag(201)->setZOrder(101);
		}
		if (elapsedTime < highScoreTime) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_HARD, time new score.");
			_userDef->setIntegerForKey("hard.milli", milli);
			_userDef->setIntegerForKey("hard.sec", sec);
			_userDef->setIntegerForKey("hard.min", min);
			this->getChildByTag(100)->setZOrder(101);
			this->getChildByTag(101)->setZOrder(101);
		}
		break;

	case LV_T::LV_EXPERT:
		if (tapCount < highScore.tap) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_EXPERT, tap count new score.");
			_userDef->setIntegerForKey("expert.tap", tapCount);
			this->getChildByTag(200)->setZOrder(101);
			this->getChildByTag(201)->setZOrder(101);
		}
		if (elapsedTime < highScoreTime) {
			my_debug::debug_msg(__FILE__, __LINE__, "LV_EXPERT, time new score.");
			_userDef->setIntegerForKey("expert.milli", milli);
			_userDef->setIntegerForKey("expert.sec", sec);
			_userDef->setIntegerForKey("expert.min", min);
			this->getChildByTag(100)->setZOrder(101);
			this->getChildByTag(101)->setZOrder(101);
		}
		break;

	default:
		break;
	}
	my_debug::debug_msg(__FILE__, __LINE__, "writeScore() was called.");
}

void MainGame::setTapText(int tap) {
	char str[128];
	sprintf(str, "%03d", tap);
	tapText->setString(str);
}

void MainGame::setTimerText(my_u_int64 milliSec) {
	char str[128];
	my_u_int64 milli = (milliSec % 1000) / 10;
	my_u_int64 sec = (milliSec / 1000) % 60;
	my_u_int64 min = (milliSec / 1000) / 60;
	sprintf(str, "%02llu:%02llu:%02llu", min, sec, milli);
	timerText->setString(str);
}

void MainGame::showTapCount(float delta) {
	if (isGameStart) {
		setTapText(tapCount);
	}
}

void MainGame::showMilliSec(float delta) {
	if (isGameStart) {
		elapsedTime = getMilliSec() - startTime;
		setTimerText(elapsedTime);
	}
}

my_u_int64 MainGame::getMilliSec() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000ull + t.tv_usec / 1000ull;
}

void MainGame::flipLoadingAnimation(bool visible) {
	isAnalyzeing = visible;
	loading_spr->setVisible(visible);
	analyzing_menu->setVisible(visible);
	my_debug::debug_msg(__FILE__, __LINE__, "flipLoadingAnimation() was called.");
}

void MainGame::shuffle(int count) {
	my_debug::debug_msg(__FILE__, __LINE__, "shuffle() was called.");
	if (isAnalyzeing)return;
	srand((unsigned int)time(NULL));
	DIRECTION_T d1 = DIRECTION_T::NONE;
	DIRECTION_T d2 = DIRECTION_T::NONE;
	for (int i = 0; i < count; i++) {
		int r = rand() % 15;
		Vec2 v1 = menu[r]->getPosition();
		Vec2 v2 = menu[menu.size() - 1]->getPosition();
		if (isMoved(v1, v2, &d2)) {
			if (d1 == DIRECTION_T::TOP && d2 == DIRECTION_T::BOTTOM)continue;
			if (d1 == DIRECTION_T::BOTTOM && d2 == DIRECTION_T::TOP)continue;
			if (d1 == DIRECTION_T::LEFT && d2 == DIRECTION_T::RIGHT)continue;
			if (d1 == DIRECTION_T::RIGHT && d2 == DIRECTION_T::LEFT)continue;
			d1 = d2;
			menu[r]->setPosition(v2);
			menu[menu.size() - 1]->setPosition(v1);
		}
	}
}

// p :
// 00 01 02 03
// 04 05 06 07
// 08 09 10 11
// 12 13 14 15

// getTag(14)とは,
// 上の表の14の場所にちょうど存在する
// menuのtagを返す.
int MainGame::getTag(int p) {
	Vec2 v1((p % 4) * panelSize, imgSize - panelSize - (p / 4) * panelSize + 330);
	for (unsigned int i = 0; i < menu.size(); i++) {
		Vec2 v2 = menu[i]->getPosition();
		if (v1.equals(v2)) {
			return menu[i]->getTag();
		}
	}
	return -1;
}

bool MainGame::checkClear() {
	for (unsigned int i = 0; i < menu.size(); i++) {
		Vec2 v1(panelSize * (i % 4), (imgSize + 330) - panelSize - panelSize * (i / 4));
		Vec2 v2(menu[i]->getPosition());
		if (!v2.equals(v1))return false;
	}
	return true;
}

bool MainGame::movePanel(int tag, bool pushTapHistory = true) {
	if (!isActionFinished)return false;
	int panelTouchedIndex = tag;
	Vec2 v1 = menu[panelBlankIndex]->getPosition();
	Vec2 v2 = menu[panelTouchedIndex]->getPosition();
	if (!isMoved(v2, v1))return false;
	isActionFinished = false;
	auto v1_action = MoveTo::create(moveTime, v1);
	auto v2_action = MoveTo::create(moveTime, v2);
	menu[panelTouchedIndex]->runAction(Sequence::create(v1_action, NULL));
	menu[panelBlankIndex]->runAction(Sequence::create(v2_action, NULL));

	playMoveSE();

	this->runAction(Sequence::create(DelayTime::create(waitTime), CallFunc::create([this]() {
		isActionFinished = true;
		if (checkClear()) {
			cleared_menu->setVisible(true);
			this->unscheduleUpdate();
			this->unschedule(schedule_selector(MainGame::showMilliSec));
			SimpleAudioEngine::sharedEngine()->playEffect("se/cleared.wav");
			isCleared = true;
		}
	}), NULL));
	return true;
}

void MainGame::panelTapCallback(Ref* pSender) {
	if (autoAnalyze)return;
	if (isAnalyzeing)return;

	MenuItemSprite* target = static_cast<MenuItemSprite*>(pSender);
	int tag = target->getTag();

	if (movePanel(tag)) {

		if ((unsigned int)tapCount >= moveHistory.size()) {
			moveHistory.push_back(tag);
			tapCount++;
		}
		else {
			moveHistory.erase(
				moveHistory.begin() + tapCount,
				moveHistory.begin() + moveHistory.size());
			moveHistory.push_back(tag);
			tapCount++;
		}
	}

	/*
	string log = "";
	char log_c[32];
	for (unsigned int i = 0; i < moveHistory.size(); i++) {
		sprintf(log_c, "%03d -> ", moveHistory[i]);
		log += log_c;
	}
	cocos2d::log("%s", log.c_str());
	*/
}

//v1 = 選択したパネルの座標
//v2 = ブランクパネルの座標
//として、v1パネルの上下左右のいずれかに、
//v2パネルが存在するかどうか
bool MainGame::isMoved(Vec2 v1, Vec2 v2, DIRECTION_T *d) {
	if (d != nullptr)*d = DIRECTION_T::NONE;
	int p_tag1 = (((int)v1.x) % imgSize) / panelSize + ((int)v1.y - 330) * 4 / panelSize;
	int p_tag2 = (((int)v2.x) % imgSize) / panelSize + ((int)v2.y - 330) * 4 / panelSize;

	//上に存在するか
	if (p_tag1 + 4 < 16 && p_tag1 + 4 == p_tag2) {
		if (d != nullptr)*d = DIRECTION_T::TOP;
		return true;
	}

	//下に存在するか
	if (p_tag1 - 4 >= 0 && p_tag1 - 4 == p_tag2) {
		if (d != nullptr)*d = DIRECTION_T::BOTTOM;
		return true;
	}

	//左に存在するか
	if (p_tag1 % 4 != 0 && p_tag1 - 1 >= 0 && p_tag1 - 1 == p_tag2) {
		if (d != nullptr)*d = DIRECTION_T::LEFT;
		return true;
	}

	//右に存在するか
	if ((p_tag1 + 1) % 4 != 0 && p_tag1 + 1 < 16 && p_tag1 + 1 == p_tag2) {
		if (d != nullptr)*d = DIRECTION_T::RIGHT;
		return true;
	}
	return false;
}

void MainGame::update(float delta) {
	setAction();
}

void MainGame::setAction() {
	if (!autoAnalyze)return;
	if (!isActionFinished)return;
	int x = RESULT[backNumber];
	if (x == 0) {
		autoAnalyze = true;
		return;
	}
	if (movePanel(x - 1)) {
		tapCount++;
		backNumber++;
	}
}

void MainGame::Initialize(void) {
	my_debug::debug_msg(__FILE__, __LINE__, "Initialize() was called.");
	for (int i = 0; i < 16; i++) {
		int k = getTag(i) + 1;
		if (k == 16)k = 0;
		BOARD[i] = k;
	}
	cocos2d::log("%02d %02d %02d %02d", BOARD[0], BOARD[1], BOARD[2], BOARD[3]);
	cocos2d::log("%02d %02d %02d %02d", BOARD[4], BOARD[5], BOARD[6], BOARD[7]);
	cocos2d::log("%02d %02d %02d %02d", BOARD[8], BOARD[9], BOARD[10], BOARD[11]);
	cocos2d::log("%02d %02d %02d %02d", BOARD[12], BOARD[13], BOARD[14], BOARD[15]);
}
