//
//  GSwitch.h
//  MiniPool
//
//  Created by Roger Engelbert on 1/13/13.
//
//

#ifndef ___GSwitch__
#define ___GSwitch__

#include "GameLayer.h"

using namespace cocos2d;

class GSwitch : public Sprite {
public:
    
    CC_SYNTHESIZE(int, _direction, Direction);
    
    virtual ~GSwitch();
    GSwitch();
    
    static GSwitch* create();
    void initGSwitch(int direction, Point position);
    
    
};
 

#endif /* defined(___GSwitch__) */
