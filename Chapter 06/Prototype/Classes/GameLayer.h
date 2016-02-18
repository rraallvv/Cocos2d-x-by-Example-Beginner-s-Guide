#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "Terrain.h"
#include "Player.h"

USING_NS_CC;


class GameLayer : public cocos2d::Layer
{
    
    Terrain * _terrain;
    Player * _player;
    
    SpriteBatchNode * _gameBatchNode;
    
    Size _screenSize;
    
    bool _running;
    int _speedIncreaseInterval;
	float _speedIncreaseTimer;
    
    void createGameScreen();
    void resetGame();
    
    
public:
    
    virtual ~GameLayer();
    
    virtual bool init();

    static Scene* scene();
    
    CREATE_FUNC(GameLayer);
    
    void update (float dt);
    
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
};


#endif // __GAMESCENE_H__


