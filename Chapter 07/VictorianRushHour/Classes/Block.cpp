#define TOTAL_PUFFS 3


#include "Block.h"

Block::~Block () {
    
    _chimneys.clear();
    _wallTiles.clear();
    _roofTiles.clear();
    
    CC_SAFE_RELEASE(_puffAnimation);
    CC_SAFE_RELEASE(_puffSpawn);
    CC_SAFE_RELEASE(_puffMove);
    CC_SAFE_RELEASE(_puffFade);
    CC_SAFE_RELEASE(_puffScale);
}

Block::Block () : _wallTiles(20), _roofTiles(5), _chimneys(5){
    
    //get screen size
	_screenSize = Director::getInstance()->getWinSize();
    _tileWidth = _screenSize.width / TILE_W_SIZE;
    _tileHeight = _screenSize.height / TILE_H_SIZE;
    _puffing = false;
    this->setVisible(false);
}

Block * Block::create () {
    
    auto block = new Block();
	if (block && block->initWithSpriteFrameName("blank.png")) {
		block->autorelease();
        block->initBlock();
		return block;
	}
	CC_SAFE_DELETE(block);
	return nullptr;
}

void Block::setPuffing (bool value) {
    
    _puffing = value;
    
    if (value) {
        this->runAction( _puffSpawn->clone());
        auto hide = Sequence::create(DelayTime::create(2.5f),
                                            CallFunc::create(std::bind(&Block::hidePuffs, this)),
                                             nullptr);
        this->runAction(hide);
    } else {
        //reset all puffs
        _puffIndex = 0;
        for (auto chimney : _chimneys)
        {
            for (int j = 0; j < TOTAL_PUFFS; j++) {
                auto puff = (Sprite *) chimney->getChildByTag(j);
                puff->setVisible(false);
                puff->stopAllActions();
                puff->setScale(1.0);
                puff->setOpacity(255);
                puff->setPosition(Vec2(0,0));
            }
        }

    }
    
}

void Block::hidePuffs() {
    setPuffing(false);
}


void Block::setupBlock (int width, int height, int type) {
    
    this->setPuffing(false);
    
    _type = type;
    
    _width = width * _tileWidth;
    _height = height * _tileHeight + _tileHeight * 0.49f;
    this->setPositionY(_height);
    
    SpriteFrame * wallFrame;
    SpriteFrame * roofFrame = rand() % 10 > 6 ? _roof1 : _roof2;
    
    
    int num_chimneys;
    float chimneyX[] = {0,0,0,0,0};
    
    switch (type) {
        
        case kBlockGap:
            this->setVisible(false);
            return;
            
        case kBlock1:
            wallFrame = _tile1;
            chimneyX[0] = 0.2f;
            chimneyX[1] = 0.8f;
            num_chimneys = 2;
            break;
        case kBlock2:
            wallFrame = _tile2;
            chimneyX[0] = 0.2f;
            chimneyX[1] = 0.8f;
            chimneyX[2] = 0.5f;
            num_chimneys = 3;
            break;
        case kBlock3:
            wallFrame = _tile3;
            chimneyX[0] = 0.2f;
            chimneyX[1] = 0.8f;
            chimneyX[2] = 0.5f;
            num_chimneys = 3;
            
            break;
        case kBlock4:
            wallFrame = _tile4;
            chimneyX[0] = 0.2f;
            chimneyX[1] = 0.5f;
            num_chimneys = 2;
            break;
    }
    
	
    for ( int i = 0; i < _chimneys.size(); i++) {
    	auto chimney = _chimneys.at(i);
        if (i < num_chimneys) {
            chimney->setPosition( Vec2 (chimneyX[i] * _width, 0) );
            chimney->setVisible(true);
            
        } else {
            chimney->setVisible(false);
        }
    }
    
    this->setVisible(true);
    
    for (auto tile : _roofTiles) {
        if (tile->getPositionX() < _width) {
            tile->setVisible(true);
            tile->setDisplayFrame(roofFrame);
        } else {
            tile->setVisible(false);
        }
    }
    
    for (auto tile : _wallTiles) {
        if (tile->getPositionX() < _width && tile->getPositionY() > -_height) {
            tile->setVisible(true);
            tile->setDisplayFrame(wallFrame);
        } else {
            tile->setVisible(false);
        }
    }
}

