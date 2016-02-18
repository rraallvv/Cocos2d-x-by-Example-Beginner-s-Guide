//
//  GSwitch.cpp
//  MiniPool
//
//  Created by Roger Engelbert on 1/13/13.
//
//

#include "GSwitch.h"
#include "GameLayer.h"

GSwitch::~GSwitch() {
}

GSwitch::GSwitch() {
}

GSwitch* GSwitch::create() {
    auto sprite = new GSwitch();
	if (sprite && sprite->initWithSpriteFrameName("blank.png")) {
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}


void GSwitch::initGSwitch(int direction, Point position) {
    _direction = direction;
    this->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName (String::createWithFormat("switch_%i.png", direction)->getCString()));
    this->setPosition(position);
    setVisible(true);
}
