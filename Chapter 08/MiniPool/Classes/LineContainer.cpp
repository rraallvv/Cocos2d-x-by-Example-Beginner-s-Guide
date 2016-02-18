#include "LineContainer.h"

using namespace cocos2d;

LineContainer::LineContainer() {

	_dash = 5;
	_dashSpace = 5;
	_screenSize = Director::getInstance()->getWinSize();
    _drawing = false;
}

LineContainer* LineContainer::create () {
    LineContainer *obj = new LineContainer();
    if ( obj && obj->init() )
    {
        obj->autorelease();
        return obj;
    }
    
    CC_SAFE_DELETE( obj );
    return obj = nullptr;
}

bool LineContainer::init (){
    if (!DrawNode::init())
    {
        return false;
    }
    
    this->scheduleUpdate();
    return true;
}

void LineContainer::update (float dt){
    clear();
    if (_drawing) {
         drawDottedLine();
     }
}

void LineContainer::drawDottedLine () {
	
    float length = _ballPoint.distance ( _cuePoint );
    int segments = length / (_dash + _dashSpace);
    
    float t = 0.0f;
    float x_;
    float y_;
    
    for (int i = 0; i < segments + 1; i++) {
        
        x_ = _cuePoint.x + t * (_ballPoint.x - _cuePoint.x);
        y_ = _cuePoint.y + t * (_ballPoint.y - _cuePoint.y);
        
        drawDot(Vec2 ( x_, y_ ), 3, Color4F(1.0, 1.0, 1.0, 1.0));
        
        t += (float) 1 / segments;
    }
    
}

