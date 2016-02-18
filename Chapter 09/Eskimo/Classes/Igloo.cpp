//
//  Igloo.cpp
//  Eskimo
//
//  Created by Roger Engelbert on 1/23/13.
//
//

#include "Igloo.h"
#include "Eskimo.h"
#include "GameLayer.h"


Igloo::~Igloo() {
    EVENT_DISPATCHER->removeCustomEventListeners(GameLayer::NOTIFICATION_GRAVITY_SWITCH);
    EVENT_DISPATCHER->removeCustomEventListeners(GameLayer::NOTIFICATION_LEVEL_COMPLETED);
}

Igloo::Igloo(GameLayer * game) {
    
    _game = game;
    auto onGravityChanged = [=] (EventCustom * event) {
        _block->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName (String::createWithFormat("block_large_%i.png", (int) _game->getGravity())->getCString()));
        
    };
    
    auto onLevelCompleted = [=] (EventCustom * event) {
        this->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName ("igloo_on.png"));
    };

    
    //register game notifications
    ADD_NOTIFICATION(this, GameLayer::NOTIFICATION_GRAVITY_SWITCH, onGravityChanged);

    ADD_NOTIFICATION(this, GameLayer::NOTIFICATION_LEVEL_COMPLETED, onLevelCompleted);
}

Igloo* Igloo::create(GameLayer * game) {
    auto sprite = new Igloo(game);
	if (sprite && sprite->initWithSpriteFrameName("igloo_off.png")) {
		sprite->autorelease();
        sprite->addBlock();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Igloo::initIgloo(int gravity, Point position) {

    _block->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName (    String::createWithFormat("block_large_%i.png", gravity)->getCString()));

    this->setPosition(position);
    this->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName ("igloo_off.png"));
    setVisible(true);
    
}
