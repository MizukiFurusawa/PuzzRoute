#pragma once
#include "cocos2d.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


using namespace cocos2d;
using namespace std;
typedef unsigned long long my_u_int64;
#define  BOARD_SIZE	16

typedef struct {
	int milli;
	int sec;
	int min;
	int tap;
	
}SCORE_T;

typedef enum {
	LV_EASY,
	LV_NORMAL,
	LV_HARD,
	LV_EXPERT,
	LV_UNDEFINE
}LV_T;



class MainGame : public cocos2d::Layer {
public:

	static cocos2d::Scene* createScene(int shuffle, LV_T lv);

	CREATE_FUNC(MainGame);


	//const values
	const int panelBlankIndex = 15;
	const int imgSize = 720;
	const int panelSize = imgSize / 4;


	//static values
	static int shuffle_max;
	static LV_T myLv;
	static SCORE_T highScore;

	typedef enum {
		NONE,
		TOP,
		LEFT,
		RIGHT,
		BOTTOM,
	}DIRECTION_T;


	//integer values
	int callcnt = 0;
	int BOARD[BOARD_SIZE] = {};
	int RESULT[100] = {};
	int DEPTH = 0;
	int tapCount = 0;
	int backNumber = 0;


	//double values
	double moveTime = 0.0625;
	double waitTime = 0.125;


	//void functions
	void httpRequest();
	void flipLoadingAnimation(bool visible);
	void showTapCount(float delta);
	void showMilliSec(float delta);
	void update(float delta);
	void Initialize(void);
	void shuffle(int count);
	void setAction();
	void panelTapCallback(cocos2d::Ref* pSender);
	void setTimerText(my_u_int64 milliSec);
	void setTapText(int tap);
	void writeScore();
	void playMoveSE();

	//integer functions
	int getTag(int p);


	//bool functions
	bool isMoved(Vec2 tag1, Vec2 tag2, DIRECTION_T *d = nullptr);
	bool movePanel(int tag, bool pushTapHistory);
	bool checkClear();
	virtual bool init();


	//bool values
	bool isGameStart = false;
	bool isAnalyzeing = false;
	bool autoAnalyze = false;
	bool isCleared = false;
	bool isActionFinished = true;

	
	//my_u_int64 values
	my_u_int64 startTime = 0;
	my_u_int64 elapsedTime = 0;


	//my_u_int64 functins
	my_u_int64 getMilliSec();


	vector <cocos2d::Menu*> menu;
	vector <int> moveHistory;
	Menu* analyzing_menu = nullptr;
	Menu* cleared_menu = nullptr;
	Menu* start_menu = nullptr;
	Label* tapText = nullptr;
	Label* timerText = nullptr;
	Sprite* loading_spr = nullptr;
};
