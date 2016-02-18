//
//  LevelSelectLayer.cpp
//  Eskimo
//
//  Created by Roger Engelbert on 1/22/13.
//
//

#include "LevelSelectLayer.h"
#include "MenuLayer.h"
#include "GameLayer.h"
#include "GameConfig.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;


//this is for testing purposes... so imagine we have 100 levels!
#define TOTAL_LEVELS 100
#define LEVELS_PER_SCREEN 25


LevelSelectLayer::~LevelSelectLayer()
{
    _levels.clear();
    _levelLabels.clear();
}

LevelSelectLayer::LevelSelectLayer()
{
    _screenSize = Director::getInstance()->getWinSize();
	
	//load user data for number of levels completed
    _levelsCompleted = UserDefault::getInstance()->getIntegerForKey("levelsCompleted");
    if (_levelsCompleted == 0) {
        _levelsCompleted = 1;
        UserDefault::getInstance()->setIntegerForKey("levelsCompleted", 1);
        UserDefault::getInstance()->flush();
    }
   
    _firstIndex = 1 + (floor(_levelsCompleted/LEVELS_PER_SCREEN) * LEVELS_PER_SCREEN);
    
    createScreen();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(LevelSelectLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(LevelSelectLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
}


bool LevelSelectLayer::onTouchBegan(Touch * touch, Event* event) {
    
     if (touch) {
         Point tap = touch->getLocation();
         for (auto btn : _levels)
         {
             if (tap.distance( btn->getPosition()) <=  btn->getBoundingBox().size.width * 0.8f) {
                 if (btn->getTag() == kTagButtonOff) {
                     btn->setDisplayFrame(
                        SpriteFrameCache::getInstance()->getSpriteFrameByName ("btn_num_on.png")
                                        );
                     return true;
                 }
                 
             }
         }
     }
    return true;

}

void LevelSelectLayer::onTouchEnded (Touch * touch, Event* event) {
    
    if (touch) {
        Point tap = touch->getLocation();
        for (int i = 0; i < _levels.size(); i++)
        {
            auto btn = _levels.at(i);
            if (tap.distance( btn->getPosition()) <= btn->getBoundingBox().size.width * 0.8f) {
                if (btn->getTag() == kTagButtonOff) {
                    SimpleAudioEngine::getInstance()->playEffect("button.wav");
                    btn->setDisplayFrame(
                     SpriteFrameCache::getInstance()->getSpriteFrameByName ("btn_num_off.png")
                     );
                
                    auto newScene = TransitionMoveInR::create(0.2f, GameLayer::scene(_firstIndex + i, _levelsCompleted));
                    Director::getInstance()->replaceScene(newScene);
                    
                    return;
                }
            }
            i++;
        }
        //track touch on back button button
        if (tap.distance( _btnBack->getPosition()) <=  _btnBack->getTextureRect().size.width) {
            SimpleAudioEngine::getInstance()->playEffect("button.wav");
            if (_firstIndex == 1) {
                auto newScene = TransitionMoveInL::create(0.2f, MenuLayer::scene());
                Director::getInstance()->replaceScene(newScene);
            } else {
                _firstIndex -= LEVELS_PER_SCREEN;
                createMenu();
                _btnNext->setVisible(true);
                
            }
        //track touch on next button button
        } else if (tap.distance( _btnNext->getPosition()) <=  _btnNext->getTextureRect().size.width) {
            SimpleAudioEngine::getInstance()->playEffect("button.wav");
            if (_firstIndex < TOTAL_LEVELS - LEVELS_PER_SCREEN) {
                _firstIndex += LEVELS_PER_SCREEN;
                createMenu();
            }
        }
        
    }
}

Scene* LevelSelectLayer::scene()
{
    auto scene = Scene::create();
    
    auto layer = new LevelSelectLayer();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}


void LevelSelectLayer::createScreen() {
    //if we load this scene first, load sprite frames
    //SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("sprite_sheet.plist");

    auto bg = Sprite::create("bg.jpg");
    bg->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg);
    
    //add snow particle
    _snow = ParticleSystemQuad::create("snow.plist");
    _snow->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height));
    this->addChild(_snow, kBackground);
    
    _layerBatchNode = SpriteBatchNode::create("sprite_sheet.png", 60);
    this->addChild(_layerBatchNode, kMiddleground);
    
    auto ground = Sprite::createWithSpriteFrameName("intro_igloo.png");
    ground->setAnchorPoint(Vec2(0.5f, 0.05f));
    ground->setPosition(Vec2(_screenSize.width * 0.5f, 0.0));
    _layerBatchNode->addChild(ground);
    
    auto logo = Sprite::createWithSpriteFrameName("logo.png");
    logo->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.82f));
    _layerBatchNode->addChild(logo);
    
    
    //build level picker menu
    int cols = 5;
    int rows = 5;
    int i = _firstIndex;
    
    Sprite * menuItem;
    Point position;
    
    _levelLabels = Vector<Label *>(LEVELS_PER_SCREEN);
    _levels = Vector<Sprite *>(LEVELS_PER_SCREEN);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            position = Vec2(_screenSize.width * 0.2f +
                           c * _screenSize.width * 0.15f,
                           _screenSize.height * 0.65f -
                           _screenSize.width * 0.15f * r );
            if (i > _levelsCompleted) {
                menuItem = Sprite::createWithSpriteFrameName("btn_num_on.png");
                menuItem->setTag(kTagButtonOn);
            } else {
                menuItem = Sprite::createWithSpriteFrameName("btn_num_off.png");
                menuItem->setTag(kTagButtonOff);
            }
            menuItem->setPosition(position);
            _layerBatchNode->addChild(menuItem);
            //add the background sprite to an array so we track touch collision
            _levels.pushBack(menuItem);
            
            auto label = Label::createWithBMFont("font_levels.fnt", String::createWithFormat("%i", i)->getCString());
            label->setAnchorPoint(Vec2(0.5f, 0.5f));
            //label->setWidth(_screenSize.width * 0.1f);
            label->setPosition(position);
            //add the labels to an array so we can update them
            _levelLabels.pushBack(label);
            
            this->addChild(label, kForeground);
            i++;
        }
    }
   
    //the nav buttons
    _btnBack = Sprite::createWithSpriteFrameName("switch_3.png");
    _btnBack->setPosition(Vec2(_screenSize.width * 0.15f, _screenSize.height * 0.1f));
    _layerBatchNode->addChild(_btnBack);
    
    _btnNext = Sprite::createWithSpriteFrameName("switch_2.png");
    _btnNext->setPosition(Vec2(_screenSize.width * 0.85f, _screenSize.height * 0.1f));
    _layerBatchNode->addChild(_btnNext);
    
    if (_firstIndex == TOTAL_LEVELS - LEVELS_PER_SCREEN + 1) {
        _btnNext->setVisible(false);
    }
}

//update the level labels with the current displayed range (1-25, 26-50, 51-75, 76-100)
void LevelSelectLayer::createMenu() {
    
    int count = (int) _levelLabels.size();
    int index = _firstIndex;
    for (int  i = 0; i <  count; i++) {
        auto label = _levelLabels.at(i);
        auto btn = _levels.at(i);
        if (index > _levelsCompleted) {
            btn->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName ("btn_num_on.png"));
            btn->setTag(kTagButtonOn);
        } else {
            btn->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName ("btn_num_off.png"));
            btn->setTag(kTagButtonOff);
        }
        
        label->setString(String::createWithFormat("%i", index)->getCString() );
        
        index++;
        
    }
    if (_firstIndex == TOTAL_LEVELS - LEVELS_PER_SCREEN + 1) {
        _btnNext->setVisible(false);
    }
}
