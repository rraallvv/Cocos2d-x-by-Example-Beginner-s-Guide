//
//  GameLayer.h
//  MiniPool
//
//  Created by Roger Engelbert on 1/13/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Ball.h"
#include "Cue.h"
#include "LineContainer.h"
#include "GLES-Render.h"

USING_NS_CC;

enum {
    kBackground,
    kMiddleground,
    kForeground
};


enum {
    kGameIntro,
    kGamePlay,
    kGameOver
};

class GameLayer : public Layer {

public:
    
    virtual ~GameLayer();
    GameLayer();
    
    CC_SYNTHESIZE(b2World *, _world, World);
    CC_SYNTHESIZE(bool, _canShoot, CanShoot);
    
    static Scene* scene();
    
    void initPhysics();
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    virtual bool onTouchBegan(Touch * touch, Event * event);
    virtual void onTouchMoved(Touch * touch, Event * event);
    virtual void onTouchEnded(Touch * touch, Event * event);
    void update(float dt);
    void ticktock(float dt);
    
private:

    b2ContactListener * _collisionListener;
    GLESDebugDraw* _debugDraw;
    
    Touch * _touch;
    Vector<Ball *> _balls;
    Vector<b2Sprite *> _pockets;
    SpriteBatchNode * _gameBatchNode;
    Sprite * _intro;
    Sprite * _gameOver;
    Label * _timer;
    
    LineContainer * _lineContainer;
    
    Point _cueStartPoint;
    Size _screenSize;
    bool _running;
    bool _usingCue;
    
    float _pullBack;
    float _cueAngle;
    int _gameState;
    int _ballsInPlay;
    int _time;
    
    Ball * _player;
    Cue * _cue;
    
    void placeCue(Point position);
    void resetGame(void);

    Mat4 _modelViewMV;
    void onDraw();
    CustomCommand _customCommand;
};

#endif // __GAME_LAYER_H__
