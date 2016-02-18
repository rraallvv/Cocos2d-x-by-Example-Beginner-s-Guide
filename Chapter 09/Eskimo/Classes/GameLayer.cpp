/*
Soundtrack:
Merry Go Slower
by Kevin MacLeod (incompetech.com)

*/
#include "GameLayer.h"
#include "Platform.h"
#include "GSwitch.h"
#include "MenuLayer.h"
#include "Eskimo.h"
#include "Igloo.h"

static GameLayer* _instance = NULL;

const char* GameLayer::NOTIFICATION_GRAVITY_SWITCH = "NOTIFICATION_GRAVITY_SWITCH";
const char* GameLayer::NOTIFICATION_LEVEL_COMPLETED = "NOTIFICATION_LEVEL_COMPLETED";

GameLayer::~GameLayer()
{
    _platformPool.clear();
    _gSwitchPool.clear();
    _buttons.clear();
    CC_SAFE_RELEASE(_player);
    
    delete _world;
    _world = nullptr;
}

GameLayer::GameLayer() {
    _screenSize = Director::getInstance()->getWinSize();
    _running = false;
    
    createScreen();
    _levels = FileUtils::getInstance()->getValueVectorFromFile("levels.plist");
    
    initPhysics();
    
    createPools();
    
    //add events
    Device::setAccelerometerEnabled(true);
    auto listenerAccelerometer = EventListenerAcceleration::create(CC_CALLBACK_2(GameLayer::onAcceleration, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerAccelerometer, this);
    
    auto listenerTouches = EventListenerTouchOneByOne::create();
    listenerTouches->setSwallowTouches(true);
    listenerTouches->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    listenerTouches->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouches, this);
    
    
}

GameLayer * GameLayer::create (int level, int levelsCompleted) {
    if (!_instance) {
        _instance = new GameLayer();
    } else {
        _instance->clearLayer();
    }
    _instance->setLevelsCompleted(levelsCompleted);
    _instance->loadLevel(level);
    _instance->scheduleUpdate();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true);
    return _instance;
}

//create Box2D world with default gravity
void GameLayer::initPhysics() {
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -FORCE_GRAVITY);
    _world = new b2World(gravity);
    
    _world->SetAllowSleeping(true);
    _world->SetContinuousPhysics(true);
    
    _player = Eskimo::create(this);
    _gameBatchNode->addChild(_player, kMiddleground);
}

//
void GameLayer::loadLevel (int level) {
    
    clearLayer();
    
    _currentLevel = level;
    resetLevel();
    
   ValueMap  levelData = _levels.at(_currentLevel).asValueMap();
    
    //create platforms
    ValueVector platforms = levelData.at("platforms").asValueVector();
    for ( auto platformData : platforms)
    {
        auto platform = _platformPool.at(_platformPoolIndex);
        _platformPoolIndex++;
        if (_platformPoolIndex == _platformPool.size()) _platformPoolIndex = 0;
        
        ValueMap data = platformData.asValueMap();
        
        platform->initPlatform (
                                data.at("width").asInt() * TILE,
                                data.at("angle").asFloat(),
                                Vec2(data.at("x").asFloat() * TILE,
                                     data.at("y").asFloat() * TILE)
                                );
        
    }
    
}

