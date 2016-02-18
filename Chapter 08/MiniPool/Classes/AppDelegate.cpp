#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "GameLayer.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }
    
    // turn on display FPS
    director->setDisplayStats(true);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    auto screenSize = glview->getFrameSize();
    auto designSize = Size(320, 480);
    
    glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);
    
    std::vector<std::string> searchPaths;
    if (screenSize.width > 640) {
        searchPaths.push_back("ipadhd");
        director->setContentScaleFactor(1280/designSize.width);
    } else if (screenSize.width > 320) {
        searchPaths.push_back("ipad");
        director->setContentScaleFactor(640/designSize.width);
    } else {
        searchPaths.push_back("iphone");
        director->setContentScaleFactor(320/designSize.width);
    }
    auto fileUtils = FileUtils::getInstance();
    fileUtils->setSearchPaths(searchPaths);
    
    auto audioEngine = SimpleAudioEngine::getInstance();
    
    audioEngine->preloadEffect( fileUtils->fullPathForFilename("ball.wav").c_str() );
    audioEngine->preloadEffect( fileUtils->fullPathForFilename("drop.wav").c_str() );
    audioEngine->preloadEffect( fileUtils->fullPathForFilename("hit.wav").c_str() );
    audioEngine->preloadEffect( fileUtils->fullPathForFilename("whitedrop.wav").c_str() );
    
    
    // create a scene. it's an autorelease object
    auto scene = GameLayer::scene();
    
    // run
    director->runWithScene(scene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
