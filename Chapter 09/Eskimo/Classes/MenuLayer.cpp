//
//  MenuLayer.cpp
//  Eskimo
//
//  Created by Roger Engelbert on 1/22/13.
//
//

#include "MenuLayer.h"
#include "LevelSelectLayer.h"
#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "GameConfig.h"

using namespace CocosDenshion;


MenuLayer::~MenuLayer(){}

MenuLayer::MenuLayer()
{
    _screenSize = Director::getInstance()->getWinSize();
    //interval between balloon messages
    _msgCntDown = 5;
    createScreen();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(MenuLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(MenuLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    scheduleUpdate();
    
}

void MenuLayer::update(float dt) {

	//update balloon messages
    if (!_caption2->isVisible()) _msgCntDown += dt;
    
    if (_msgCntDown > 6) {
        if (!_balloon->isVisible()) {
            SimpleAudioEngine::getInstance()->playEffect("cap.wav");
            _msgCntDown = 0;
            _balloon->setVisible(true);
            _caption1->setVisible(true);
        } else {
            if (!_caption2->isVisible()) {
                SimpleAudioEngine::getInstance()->playEffect("cap.wav");
                _caption1->setVisible(false);
                _caption2->setVisible(true);
            }
        }
    }
}


Scene* MenuLayer::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // add layer as a child to scene
    auto layer = new MenuLayer();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

void MenuLayer::showLevels() {
    SimpleAudioEngine::getInstance()->playEffect("button.wav");
    auto newScene = TransitionMoveInR::create(0.2f, LevelSelectLayer::scene());
    Director::getInstance()->replaceScene(newScene);
}
void MenuLayer::showHelp() {
    SimpleAudioEngine::getInstance()->playEffect("button.wav");
    int levelsCompleted = UserDefault::getInstance()->getIntegerForKey("levelsCompleted");
    auto newScene = TransitionMoveInR::create(0.2f, GameLayer::scene(0, levelsCompleted));
    Director::getInstance()->replaceScene(newScene);
}

void MenuLayer::createScreen() {
    
    auto bg = Sprite::create("bg.jpg");
    bg->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg);
    
    //load a particle system for snow
    _snow = ParticleSystemQuad::create("snow.plist");
    _snow->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height));
    this->addChild(_snow, kBackground);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite_sheet.plist");
    _layerBatchNode = SpriteBatchNode::create("sprite_sheet.png", 50);
    this->addChild(_layerBatchNode, kMiddleground);
    
    auto ground = Sprite::createWithSpriteFrameName("intro_igloo.png");
    ground->setAnchorPoint(Vec2(0.5f, 0.05f));
    ground->setPosition(Vec2(_screenSize.width * 0.5f, 0.0));
    _layerBatchNode->addChild(ground);
    
    auto logo = Sprite::createWithSpriteFrameName("logo.png");
    logo->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.75f));
    _layerBatchNode->addChild(logo);

    //create menu
    auto menuItemOn = Sprite::createWithSpriteFrameName("btn_play_on.png");
    auto menuItemOff = Sprite::createWithSpriteFrameName("btn_play_off.png");
    
    auto playItem =MenuItemSprite::create(menuItemOff, menuItemOn,  std::bind(&MenuLayer::showLevels, this));

    menuItemOn = Sprite::createWithSpriteFrameName("btn_help_on.png");
    menuItemOff = Sprite::createWithSpriteFrameName("btn_help_off.png");
    
    auto helpItem = MenuItemSprite::create( menuItemOff, menuItemOn, std::bind(&MenuLayer::showHelp, this));
    
    _mainMenu = Menu::create(playItem, helpItem, NULL);
    _mainMenu->alignItemsVerticallyWithPadding(10);
    _mainMenu->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(_mainMenu, kForeground);
    

    
    //add balloon caption
    _balloon = Sprite::createWithSpriteFrameName("cap.png");
    _balloon->setPosition(Vec2(_screenSize.width * 0.36f, _screenSize.height * 0.25f));
    _layerBatchNode->addChild(_balloon, kForeground);
    _balloon->setVisible(false);
    
    _caption1 = Sprite::createWithSpriteFrameName("cap_supper.png");
    _caption2 = Sprite::createWithSpriteFrameName("cap_where.png");

    _caption1->setVisible(false);
    _caption2->setVisible(false);
    
    _balloon->addChild(_caption1);
    _balloon->addChild(_caption2);
    
    Rect rect = _balloon->getTextureRect();
    _caption1->setPosition(Vec2(rect.size.width * 0.42f, rect.size.height * 0.45f));
    _caption2->setPosition(Vec2(rect.size.width * 0.42f, rect.size.height * 0.45f));
}