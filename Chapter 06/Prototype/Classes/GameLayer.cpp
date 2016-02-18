#include "GameLayer.h"
#include "SimpleAudioEngine.h"


using namespace cocos2d;
using namespace CocosDenshion;

GameLayer::~GameLayer() {
    
}

Scene* GameLayer::scene()
{
    auto scene = Scene::create();
    
    GameLayer *layer = GameLayer::create();

    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    
    //get screen size
    _screenSize = Director::getInstance()->getWinSize();
    
    createGameScreen();
    
    resetGame();
    
    //listen for touches
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //create main loop
    this->scheduleUpdate();
    
    return true;
}

void GameLayer::resetGame () {
    
    _speedIncreaseInterval = 15;
	_speedIncreaseTimer = 0;
    
    _running = true;
}


void GameLayer::update(float dt) {

    if (!_running) return;


    if (_player->getPositionY() < -_player->getHeight() ||
        _player->getPositionX() < -_player->getWidth() * 0.5f) {

       _running = false;

    }

    _player->update(dt);

    _terrain->move(_player->getVector().x);

    if (_player->getState() != kPlayerDying) _terrain->checkCollision(_player);

    _player->place();


    if (_player->getNextPosition().y > _screenSize.height * 0.6f) {
        _gameBatchNode->setPositionY( (_screenSize.height * 0.6f - _player->getNextPosition().y) * 0.8f);
    } else {
        _gameBatchNode->setPositionY  ( 0 );
    }

    if (_terrain->getStartTerrain() && _player->getVector().x > 0) {

        _speedIncreaseTimer += dt;
        if (_speedIncreaseTimer > _speedIncreaseInterval) {
            _speedIncreaseTimer = 0;
            _player->setMaxSpeed (_player->getMaxSpeed() + 4);
        }
    }
    
}

bool GameLayer::onTouchBegan(Touch* touch, Event* event) {
    
    if (!_running) {
        
        if (_player->getState() == kPlayerDying) {
            _terrain->reset();
            _player->reset();
            resetGame();
        }
        
        return true;
        
    }
    
    if (!_terrain->getStartTerrain()) {
        _terrain->setStartTerrain ( true );
        return true;
    }

    if (touch) {
	    
                
        if (_player->getState() == kPlayerFalling) {
            _player->setFloating ( _player->getFloating() ? false : true );
        
        } else {
            
            if (_player->getState() !=  kPlayerDying) _player->setJumping(true);
        }
        return true;
    }
    
    return false;
}

void GameLayer::onTouchEnded(Touch* touch, Event* event) {
    _player->setJumping(false);    
}

void GameLayer::createGameScreen () {
    
    _gameBatchNode = SpriteBatchNode::create("blank.png", 200);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _terrain = Terrain::create();
    _gameBatchNode->addChild(_terrain, kMiddleground);
    
    _player = Player::create();
    _gameBatchNode->addChild(_player, kBackground);
    
}