void Block::initBlock() {

    
    _tile1 = SpriteFrameCache::getInstance()->getSpriteFrameByName("building_1.png");
    _tile2 = SpriteFrameCache::getInstance()->getSpriteFrameByName ("building_2.png");
    _tile3 = SpriteFrameCache::getInstance()->getSpriteFrameByName ("building_3.png");
    _tile4 = SpriteFrameCache::getInstance()->getSpriteFrameByName ("building_4.png");
    
    _roof1 = SpriteFrameCache::getInstance()->getSpriteFrameByName ("roof_1.png");
    _roof2 = SpriteFrameCache::getInstance()->getSpriteFrameByName ("roof_2.png");
    

    //create tiles
    for (int i = 0; i < 5; i++) {
        auto tile = Sprite::createWithSpriteFrameName("roof_1.png");
        tile->setAnchorPoint(Vec2(0, 1));
        tile->setPosition(Vec2(i * _tileWidth, 0));
        tile->setVisible(false);
        this->addChild(tile, kMiddleground, kRoofTile);
        _roofTiles.pushBack(tile);
        
        for (int j = 0; j < 4; j++) {
            tile = Sprite::createWithSpriteFrameName("building_1.png");
            tile->setAnchorPoint(Vec2(0, 1));
            tile->setPosition(Vec2(i * _tileWidth, -1 * (_tileHeight * 0.47f + j * _tileHeight)));
            tile->setVisible(false);
            this->addChild(tile, kBackground, kWallTile);
            _wallTiles.pushBack(tile);
        }
        
    }
       
    
    for (int i = 0; i < 5; i++) {
        auto chimney = Sprite::createWithSpriteFrameName("chimney.png");
        chimney->setVisible(false);
        this->addChild(chimney, kForeground, kChimney);
        _chimneys.pushBack(chimney);
        
        for (int j = 0; j < TOTAL_PUFFS; j++) {
            auto puff = Sprite::createWithSpriteFrameName("puff_1.png");
            puff->setAnchorPoint(Vec2(0,-0.5));
            puff->setVisible(false);
            chimney->addChild(puff, -1, j);
        }
        
    }
    
    Animation* animation;
    animation = Animation::create();
    SpriteFrame * frame;

    for(int i = 1; i <= 4; i++) {
        frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(String::createWithFormat("puff_%i.png", i)->getCString());
        animation->addSpriteFrame(frame);
    }

    animation->setDelayPerUnit(0.75f / 4.0f);
    animation->setRestoreOriginalFrame(false);
    animation->setLoops(-1);
    _puffAnimation = Animate::create(animation);
    _puffAnimation->retain();

    _puffSpawn = Repeat::create(Sequence::create(DelayTime::create(0.5f),
                                                 CallFunc::create(std::bind(&Block::createPuff, this)),
                                  nullptr), TOTAL_PUFFS);
    _puffSpawn->retain();

    _puffMove = MoveBy::create(1.0f, Vec2(-100,80));
    _puffMove->retain();
    _puffFade = FadeOut::create(2.0f);
    _puffFade->retain();
    _puffScale = ScaleBy::create(1.5f, 1.5);
    _puffScale->retain();
    
    
    _puffIndex = 0;
}


void Block::createPuff () {
    
    
    for ( auto chimney : _chimneys) {
        if (chimney->isVisible()) {
            
            auto puff = chimney->getChildByTag(_puffIndex);
            puff->setVisible(true);
            puff->stopAllActions();
            puff->setScale(1.0);
            puff->setOpacity(255);
            puff->setPosition(Vec2(0,0));
            puff->runAction( _puffAnimation->clone());
            puff->runAction( _puffMove->clone());
            puff->runAction( _puffScale->clone());
            
        }
    }
    
    _puffIndex++;
    if (_puffIndex == TOTAL_PUFFS) _puffIndex = 0;
}
