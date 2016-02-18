#ifndef __LINECONTAINER_H__
#define __LINECONTAINER_H__


#include "cocos2d.h"

using namespace cocos2d;

class LineContainer : public DrawNode {
    
public:
    
    CC_SYNTHESIZE(Point, _ballPoint, BallPoint);
	CC_SYNTHESIZE(Point, _cuePoint, CuePoint);
    CC_SYNTHESIZE(bool, _drawing, Drawing);
	
    LineContainer();
	static LineContainer * create();
    virtual bool init ();
    void drawDottedLine();
    void update (float dt);

private:
    
    int _dash;
    int _dashSpace;
	Size _screenSize;
    
};

#endif // __LINECONTAINER_H__


