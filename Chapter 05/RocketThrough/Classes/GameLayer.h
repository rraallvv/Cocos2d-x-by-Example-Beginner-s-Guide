#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameSprite.h"

USING_NS_CC;

using namespace CocosDenshion;

enum {
    kBackground,
    kMiddleground,
    kForeground
};

enum {
    kSpriteRocket,
    kSpritePlanet,
    kSpriteBoost,
    kSpriteStar
};

typedef enum gamestates {
    kGameIntro,
    kGamePaused,
    kGamePlay,
    kGameOver
    
}GameState;

class Rocket;
class LineContainer;
class GameSprite;


class GameLayer : public Layer {
    
    Rocket * _rocket;
    LineContainer * _lineContainer;
    
    SpriteBatchNode *_gameBatchNode;
    Label * _scoreDisplay;
    
    GameSprite * _pauseBtn;
    Sprite * _intro;
    Sprite *_gameOver;
    Sprite *_paused;
    
    ParticleSystem * _star;
    ParticleSystem * _jet;
    ParticleSystem * _boom;
    ParticleSystem * _comet;
    ParticleSystem * _pickup;
    ParticleSystem * _warp;
    
    Vector<GameSprite *> _planets;
    Size _screenSize;
    
    GameState _state;
    
    bool _drawing;
    bool _running;

    std::vector<Point> _grid;
    int _gridIndex;
    
    int _minLineLength;
    float _cometInterval;
    float _cometTimer;
    
    void resetGame(void);
    void resetStar(void);
    void killPlayer(void);
    
    void createGameScreen(void);
    void createParticles(void);
    void createStarGrid(void);

	int _score;
	float _timeBetweenPickups;
    
public:
    
    GameLayer();
    virtual ~GameLayer();
    
    virtual bool init();
    
    static Scene* scene();
    
    void update (float dt);
	
    virtual bool onTouchBegan(Touch * touch, Event* event);
	virtual void onTouchMoved(Touch * touch, Event* event);
	virtual void onTouchEnded(Touch * touch, Event* event);
    
    CREATE_FUNC(GameLayer);
	

};

#endif // __GAMELAYER_H__
