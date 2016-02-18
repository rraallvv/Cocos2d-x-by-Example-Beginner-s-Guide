#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "GameSprite.h"
#include <string>

using namespace CocosDenshion;

GameLayer::~GameLayer() {
    _players.clear();
}

GameLayer::GameLayer ()
{
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
    if ( !Layer::init() )
    {
        return false;
    }
    
    _players = Vector<GameSprite*>(2);
    
	//Init Game Elements
	_player1Score = 0;
    _player2Score = 0;
    
	//get screen size
	_screenSize = Director::getInstance()->getWinSize();
    
	//1. add court image
    auto court = Sprite::create("court.png");
    court->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(court);
    
    
	//2. add players
	_player1 =  GameSprite::gameSpriteWithFile("mallet.png");
	_player1->setPosition(Vec2(_screenSize.width * 0.5, _player1->radius() * 2));
    _players.pushBack(_player1);
    this->addChild(_player1);
    
    _player2 =  GameSprite::gameSpriteWithFile("mallet.png");
	_player2->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2));
    _players.pushBack(_player2);
    this->addChild(_player2);
	

	//3. add puck
	_ball = GameSprite::gameSpriteWithFile("puck.png");
    _ball->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5 - 2 * _ball->radius()));
    this->addChild(_ball);
    
    
	//4. add score display
    _player1ScoreLabel = Label::createWithTTF("0", "fonts/Arial.ttf", 60);
    _player1ScoreLabel->setPosition(Vec2(_screenSize.width - 60, _screenSize.height * 0.5 - 80));
    _player1ScoreLabel->setRotation(90);
    this->addChild(_player1ScoreLabel);
	
    _player2ScoreLabel = Label::createWithTTF("0", "fonts/Arial.ttf", 60);
    _player2ScoreLabel->setPosition(Vec2(_screenSize.width - 60, _screenSize.height * 0.5 + 80));
    _player2ScoreLabel->setRotation(90);
    this->addChild(_player2ScoreLabel);

    
	//listen for touches
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(GameLayer::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(GameLayer::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(GameLayer::onTouchesEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //create main loop
    this->scheduleUpdate();
    
    return true;
}

void GameLayer::onTouchesBegan(const std::vector<Touch*> &touches, Event* event)
{
    //loop through all beginning touches
    for( auto touch : touches) {

        if(touch != nullptr) {

            //get location in OpenGL view
            auto tap = touch->getLocation();
            
            //loop through players and check to see if touch is landing on one of them
            for (auto player : _players) {
                if (player->getBoundingBox().containsPoint(tap)) {
                    //store player's touch
                    player->setTouch(touch);
                }
            }
        }
    }
}

void GameLayer::onTouchesMoved(const std::vector<Touch*> &touches, Event* event)
{
    
    //loop through all moving touches
    for( auto touch : touches) {
        
        if(touch) {
            auto tap = touch->getLocation();
            
            for (auto player : _players) {
                //if touch belongs to player
                if (player->getTouch() != nullptr && player->getTouch() == touch) {

                    Point nextPosition = tap;
                    
                    //keep player inside screen
                    if (nextPosition.x < player->radius())
                        nextPosition.x = player->radius();
                    if (nextPosition.x > _screenSize.width - player->radius())
                        nextPosition.x = _screenSize.width - player->radius();
                    if (nextPosition.y < player->radius())
                        nextPosition.y  = player->radius();
                    if (nextPosition.y > _screenSize.height - player->radius())
                        nextPosition.y = _screenSize.height - player->radius();
                    
                    //keep player inside its court
                    if (player->getPositionY() < _screenSize.height* 0.5f) {
                        if (nextPosition.y > _screenSize.height* 0.5 - player->radius()) {
                            nextPosition.y = _screenSize.height* 0.5 - player->radius();
                        }
                    } else {
                        if (nextPosition.y < _screenSize.height* 0.5 + player->radius()) {
                            nextPosition.y = _screenSize.height* 0.5 + player->radius();
                        }
                    }
                    
                    player->setNextPosition(nextPosition);
                    player->setVector(Vec2(tap.x - player->getPositionX(), tap.y - player->getPositionY()));
                }	 
            }
        }
    }
}

void GameLayer::onTouchesEnded(const std::vector<Touch*> &touches, Event* event)
{
    //loop through all ending touches
    for( auto touch : touches) {
        if(touch) {
             for (auto player : _players) {
                if (player->getTouch() != nullptr && player->getTouch() == touch) {
                    //if touch ending belongs to this player, clear it
                    player->setTouch(nullptr);
                    player->setVector(Vec2(0,0));
                }
            }
        }
    }
}

void GameLayer::update (float dt) {
	
	//update puck
    auto ballNextPosition = _ball->getNextPosition();
    auto ballVector = _ball->getVector();
    ballVector *=  0.98f;
    
    ballNextPosition.x += ballVector.x;
    ballNextPosition.y += ballVector.y;
    
	//test for puck and mallet collision
	float squared_radii = pow(_player1->radius() + _ball->radius(), 2);
    
    for (auto player : _players) {
		
		auto playerNextPosition = player->getNextPosition();
        auto playerVector = player->getVector();
        
		float diffx = ballNextPosition.x - player->getPositionX();
		float diffy = ballNextPosition.y - player->getPositionY();
		
		float distance1 = pow(diffx, 2) + pow(diffy, 2);
		float distance2 = pow(_ball->getPositionX() - playerNextPosition.x, 2) + pow(_ball->getPositionY() - playerNextPosition.y, 2);
		
		if (distance1 <= squared_radii || distance2 <= squared_radii) {
            
          	float mag_ball = pow(ballVector.x, 2) + pow(ballVector.y, 2);
			float mag_player = pow (playerVector.x, 2) + pow (playerVector.y, 2);
          	
			float force = sqrt(mag_ball + mag_player);
			float angle = atan2(diffy, diffx);
			
			ballVector.x = force* cos(angle);
			ballVector.y = (force* sin(angle));
			
            ballNextPosition.x = playerNextPosition.x + (player->radius() + _ball->radius() + force) * cos(angle);
            ballNextPosition.y = playerNextPosition.y + (player->radius() + _ball->radius() + force) * sin(angle);
            
            SimpleAudioEngine::getInstance()->playEffect("hit.wav");
		}
	}
	
	//check collision of ball and sides
	if (ballNextPosition.x < _ball->radius()) {
		ballNextPosition.x = _ball->radius();
        ballVector.x *= -0.8f;
        SimpleAudioEngine::getInstance()->playEffect("hit.wav");
	}
	
	if (ballNextPosition.x > _screenSize.width - _ball->radius()) {
		ballNextPosition.x = _screenSize.width - _ball->radius();
		ballVector.x *= -0.8f;
        SimpleAudioEngine::getInstance()->playEffect("hit.wav");
	}
    //ball and top of the court
	if (ballNextPosition.y > _screenSize.height - _ball->radius()) {
		if (_ball->getPosition().x < _screenSize.width* 0.5f - GOAL_WIDTH* 0.5f ||
			_ball->getPosition().x > _screenSize.width* 0.5f + GOAL_WIDTH* 0.5f) {
			ballNextPosition.y = _screenSize.height - _ball->radius();
			ballVector.y *= -0.8f;
            SimpleAudioEngine::getInstance()->playEffect("hit.wav");
		} 
	}
	//ball and bottom of the court
	if (ballNextPosition.y < _ball->radius() ) {
		if (_ball->getPosition().x < _screenSize.width* 0.5f - GOAL_WIDTH* 0.5f ||
			_ball->getPosition().x > _screenSize.width* 0.5f + GOAL_WIDTH* 0.5f) {
			ballNextPosition.y = _ball->radius();
			ballVector.y *= -0.8f;
            SimpleAudioEngine::getInstance()->playEffect("hit.wav");
		} 
	}
    
    //finally, after all checks, update ball's vector and next position
    _ball->setVector(ballVector);
    _ball->setNextPosition(ballNextPosition);
    
    
    //check for goals!
    if (ballNextPosition.y  < -_ball->radius() * 2) {
        this->playerScore(2);
        
    }
    if (ballNextPosition.y > _screenSize.height + _ball->radius() * 2) {
        this->playerScore(1);
    }
	
    //move pieces to next position
	_player1->setPosition(_player1->getNextPosition());
    _player2->setPosition(_player2->getNextPosition());
	_ball->setPosition(_ball->getNextPosition());    
	
}

void GameLayer::playerScore (int player) {
    
    SimpleAudioEngine::getInstance()->playEffect("score.wav");
    
    _ball->setVector(Vec2(0,0));
    //if player 1 scored...
    if (player == 1) {
    
        _player1Score++;
        _player1ScoreLabel->setString(String::createWithFormat("%i",_player1Score )->getCString());
        //move ball to player 2 court
        _ball->setNextPosition(Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5 + 2 * _ball->radius()));
    
    //if player 2 scored...
    } else {
    
        _player2Score++;
        _player2ScoreLabel->setString(String::createWithFormat("%i",_player2Score )->getCString());
        //move ball to player 1 court
        _ball->setNextPosition(Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5 - 2 * _ball->radius()));
    
    }
    //move players to original position
    _player1->setPosition(Vec2(_screenSize.width * 0.5, _player1->radius() * 2));
    _player2->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2));
	
	//clear current touches
    _player1->setTouch(nullptr);
    _player2->setTouch(nullptr);
}
	
