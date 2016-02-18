//
//  Igloo.h
//  Eskimo
//
//  Created by Roger Engelbert on 1/23/13.
//
//

#ifndef __Eskimo__Igloo__
#define __Eskimo__Igloo__

#include "cocos2d.h"
#include "GameConfig.h"

class Eskimo;
class GameLayer;

USING_NS_CC;

class Igloo : public Sprite {
public:
    
    virtual ~Igloo();
    Igloo(GameLayer * game);
    
    static Igloo* create(GameLayer * game);
    void initIgloo(int gravity, Point position);
    
private:
    Sprite * _block;
    Sprite * _iglooOn;
    GameLayer * _game;
    inline void addBlock () {
        _block = Sprite::createWithSpriteFrameName("block_large_1.png");
        _block->setAnchorPoint(Vec2(0,0));
        _block->setPosition(Vec2(0, -TILE * 0.75f));
        this->addChild(_block, kBackground);
    }
};

#endif /* defined(__Eskimo__Igloo__) */