void GameLayer::resetLevel() {
    /*
     level initialization is broken into loadLevel and resetLevel because there are
     actions that need to be taken only when loading the new level, and actions that
     need to be taken when loading a new level AND resetting the level (when user presses
     play again or the reset level button)
     */
    
    _btnReset->setVisible(false);
    _btnPause->setVisible(false);
    
    ValueMap levelData = _levels.at(_currentLevel).asValueMap();
    
    //set up world gravity based on level data value
    _gravity = levelData.at("gravity").asInt();
    
    int i = 1;
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent( GameLayer::NOTIFICATION_GRAVITY_SWITCH, &i);

    switch (_gravity) {
        case kDirectionUp:
            _world->SetGravity(b2Vec2(0,FORCE_GRAVITY));
            break;
        case kDirectionDown:
            _world->SetGravity(b2Vec2(0,-FORCE_GRAVITY));
            break;
        case kDirectionLeft:
            _world->SetGravity(b2Vec2(-FORCE_GRAVITY, 0));
            break;
        case kDirectionRight:
            _world->SetGravity(b2Vec2(FORCE_GRAVITY, 0));
            break;
    }
    
    //create switches
    ValueVector switches = levelData.at("switches").asValueVector();

    for (int i = 0; i < switches.size(); i++) {
        auto gswitch = _gSwitchPool.at(i);
        ValueMap data = switches.at(i).asValueMap();
        if ( i< switches.size())
        {
            gswitch->initGSwitch(
                                 data.at("gravity").asInt(),
                                 Vec2(data.at("x").asFloat() * TILE,
                                      data.at("y").asFloat() * TILE
                                      ));
            
        }
        else
        {
            gswitch->setVisible(false);
        }
        i++;

    }

    //reset player to level start position
    _player->setSpritePosition(Vec2(levelData.at("startx").asFloat() * TILE,
                                   levelData.at("starty").asFloat() * TILE));
    _player->setVisible(true);
    _player->reset();

    //reset igloo to level end position
    _igloo->initIgloo(_gravity, Vec2(levelData.at("endx").asFloat() * TILE,
                                    levelData.at("endy").asFloat() * TILE));
    //reset smoke particle to match igloo position
    _smoke->setPosition(Vec2(_igloo->getPositionX() + TILE,
                            _igloo->getPositionY() + TILE * 0.5f));
    //hide smoke. only shown when level is completed
    _smoke->setVisible(false);
    _smoke->stopSystem();

    //if _currentLevel == 0 this is tutorial level
    if (_currentLevel != 0) {
        _messages->setString(String::createWithFormat("level %i", (int) _currentLevel)->getCString());
        
    } else {
        _messages->setString("tutorial");
        _tutorialStep = 0;
        _tutorialCounter = 0.0;
    }
    _tutorialLabel->setVisible(false);
    
    _messages->setVisible(true);
    _btnStart->setVisible(true);
    _btnMenu->setVisible(false);
    _btnAgain->setVisible(false);
    
    _acceleration = Vec2(0,0);
}

