//
//  GameLayerScene.h
//  Eskimo
//
//  Created by Roger Engelbert on 1/17/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "Box2D/Box2D.h"

class GSwitch;
class Platform;
class Eskimo;
class Igloo;

USING_NS_CC;
using namespace CocosDenshion;



class GameLayer : public cocos2d::Layer {
public:
	static const char* NOTIFICATION_GRAVITY_SWITCH;
	static const char* NOTIFICATION_LEVEL_COMPLETED;

    CC_SYNTHESIZE(b2World *, _world, World);
    CC_SYNTHESIZE(int, _gravity, Gravity);
    CC_SYNTHESIZE(int, _levelsCompleted, LevelsCompleted);
    
    virtual ~GameLayer();
    GameLayer();
    
    static GameLayer * create(int level, int levelsCompleted);
    static Scene* scene(int level, int levelsCompleted);
    
    //virtual void draw();
    bool onTouchBegan(Touch * touch, Event* event);
    void onTouchEnded(Touch * touch, Event* event);
    void onAcceleration(Acceleration *acc, Event *event);
    void update(float dt);
    void loadLevel(int level);
    void newLevel(void);

        
private:
    
    SpriteBatchNode * _gameBatchNode;
    
    Sprite * _btnStart;
    Sprite * _btnReset;
    Sprite * _btnPause;
    Sprite * _btnAgain;
    Sprite * _btnMenu;
    Label * _messages;
    Label * _tutorialLabel;
    
    ParticleSystem * _smoke;
    
    Vector<Platform *> _platformPool;
    Vector<GSwitch *> _gSwitchPool;
    ValueVector _levels;
    Vector<Sprite *> _buttons;
    
    Eskimo * _player;
    Igloo * _igloo;
    
    Size _screenSize;
    bool _running;
    int _gameState;
    int _platformPoolIndex;
    int _currentLevel;
    int _tutorialStep;
    float _tutorialCounter;
    
    Point _acceleration;
    
    void initPhysics();
    void createScreen(void);
    void resetLevel(void);
    void levelCompleted(void);
    void createPools(void);
    void clearLayer(void);
    
};

#endif // __GAMELAYER_H__
