
#define TILE_H_SIZE 6
#define TILE_W_SIZE 8

#include "Block.h"

Block::~Block () {

}

Block::Block () {
    
    //get screen size
	_screenSize = Director::getInstance()->getWinSize();
    _tileWidth = _screenSize.width / TILE_W_SIZE;
    _tileHeight = _screenSize.height / TILE_H_SIZE;
    
    this->setVisible(false);
}

Block * Block::create () {
    
    auto block = new Block();
	if (block && block->initWithFile("blank.png")) {
		block->autorelease();
		return block;
	}
	CC_SAFE_DELETE(block);
	return nullptr;
}


void Block::setupBlock (int width, int height, int type) {
    
    _type = type;
    
    _width = width * _tileWidth;
    _height = height * _tileHeight;
    
    this->setAnchorPoint(Vec2(0,0));
    this->setTextureRect(Rect(0, 0, _width, _height));
    
    switch (type) {
        
        case kBlockGap:
            this->setVisible(false);
            return;
            
        case kBlock1:
            
            this->setColor(Color3B(200,200,200));
            
            break;
        case kBlock2:
            
            this->setColor(Color3B(150,150,150));
            
            break;
        case kBlock3:
            
            this->setColor(Color3B(100,100,100));
            
            break;
        case kBlock4:
            
            this->setColor(Color3B(50,50,50));
            
            break;
    }
    
    this->setVisible(true);
   
}

