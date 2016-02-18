#include "GameLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

GameLayer::~GameLayer() {
    
    CC_SAFE_RELEASE(_jamMove);
    CC_SAFE_RELEASE(_jamAnimate);
}


Scene* GameLayer::scene()
{
    auto scene = Scene::create();
    
    auto layer = GameLayer::create();

    scene->addChild(layer);

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
    
    _score = 0;
    _speedIncreaseInterval = 15;
	_speedIncreaseTimer = 0;
    _scoreDisplay->setString ( String::createWithFormat("%i", (int)_score)->getCString() );
    _scoreDisplay->setAnchorPoint(Vec2(1,0));
    _scoreDisplay->setPosition(Vec2(_screenSize.width * 0.95f, _screenSize.height * 0.88f));
    
    _state = kGameIntro;
    
    _intro->setVisible(true);
    _mainMenu->setVisible(true);
    
    _jam->setPosition(Vec2(_screenSize.width * 0.19f, _screenSize.height * 0.47f));
    _jam->setVisible(true);
    _jam->runAction(_jamAnimate);
    
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("background3.mp3", true);
    _running = true;
}


void GameLayer::update(float dt) {
    
    if (!_running) return;
    

    if (_player->getPositionY() < -_player->getHeight() ||
        _player->getPositionX() < -_player->getWidth() * 0.5f) {
        
        if (_state == kGamePlay) {
            
            _running = false;

            //create GAME OVER state
            
            _state = kGameOver;
            
            _tryAgain->setVisible(true);
            _scoreDisplay->setAnchorPoint(Vec2(0.5f, 0.5f));
            _scoreDisplay->setPosition(Vec2(_screenSize.width * 0.5f,
                                           _screenSize.height * 0.88f));
            _hat->setPosition(Vec2(_screenSize.width * 0.2f, -_screenSize.height * 0.1f));
            _hat->setVisible(true);
            auto rotate = RotateBy::create(2.0f, 660);
            auto jump = JumpBy::create(2.0f, Vec2(0,10), _screenSize.height * 0.8f, 1);
            _hat->runAction(rotate);
            _hat->runAction(jump);
            SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            SimpleAudioEngine::getInstance()->playEffect("crashing.wav");
            
        }
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
    
    
    //update paralax
    if (_player->getVector().x > 0) {
        _background->setPositionX(_background->getPosition().x - _player->getVector().x * 0.25f);
        float diffx;
        
        if (_background->getPositionX() < -_background->getContentSize().width) {
            diffx = fabs(_background->getPositionX()) - _background->getContentSize().width;
            _background->setPositionX(-diffx);
        }
        
        _foreground->setPositionX(_foreground->getPosition().x - _player->getVector().x * 4);
        
        if (_foreground->getPositionX() < -_foreground->getContentSize().width * 4) {
            diffx = fabs(_foreground->getPositionX()) - _foreground->getContentSize().width * 4;
            _foreground->setPositionX(-diffx);
        }
        
        for (auto cloud : _clouds) {
            cloud->setPositionX(cloud->getPositionX() - _player->getVector().x * 0.15f);
            if (cloud->getPositionX() + cloud->boundingBox().size.width * 0.5f < 0 )
                cloud->setPositionX(_screenSize.width + cloud->boundingBox().size.width * 0.5f);
        }
    }
    
    if (_jam->isVisible()) {
        if (_jam->getPositionX() < -_screenSize.width * 0.2f) {
            _jam->stopAllActions();
            _jam->setVisible(false);
        }
    }
    
    
    //update score
    if (_terrain->getStartTerrain() && _player->getVector().x > 0) {
        
        _score += dt * 50;
        _scoreDisplay->setString ( String::createWithFormat("%i", (int)_score)->getCString() );
        
        _speedIncreaseTimer += dt;
        if (_speedIncreaseTimer > _speedIncreaseInterval) {
            _speedIncreaseTimer = 0;
            _player->setMaxSpeed (_player->getMaxSpeed() + 4);
        }
    }
    
    if (_state > kGameTutorial) {
        if (_state == kGameTutorialJump) {
            if (_player->getState() == kPlayerFalling && _player->getVector().y < 0) {
                _player->stopAllActions();
                _jam->setVisible(false);
                _jam->stopAllActions();
                _running = false;
                _tutorialLabel->setString("While in the air, tap the screen to float.");
                _state = kGameTutorialFloat;
            }
        } else if (_state == kGameTutorialFloat) {
            if (_player->getPositionY() < _screenSize.height * 0.95f) {
                _player->stopAllActions();
                _running = false;
                _tutorialLabel->setString("While floating, tap the screen again to drop.");
                _state = kGameTutorialDrop;
            }
        } else {
            _tutorialLabel->setString("That's it. Tap the screen to play.");
            _state = kGameTutorial;
        }
    }
    
    
}

bool GameLayer::onTouchBegan(Touch * touch, Event* event) {
    
    if (touch) {
	    
	    Point tap = touch->getLocation();
        
        switch (_state) {
            
            case kGameIntro:
                break;
            case kGameOver:
                
                if (_tryAgain->boundingBox().containsPoint(tap)) {
                    _hat->setVisible(false);
                    _hat->stopAllActions();
                    _tryAgain->setVisible(false);
                    _terrain->reset();
                    _player->reset();
                    
                    resetGame();
                }
                break;
                
            case kGamePlay:
                
                if (_player->getState() == kPlayerFalling) {
                    _player->setFloating ( _player->getFloating() ? false : true );
                
                } else {
                    if (_player->getState() !=  kPlayerDying) {
                        SimpleAudioEngine::getInstance()->playEffect("jump.wav");
                        _player->setJumping(true);
                    }
                }
                
                _terrain->activateChimneysAt(_player);
                
                break;
            case kGameTutorial:
                _tutorialLabel->setString("");
                _tutorialLabel->setVisible(false);
                _terrain->setStartTerrain ( true );
                _state = kGamePlay;
                break;
            case kGameTutorialJump:
                if (_player->getState() == kPlayerMoving) {
                    SimpleAudioEngine::getInstance()->playEffect("jump.wav");
                    _player->setJumping(true);
                }
                break;
            case kGameTutorialFloat:
                if (!_player->getFloating()) {
                    _player->setFloating (true);
                    _running = true;
                }
                break;
            case kGameTutorialDrop:
                _player->setFloating (false);
                _running = true;
                break;
        }
    }
    return true;

}

void GameLayer::onTouchEnded(Touch* touch, Event* event) {
    
    if (_state == kGamePlay) {
        _player->setJumping(false);
    }
    
}

void GameLayer::showTutorial (Ref* pSender) {
    _tutorialLabel->setString("Tap the screen to make the player jump.");
    _state = kGameTutorialJump;
    _jam->runAction(_jamMove);
    _intro->setVisible(false);
    _mainMenu->setVisible(false);
    SimpleAudioEngine::getInstance()->playEffect("start.wav");
    _tutorialLabel->setVisible(true);
    
}

void GameLayer::startGame (Ref* pSender) {
    _tutorialLabel->setVisible(false);
    _intro->setVisible(false);
    _mainMenu->setVisible(false);
    
    _jam->runAction(_jamMove);
    SimpleAudioEngine::getInstance()->playEffect("start.wav");
    _terrain->setStartTerrain ( true );
    _state = kGamePlay;
}

void GameLayer::createGameScreen () {
    
    
    auto bg = Sprite::create("bg.png");
    bg->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg, kBackground);
    
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprite_sheet.plist");
    _gameBatchNode = SpriteBatchNode::create("sprite_sheet.png", 200);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _background = Sprite::createWithSpriteFrameName("background.png");
    _background->setAnchorPoint(Vec2(0,0));
    _gameBatchNode->addChild(_background, kBackground);
    
    auto repeat = Sprite::createWithSpriteFrameName("background.png");
    repeat->setAnchorPoint(Vec2(0,0));
    repeat->setPosition(Vec2(repeat->getContentSize().width - 1, 0));
    _background->addChild(repeat, kBackground);
    
    repeat = Sprite::createWithSpriteFrameName("background.png");
    repeat->setAnchorPoint(Vec2(0,0));
    repeat->setPosition(Vec2(2 * (repeat->getContentSize().width - 1), 0));
    _background->addChild(repeat, kBackground);
    
    _foreground = Sprite::createWithSpriteFrameName("lamp.png");
    _foreground->setAnchorPoint(Vec2(0,0));
    _gameBatchNode->addChild(_foreground, kForeground);
    
    repeat = Sprite::createWithSpriteFrameName("lamp.png");
    repeat->setAnchorPoint(Vec2(0,0));
    repeat->setPosition(Vec2(repeat->getContentSize().width * 4, 0));
    _foreground->addChild(repeat, kBackground);
    
    repeat = Sprite::createWithSpriteFrameName("lamp.png");
    repeat->setAnchorPoint(Vec2(0,0));
    repeat->setPosition(Vec2(repeat->getContentSize().width * 8, 0));
    _foreground->addChild(repeat, kBackground);
    
    
    //add clouds
    _clouds = Vector<Sprite*>(4);
    float cloud_y;
    for (int i = 0; i < 4; i++) {
        cloud_y = i % 2 == 0 ? _screenSize.height * 0.7f : _screenSize.height * 0.8f;
        auto cloud = Sprite::createWithSpriteFrameName("cloud.png");
        cloud->setPosition(Vec2 (_screenSize.width * 0.15f + i * _screenSize.width * 0.25f,  cloud_y));
        _gameBatchNode->addChild(cloud, kBackground);
        _clouds.pushBack(cloud);
    }
    
    
    _terrain = Terrain::create();
    _gameBatchNode->addChild(_terrain, kMiddleground);
    
    _player = Player::create();
    _gameBatchNode->addChild(_player, kBackground);
    
    _intro = Sprite::createWithSpriteFrameName("logo.png");
    _intro->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.7f));
    _gameBatchNode->addChild(_intro, kForeground);
    
    
    _tryAgain = Sprite::createWithSpriteFrameName("label_tryagain.png");
    _tryAgain->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.7f));
    _tryAgain->setVisible(false);
    this->addChild(_tryAgain, kForeground);
    
    
    _scoreDisplay = Label::createWithBMFont("font.fnt", "000000", TextHAlignment::CENTER);
    _scoreDisplay->setWidth(_screenSize.width * 0.3f);
    _scoreDisplay->setAnchorPoint(Vec2(1,0));
    _scoreDisplay->setPosition(Vec2(_screenSize.width * 0.95f, _screenSize.height * 0.88f));
    this->addChild(_scoreDisplay, kBackground);
    
    
    _hat = Sprite::createWithSpriteFrameName("hat.png");
    _hat->setVisible(false);
    _gameBatchNode->addChild(_hat, kMiddleground);
    
    
    _jam = Sprite::createWithSpriteFrameName("jam_1.png");
    
    auto animation = Animation::create();
    for(int i = 1; i <= 3; i++) {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("jam_%i.png", i)->getCString());
        animation->addSpriteFrame(frame);
    }
    
    animation->setDelayPerUnit(0.2f / 3.0f);
    animation->setRestoreOriginalFrame(false);
    animation->setLoops(-1);
    
    _jamAnimate = Animate::create(animation);
    _jamAnimate->retain();
    _gameBatchNode->addChild(_jam, kBackground);
    
    _jam->setPosition(Vec2(_screenSize.width * 0.19f, _screenSize.height * 0.47f));
    _jamMove = MoveTo::create(6.0f, Vec2(-_screenSize.width * 0.3f, _jam->getPositionY()));
    _jamMove->retain();
    
    //add menu
    auto menuItemOn = Sprite::createWithSpriteFrameName("btn_new_on.png");
    auto menuItemOff = Sprite::createWithSpriteFrameName("btn_new_off.png");
    
    auto starGametItem = MenuItemSprite::create(
                                                                menuItemOff,
                                                                menuItemOn,
                                                                CC_CALLBACK_1(GameLayer::startGame, this));
    
    
    menuItemOn = Sprite::createWithSpriteFrameName("btn_howto_on.png");
    menuItemOff = Sprite::createWithSpriteFrameName("btn_howto_off.png");
    
    auto howToItem = MenuItemSprite::create(
                                                            menuItemOff,
                                                            menuItemOn,
                                                            CC_CALLBACK_1(GameLayer::showTutorial, this));
    
    _mainMenu = Menu::create(howToItem, starGametItem, nullptr);
    _mainMenu->alignItemsHorizontallyWithPadding(120);
    _mainMenu->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.54));

    this->addChild(_mainMenu, kForeground);
      
    _tutorialLabel = Label::createWithTTF("", "fonts/Times.ttf", 60);
    _tutorialLabel->setPosition(Vec2 (_screenSize.width * 0.5f, _screenSize.height * 0.6f) );
    this->addChild(_tutorialLabel, kForeground);
    _tutorialLabel->setVisible(false);
    
}
