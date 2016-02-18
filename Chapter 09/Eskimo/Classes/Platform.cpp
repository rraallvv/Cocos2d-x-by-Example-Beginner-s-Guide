//
//  Platform.cpp
//  MiniPool
//
//  Created by Roger Engelbert on 1/13/13.
//
//

#include "Platform.h"
#include "GameLayer.h"

Platform::~Platform() {
    
    _tiles.clear();
}

Platform::Platform (GameLayer * game) : b2Sprite (game) {
    
    //create body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    
    _body = game->getWorld()->CreateBody(&bodyDef);
    _body->SetSleepingAllowed(true);
    _body->SetUserData(this);
    
    //register game notifications
    auto onGravityChanged = [=] (EventCustom * event) {
        
        if (this->isVisible()) {
            switchTexture();
        }
    };
    ADD_NOTIFICATION(this, GameLayer::NOTIFICATION_GRAVITY_SWITCH, onGravityChanged);

}

Platform* Platform::create(GameLayer * game) {
    
    auto sprite = new Platform(game);
	if (sprite && sprite->initWithSpriteFrameName("blank.png")) {
		sprite->autorelease();
        sprite->createTiles();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}



void Platform::initPlatform(int width, float angle, Point position) {
    
    //Define shape
    b2PolygonShape box;
    box.SetAsBox(width * 0.5f /PTM_RATIO, PLATFORM_HEIGHT * 0.5f / PTM_RATIO);
    
    //Define fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1;
    fixtureDef.restitution = 0;
    
    //reutilize body from the pool: so destroy any existent fixture
    if (_body->GetFixtureList()) {
        _body->DestroyFixture(_body->GetFixtureList());
    }
    
    _body->CreateFixture(&fixtureDef);
    _body->SetTransform(b2Vec2(position.x / PTM_RATIO, position.y / PTM_RATIO), CC_DEGREES_TO_RADIANS(-angle));
    _body->SetActive(true);
    Sprite * block;
    float startX = -width * 0.5f + TILE * 0.5f;
    
   //set unused tiles in the platform invisible
    for (int i = 0; i < TILES_PER_PLATFORM; i++) {
        
        block =_tiles.at(i);
        
        if (i >= width/TILE) {
            block->setVisible(false);
        } else {
            block->setVisible(true);
            block->setPosition(Vec2(startX + i * TILE,
                                   0));
            block->setFlippedX(false);
            block->setFlippedY(false);
            
            if (angle == 0.0) {
                if (position.y >= TILE * 13) block->setFlippedY(true);
            }
            if (angle == 90.0) {
                if (position.x > TILE * 5) block->setFlippedX(true);
            }
        }
    }
    switchTexture();
    
    this->setRotation(angle);
    this->setPosition(position);
    setVisible(true);
}

//texture platform with tiles
void Platform::switchTexture() {
    for (auto block : _tiles) {
        if (block->isVisible()) {
           
            block->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName (String::createWithFormat("block_%i.png", (int)_game->getGravity())->getCString()));
        }
        
    }
}

//create platform with maximum number of tiles a platform can have (larger side / tile size). 
void Platform::createTiles() {
    _tiles = Vector<Sprite *>(TILES_PER_PLATFORM);
    for (int i = 0; i < TILES_PER_PLATFORM; i++) {
        auto block = Sprite::createWithSpriteFrameName("block_1.png");
        block->setVisible(false);
        _tiles.pushBack(block);
        this->addChild(block, kBackground, kSpriteBlock);
    }
}