void GameLayer::update(float dt) {
    
    if (!_running) return;
    
    _world->Step(dt, 10, 10);
    _world->ClearForces();
    
    _player->update();
    
    //if player is touching something, update it with accelerometer data
    if (_player->getBody()->GetContactList()) {
        b2Vec2 vec;
        
        switch (_gravity) {
        	//update X axis
            case kDirectionUp:
            case kDirectionDown:
            //if player is a circle, slow it down.
                if (_player->getState() == kStateCircle) {
                    _player->getBody()->ApplyForce(b2Vec2(_acceleration.x * CIRCLE_MULTIPLIER, 0),
                                                   _player->getBody()->GetWorldCenter(), true);
                    
                    vec = _player->getBody()->GetLinearVelocity();
                    vec.x *= BALL_FRICTION;
                    _player->getBody()->SetLinearVelocity(vec);
            //if player is a box don't apply any friction    
                } else {
                    _player->getBody()->ApplyLinearImpulse(b2Vec2(_acceleration.x * BOX_MULTIPLIER, 0),
                                                           _player->getBody()->GetWorldCenter(), true);
                }
                break;
                
            //update Y axis
            case kDirectionLeft:
            case kDirectionRight:
            //if player is a circle, slow it down.
                if (_player->getState() == kStateCircle) {
                    _player->getBody()->ApplyForce(b2Vec2(0, _acceleration.y * CIRCLE_MULTIPLIER),
                                                   _player->getBody()->GetWorldCenter(), true);
                    
                    vec = _player->getBody()->GetLinearVelocity();
                    vec.y *= BALL_FRICTION;
                    _player->getBody()->SetLinearVelocity(vec);
            //if player is a box don't apply any friction  
                } else {
                    _player->getBody()->ApplyLinearImpulse(b2Vec2(0, _acceleration.y * BOX_MULTIPLIER),
                                                           _player->getBody()->GetWorldCenter(), true);
                }
                
                break;
        }
    }
    
    //track collision with GSwitches
    float diffx;
    float diffy;
    
    for (auto gSwitch : _gSwitchPool) {
        if (!gSwitch->isVisible()) continue;
        diffx = gSwitch->getPositionX() - _player->getPositionX();
        diffy = gSwitch->getPositionY() - _player->getPositionY();
        if (pow(diffx, 2) + pow (diffy, 2) < PLAYER_SWITCH_RADII) {
            SimpleAudioEngine::getInstance()->playEffect("switch.wav");
            _gravity = gSwitch->getDirection();
            gSwitch->setVisible(false);
            //notify of collision

            EVENT_DISPATCHER->dispatchCustomEvent(GameLayer::NOTIFICATION_GRAVITY_SWITCH);
			//change world gravity
            switch (_gravity) {
                case kDirectionUp:
                    _world->SetGravity(b2Vec2(0, FORCE_GRAVITY));
                    break;
                case kDirectionDown:
                    _world->SetGravity(b2Vec2(0, -FORCE_GRAVITY));
                    break;
                case kDirectionLeft:
                    _world->SetGravity(b2Vec2(-FORCE_GRAVITY, 0 ));
                    break;
                case kDirectionRight:
                    _world->SetGravity(b2Vec2(FORCE_GRAVITY, 0 ));
                    break;
            }
            break;
        }
    }
    
    //check for level complete (collision with Igloo)
    diffx = _player->getPositionX() - _igloo->getPositionX();
    diffy = _player->getPositionY() - _igloo->getPositionY();
    if (pow(diffx, 2) + pow(diffy, 2) < IGLOO_SQ_RADIUS) {
        SimpleAudioEngine::getInstance()->playEffect("igloo.wav");
        
        _player->setVisible(false);
        _btnPause->setVisible(false);
        _btnReset->setVisible(false);
        
        //notify of Level Completed. Igloo will listen to this and change its texture.
        EVENT_DISPATCHER->dispatchCustomEvent(GameLayer::NOTIFICATION_LEVEL_COMPLETED);
        
        //run smoke particle
        _smoke->setVisible(true);
        _smoke->resetSystem();
        _messages->setString("well done!");
        _messages->setVisible(true);
        
        //create delay until new level is loaded
        auto  sequence = Sequence::create(
                                                           DelayTime::create(2.5f),
                                                            CallFunc::create(std::bind(&GameLayer::newLevel, this) ),
                                                           nullptr);
        this->runAction(sequence);
        _tutorialLabel->setVisible(false);
        _running = false;
    }
    
    //check for game over. player is off screen
    if (_player->getPositionY() > _screenSize.height || _player->getPositionY() < 0
        || _player->getPositionX() > _screenSize.width || _player->getPositionX() < 0) {
        SimpleAudioEngine::getInstance()->playEffect("oops.wav");
        _running = false;
        _player->setVisible(false);
        _btnPause->setVisible(false);
        _btnReset->setVisible(false);
        
        _messages->setString("oops!");
        _messages->setVisible(true);
        _btnAgain->setVisible(true);
        _btnMenu->setVisible(true);
    }
    
    //update text in tutorial level (currentLevel == 0)
    if (_currentLevel == 0) {
        _tutorialCounter += dt;
        if (_tutorialCounter > 15 && _tutorialStep == 1) {
            _tutorialStep = 2;
            _tutorialLabel->setString(TUTORIAL_3);
        } else if (_tutorialCounter > 5 && _tutorialStep == 0) {
            _tutorialStep = 1;
            _tutorialLabel->setString(TUTORIAL_2);
        }
    }
    
}


void GameLayer::newLevel() {
    
    if (_currentLevel + 1 >= _levels.size()) return;
    
    loadLevel(_currentLevel + 1);
    
    //update user data
    if (_currentLevel > _levelsCompleted) {
        _levelsCompleted = _currentLevel;
        UserDefault::getInstance()->setIntegerForKey("levelsCompleted", _levelsCompleted);
        UserDefault::getInstance()->flush();
    }
}

bool GameLayer::onTouchBegan(Touch * touch, Event * event) {
    
    if (touch) {
        
	    Point tap = touch->getLocation();
        Rect boundary;
        //handle button touches
        for (int i = 0; i < 5; i++) {
            auto button = _buttons.at(i);
            if (!button->isVisible()) continue;
            boundary = button->boundingBox();
            
            if (boundary.containsPoint(tap)) {
                auto buttonPress = button->getChildByTag(kSpriteBtnOn);
                buttonPress->setVisible(true);
                return true;
            }
        }
        return true;
    }
    return false;
}

