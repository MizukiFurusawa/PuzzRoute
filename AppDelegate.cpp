#include "AppDelegate.h"
#include "LogoScene.h"
#include "my_debug.h"
#include "LevelSelectScene.h"
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "PlatformUtil.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
using namespace std;
USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}


const float designResolutionSize_width = 720;
const float designResolutionSize_height = 1280;

bool AppDelegate::applicationDidFinishLaunching() {
    Director * director = Director::getInstance();
    GLView * glview = director->getOpenGLView();

    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        RECT WorkAreaRect;
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkAreaRect, 0);
        glview = GLViewImpl::createWithRect("15puzzle", cocos2d::Rect(0, 0, (int)designResolutionSize_width, (int)designResolutionSize_height));
#else
        glview = GLViewImpl::create("15puzzle");
#endif
        director->setOpenGLView(glview);
    }
    glview->setDesignResolutionSize((int)designResolutionSize_width, (int)designResolutionSize_height, ResolutionPolicy::SHOW_ALL);
    Size getDesignSize = glview->getDesignResolutionSize();
    director->setAnimationInterval(1.0f / 60.0f);
    director->setContentScaleFactor(designResolutionSize_width / getDesignSize.width);
    register_all_packages();

    // create a scene. it's an autorelease object
    // run


	CCLOG("adWidth  = %.3f", PlatformUtil::getAdWidth());
	CCLOG("adHeight = %.3f", PlatformUtil::getAdHeight());

#if COCOS2D_DEBUG
	my_debug::debug_msg(__FILE__, __LINE__, "game start!");
	director->setDisplayStats(true);
    director->runWithScene(LevelSelectScene::createScene());
#else
	my_debug::debug_msg(__FILE__, __LINE__, "game start!");
	director->setDisplayStats(false);
	director->runWithScene(LogoScene::createScene());
#endif
    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
