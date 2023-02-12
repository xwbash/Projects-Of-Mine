#ifndef KeyboardDebugController_h
#define KeyboardDebugController_h

#include "cocos2d.h"
#include "../Core.h"
#include "../HBLabel.h"

USING_NS_CC;

class KeyboardDebugController
{
public:
    void initialize(Node* node);
    void update(const float dt);

private:
    int arrowMem[4];
    Vec2 moveVector;
    
    const float amplitude = 1.f;
    
    int debugWeaponIndex = 0;
    HBLabel* debugWeaponTag = nullptr;
    Sprite* debugWeaponCircle = nullptr;
    
    cocos2d::EventKeyboard::KeyCode keyCode;
    cocos2d::Event* event = nullptr;
    bool isKeyPressed = false;
    
    // don't call from outside
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    
    void handleMoveEvents(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event, int press);
};

#endif /* KeyboardDebugController_h */