void GameLayer::onTouchEnded(Touch * touch, Event * event) {
    
    if (touch) {
        
	    Point tap = touch->getLocation();
        Rect boundary;
        //handle button touches
        for (int i = 0; i < 5; i++) {
            auto button = _buttons.at(i);
            if (!button->isVisible()) continue;
            boundary = button->boundingBox();
            if (boundary.containsPoint(tap)) {
                auto buttonPress = button->getChildByTag(kSpriteBtnOn);
                buttonPress->setVisible(false);
                switch (button->getTag()) {
                    case kSpriteBtnAgain:
                        _running = false;
                        _btnAgain->setVisible(false);
                        _btnMenu->setVisible(false);
                        resetLevel();
                        SimpleAudioEngine::getInstance()->playEffect("button.wav");
                        break;
                    case kSpriteBtnReset:
                        _running = false;
                        resetLevel();
                        break;
                    case kSpriteBtnPause:
                        if (_running) {
                            _messages->setString("paused");
                            _messages->setVisible(true);
                        } else {
                            _messages->setVisible(false);
                        }
                        _running = !_running;
                        break;
                    case kSpriteBtnStart:
                        SimpleAudioEngine::getInstance()->playEffect("button.wav");
                        _btnPause->setVisible(true);
                        _btnReset->setVisible(true);
                        _btnStart->setVisible(false);
                        _messages->setVisible(false);
                        _running = true;
                        if (_currentLevel == 0) {
                            _tutorialLabel->setVisible(true);
                        }
                        break;
                    case kSpriteBtnMenu:
                        SimpleAudioEngine::getInstance()->playEffect("button.wav");
                        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
                        unscheduleUpdate();
                        Scene* newScene = TransitionMoveInL::create(0.2f, MenuLayer::scene());
                        Director::getInstance()->replaceScene(newScene);
                }
                return;
            }
        }
        
        if (!_running) return;
        _player->setSwitchShape(true);
        
    }
}

void GameLayer::onAcceleration(Acceleration *acc, Event *event) {
    _acceleration = Vec2(acc->x * ACCELEROMETER_MULTIPLIER,
                        acc->y * ACCELEROMETER_MULTIPLIER);
}

Scene* GameLayer::scene(int level, int levelsCompleted)
{
    // 'scene' is an autorelease object
    Scene *scene = Scene::create();
    
    // add layer as a child to scene
    scene->addChild(GameLayer::create(level, levelsCompleted));
    
    return scene;
}

void GameLayer::createPools() {
    _gSwitchPool = Vector<GSwitch*>(30);
    for (int i = 0; i < 30; i++) {
        auto sprite = GSwitch::create();
        sprite->setVisible(false);
        _gSwitchPool.pushBack(sprite);
        _gameBatchNode->addChild(sprite);
    }
    
    _platformPool = Vector<Platform *>(50);
    for (int  i = 0; i < 50; i++) {
        auto platform = Platform::create(this);
        platform->setVisible(false);
        _gameBatchNode->addChild(platform);
        _platformPool.pushBack(platform);
    }
    _platformPoolIndex = 0;
    
}

