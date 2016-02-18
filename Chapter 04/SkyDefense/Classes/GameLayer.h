#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "SimpleAudioEngine.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace CocosDenshion;


enum {
    kSpriteBomb,
    kSpriteShockwave,
    kSpriteMeteor,
    kSpriteHealth,
    kSpriteHalo,
    kSpriteSparkle,
    kSpriteUfo,
    kSpriteRay
};

enum {

    kBackground,
    kMiddleground,
    kForeground
};



class GameLayer : public Layer {
    
private:

	Vector<Sprite*>_meteorPool;
    int _meteorPoolIndex;
    
    Vector<Sprite*> _healthPool;
    int _healthPoolIndex;
    
	Vector<Sprite*> _fallingObjects;
    Vector<Sprite*> _clouds;
    
    Sprite* _ufo;

    SpriteBatchNode* _gameBatchNode;
    Sprite* _bomb;
    Sprite* _shockWave;
    
    Sprite* _introMessage;
    Sprite* _gameOverMessage;
    
    Label* _energyDisplay;
    Label* _scoreDisplay;
    
    Action* _growBomb;
    Action* _rotateSprite;
    Action* _shockwaveSequence;
    Action* _swingHealth;
    Action* _ufoAnimation;
    Action* _blinkRay;

    Action* _groundHit;
    Action* _explosion;
    
    Size _screenSize;
    
	float _meteorInterval;
	float _meteorTimer;
	float _meteorSpeed;
    float _ufoInterval;
    float _ufoTimer;
    float _healthInterval;
    float _healthTimer;
    float _healthSpeed;
    float _difficultyInterval;
    float _difficultyTimer;

	float _energy;
    int _score;
    int _shockwaveHits;
    bool _running;
    bool _ufoKilled;
    
	void resetMeteor(void);
    void resetUfo(void);
    void resetHealth(void);
    void resetGame(void);
    void stopGame(void);
    void increaseDifficulty(void);
    
    void createGameScreen (void);
    void createPools(void);
    void createActions(void);
    void changeEnergy (float value);

public:
    
    virtual ~GameLayer();
    GameLayer();
    
    virtual bool init();

    static cocos2d::Scene* scene();

    CREATE_FUNC(GameLayer);
    
    void fallingObjectDone(Node* pSender);
    void animationDone(Node* pSender);
    void shockwaveDone();
    
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void update (float dt);
};

#endif // __GAMELAYER_H__


		

