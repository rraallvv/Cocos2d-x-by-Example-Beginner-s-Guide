//
//  GameConfig.h
//  Eskimo
//
//  Created by Roger Engelbert on 1/19/13.
//
//

#ifndef Eskimo_GameConfig_h
#define Eskimo_GameConfig_h

enum {
    kBackground,
    kMiddleground,
    kForeground
};

enum {
    kGameIntro,
    kGamePlay,
    kGameOver
};

enum {
    kDirectionUp,
    kDirectionDown,
    kDirectionRight,
    kDirectionLeft
};

enum {
    kStateBox,
    kStateCircle
};

enum {
    kSpriteBlock,
    kSpritePlayer,
    kSpriteSwitch,
    kSpritePlatform,
    kSpriteIgloo,
    kSpriteBtnOn,
    kSpriteBtnOff,
    kSpriteBtnPlay,
    kSpriteBtnPause,
    kSpriteBtnReset,
    kSpriteBtnStart,
    kSpriteBtnMenu,
    kSpriteBtnAgain
};

#define EVENT_DISPATCHER Director::getInstance()->getEventDispatcher()
#define ADD_NOTIFICATION( __target__, __notification__, __handler__) EVENT_DISPATCHER->addEventListenerWithSceneGraphPriority(EventListenerCustom::create (__notification__, __handler__), __target__);


#define TUTORIAL_1 "tilt the device to move the eskimo."
#define TUTORIAL_2 "tap the screen to switch from snowball to ice cube."
#define TUTORIAL_3 "hit the gravity switches to help the eskimo reach the igloo."

#define ACCELEROMETER_MULTIPLIER 4.0
#define CIRCLE_MULTIPLIER 2.0
#define BOX_MULTIPLIER 4.0
#define PTM_RATIO 32.0
#define TILE 32
#define BALL_FRICTION 0.95
#define SWITCH_RADIUS 10.0
#define PLAYER_RADIUS 20.0
#define IGLOO_SQ_RADIUS TILE * TILE
#define FORCE_GRAVITY 9.0
#define PLATFORM_HEIGHT 16.0
#define PLAYER_SWITCH_RADII pow(SWITCH_RADIUS * 0.8, 2) + pow(PLAYER_RADIUS * 0.8, 2)
#define TILES_PER_PLATFORM 15
#endif
