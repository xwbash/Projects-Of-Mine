#include "KeyboardDebugController.h"
#include "../GameScene.h"
#include "../Gameplay/Scene/GameNode.h"
#include "../ConfigManager.h"


void KeyboardDebugController::initialize(Node* node)
{
    #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    auto keyboardListener = EventListenerKeyboard::create();
    
    keyboardListener->onKeyPressed = CC_CALLBACK_2(KeyboardDebugController::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(KeyboardDebugController::onKeyReleased, this);
    
    arrowMem[0] = 0;
    arrowMem[1] = 0;
    arrowMem[2] = 0;
    arrowMem[3] = 0;
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, node);
    
    #endif
}

void KeyboardDebugController::update(const float dt)
{
    //CCLOG("%s", ("is key pressed " + std::to_string(isKeyPressed)).c_str());
    
    if(Game::current()->started)
    {
        
        auto assassin = Game::current()->assassin;
        if(!assassin->killed)
        {
            auto updatedPosition = assassin->coordinate.solidValue() + (moveVector * amplitude);
            Game::current()->map->updateAssassinPathFastestRoute(assassin, updatedPosition);
        }
    }
}
             
void KeyboardDebugController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            moveVector.x = -1;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            moveVector.x = 1;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            moveVector.y = -1;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            moveVector.y = 1;
            break;
        default:
            break;
    }
}

void KeyboardDebugController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            moveVector.x = 0;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            moveVector.x = 0;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            moveVector.y = 0;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            moveVector.y = 0;
            break;
        default:
            break;
    }
}

void KeyboardDebugController::handleMoveEvents(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event, int press)
{
    

    
}
