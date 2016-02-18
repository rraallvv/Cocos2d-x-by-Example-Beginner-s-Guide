//
//  LevelSelectLayer.h
//  Eskimo
//
//  Created by Roger Engelbert on 1/22/13.
//
//

#ifndef __Eskimo__LevelSelectLayer__
#define __Eskimo__LevelSelectLayer__

#include "cocos2d.h"
USING_NS_CC;


enum {
    kTagButtonOn,
    kTagButtonOff
};

class LevelSelectLayer : public Layer {
public:
    
    virtual ~LevelSelectLayer();
    LevelSelectLayer();
    
    static cocos2d::Scene* scene();
    
    virtual bool onTouchBegan(Touch * touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    
private:
    SpriteBatchNode * _layerBatchNode;
    Size _screenSize;
    Menu * _levelMenu;
    Vector<Sprite *> _levels;
    Vector<Label *>  _levelLabels;
    
    Sprite * _btnBack;
    Sprite * _btnNext;
    
    
    ParticleSystem * _snow;
    
    int _levelsCompleted;
    int _firstIndex;
    
    void createScreen(void);
    void createMenu(void);
    
};

#endif /* defined(__Eskimo__LevelSelectLayer__) */
