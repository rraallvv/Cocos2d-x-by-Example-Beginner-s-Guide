#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Terrain.h"
#include "Player.h"

USING_NS_CC;
using namespace CocosDenshion;

typedef enum {
    kGameIntro,
    kGamePlay,
    kGameOver,
    kGameTutorial,
    kGameTutorialJump,
    kGameTutorialFloat,
    kGameTutorialDrop
    
} GameState;


class GameLayer : public Layer
{
    
    Terrain * _terrain;
    Player * _player;
    Label * _scoreDisplay;
    
    Sprite * _intro;
    Sprite * _tryAgain;
    Sprite * _background;
    Sprite * _foreground;
    Sprite * _hat;
    Sprite * _jam;
    Action * _jamAnimate;
    Action * _jamMove;
    
    Vector<Sprite *> _clouds;
    
    SpriteBatchNode * _gameBatchNode;
    Label * _tutorialLabel;
    Menu* _mainMenu;
    
    Size _screenSize;
    
    GameState _state;
    
    bool _running;
    float _score;
    int _speedIncreaseInterval;
	float _speedIncreaseTimer;
    
    void createGameScreen();
    void resetGame();
    
    
    
public:
    
    virtual ~GameLayer();
    
    virtual bool init() override;

    static Scene* scene();
    
    CREATE_FUNC(GameLayer);
    
    void update (float dt);
    
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    
    void showTutorial (Object* pSender);
    void startGame (Object* pSender);
};


#endif // __GAMESCENE_H__


