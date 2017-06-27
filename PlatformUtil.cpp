#include "PlatformUtil.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

using namespace std;
USING_NS_CC;

const static char className[] = "com/fmzk/puzzRoute/PlatformUtil";
const static char className1[] = "org/cocos2dx/cpp/AppActivity";


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
float PlatformUtil::getAdWidth() 
{
	// ‰¡•
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t, className1, "getAdBannerWidthPhysicalPixels", "()I")) {
		jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
		return ((float)ret);
	}
	return -1;
}
float PlatformUtil::getAdHeight()
{
	// ‚‚³
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t, className1, "getAdBannerHeightPhysicalPixels", "()I")) {
		jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
		return ((float)ret);
	}
	return -1;
}
void PlatformUtil::showIntersAd()
{
	JniMethodInfo t;

	if (JniHelper::getStaticMethodInfo(t, className, "showIntersAd", "()V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}
bool PlatformUtil::isTablet()
{
	JniMethodInfo t;

	if (JniHelper::getStaticMethodInfo(t, className, "isTablet", "()Z"))
	{
		return t.env->CallStaticBooleanMethod(t.classID, t.methodID);
	}
	else {
		return false;
	}
}
#else

float PlatformUtil::getAdWidth()
{
	return -1;
}
float PlatformUtil::getAdHeight()
{
	return -1;
}
void PlatformUtil::showIntersAd()
{
}
bool PlatformUtil::isTablet()
{
	return false;
}
#endif