void GameLayer::createScreen() {
    //if we load this scene first, load sprite frames
    //SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("sprite_sheet.plist");
    
    Sprite * bg = Sprite::create("bg.jpg");
    bg->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg);
    
    _gameBatchNode = SpriteBatchNode::create("sprite_sheet.png", 200);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _igloo = Igloo::create(this);
    _igloo->setVisible(false);
    _gameBatchNode->addChild(_igloo, kForeground);
    
    //create buttons
    _buttons = Vector<Sprite *>(5);
    
    _btnStart = Sprite::createWithSpriteFrameName("btn_start_off.png");
    _btnStart->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.45f));
    _btnStart->setVisible(false);
    _gameBatchNode->addChild(_btnStart, kForeground, kSpriteBtnStart);
    auto buttonOn = Sprite::createWithSpriteFrameName("btn_start_on.png");
    buttonOn->setAnchorPoint(Vec2(0,0));
    buttonOn->setVisible(false);
    _btnStart->addChild(buttonOn, kForeground, kSpriteBtnOn);
    _buttons.pushBack(_btnStart);
    
    _btnReset = Sprite::createWithSpriteFrameName("btn_reset_off.png");
    _btnReset->setAnchorPoint(Vec2(0.5, 1));
    _btnReset->setPosition(Vec2(_screenSize.width * 0.9f, _screenSize.height));
    _btnReset->setVisible(false);
    _gameBatchNode->addChild(_btnReset, kForeground, kSpriteBtnReset);
    buttonOn = Sprite::createWithSpriteFrameName("btn_reset_on.png");
    buttonOn->setAnchorPoint(Vec2(0,0));
    buttonOn->setVisible(false);
    _btnReset->addChild(buttonOn, kForeground, kSpriteBtnOn);
    _buttons.pushBack(_btnReset);
    
    _btnPause = Sprite::createWithSpriteFrameName("btn_pause_off.png");
    _btnPause->setAnchorPoint(Vec2(0.5, 1));
    _btnPause->setPosition(Vec2(_screenSize.width * 0.1f, _screenSize.height));
    _btnPause->setVisible(false);
    _gameBatchNode->addChild(_btnPause, kForeground, kSpriteBtnPause);
    buttonOn = Sprite::createWithSpriteFrameName("btn_pause_on.png");
    buttonOn->setAnchorPoint(Vec2(0,0));
    buttonOn->setVisible(false);
    _btnPause->addChild(buttonOn, kForeground, kSpriteBtnOn);
    _buttons.pushBack(_btnPause);
    
    _btnAgain = Sprite::createWithSpriteFrameName("btn_again_off.png");
    _btnAgain->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.45f));
    _btnAgain->setVisible(false);
    _gameBatchNode->addChild(_btnAgain, kForeground, kSpriteBtnAgain);
    buttonOn = Sprite::createWithSpriteFrameName("btn_again_on.png");
    buttonOn->setAnchorPoint(Vec2(0,0));
    buttonOn->setVisible(false);
    _btnAgain->addChild(buttonOn, kForeground, kSpriteBtnOn);
    _buttons.pushBack(_btnAgain);
    
    _btnMenu = Sprite::createWithSpriteFrameName("btn_menu_off.png");
    _btnMenu->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.3f));
    _btnMenu->setVisible(false);
    _gameBatchNode->addChild(_btnMenu, kForeground, kSpriteBtnMenu);
    buttonOn = Sprite::createWithSpriteFrameName("btn_menu_on.png");
    buttonOn->setAnchorPoint(Vec2(0,0));
    buttonOn->setVisible(false);
    _btnMenu->addChild(buttonOn, kForeground, kSpriteBtnOn);
    _buttons.pushBack(_btnMenu);
    
    _messages = Label::createWithBMFont("font_messages.fnt", "level", TextHAlignment::CENTER);
    _messages->setAnchorPoint(Vec2(0.5f, 0.5f));
    _messages->setVisible(false);
    _messages->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.6f));
    this->addChild(_messages, kForeground);
    
    _smoke = ParticleSystemQuad::create("smoke.plist");
    _smoke->setVisible(false);
    _smoke->stopSystem();
    this->addChild(_smoke, kForeground);
    
    
    _tutorialLabel = Label::createWithTTF(TUTORIAL_1, "fonts/Verdana.ttf", 25);
    _tutorialLabel->setWidth(_screenSize.width * 0.7f);
    _tutorialLabel->setHeight(_screenSize.height * 0.4f);
    _tutorialLabel->setAlignment(TextHAlignment::CENTER);
    _tutorialLabel->setPosition(Vec2 (_screenSize.width * 0.5f, _screenSize.height * 0.2f) );
    this->addChild(_tutorialLabel, kForeground);
    _tutorialLabel->setVisible(false);
}

//hide elements from previous level
void GameLayer::clearLayer() {
    
    for (auto sprite : _platformPool) {
        sprite->getBody()->SetActive(false);
        sprite->setVisible(false);
    }
    
    for (auto sprite : _gSwitchPool) {
        sprite->setVisible(false);
    }
}
