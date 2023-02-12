//
//  ResolutionSimulatorManager.cpp
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#include "ResolutionSimulatorManager.h"

#define IS_IPHONE_X_KEY "Is_Iphone_X"
#define IS_DYNAMIC_KEY_KEY "Is_Iphone_Dynamic_Island"
#define RESOLUTION_KEY_X "RESOLUTION_KEY_X"
#define RESOLUTION_KEY_Y "RESOLUTION_KEY_Y"

static ResolutionSimulatorManager * _instance = NULL;

ResolutionSimulatorManager* ResolutionSimulatorManager::getInstance()
{
    if (! _instance)
    {
        _instance = new ResolutionSimulatorManager();
    }
    
    return _instance;
}

void ResolutionSimulatorManager::setSimulatorResolution(cocos2d::Size sizeResolution)
{
    HBUserDefaults::getInstance()->setIntegerForKey(RESOLUTION_KEY_X, sizeResolution.width);
    HBUserDefaults::getInstance()->setIntegerForKey(RESOLUTION_KEY_Y, sizeResolution.height);
}

void ResolutionSimulatorManager::setIphoneFrameActive(bool isIphoneX)
{
    HBUserDefaults::getInstance()->setBoolForKey(IS_IPHONE_X_KEY, isIphoneX);
}

void ResolutionSimulatorManager::setDynamicIslandActive(bool isDynamicIsland)
{
    HBUserDefaults::getInstance()->setBoolForKey(IS_DYNAMIC_KEY_KEY, isDynamicIsland);
}

cocos2d::Size ResolutionSimulatorManager::getSavedSimulatorResolution()
{
    cocos2d::Size simulatorSize;
    cocos2d::Size defaultSimulatorSize = cocos2d::Size(946,2048);
    
    simulatorSize.width = HBUserDefaults::getInstance()->getIntegerForKey(RESOLUTION_KEY_X, defaultSimulatorSize.width);
    simulatorSize.height = HBUserDefaults::getInstance()->getIntegerForKey(RESOLUTION_KEY_Y, defaultSimulatorSize.height);
    
    return simulatorSize;
}

bool ResolutionSimulatorManager::isIphoneX()
{
    return HBUserDefaults::getInstance()->getBoolForKey(IS_IPHONE_X_KEY, true);
}

bool ResolutionSimulatorManager::isDynamicIsland()
{
    return HBUserDefaults::getInstance()->getBoolForKey(IS_DYNAMIC_KEY_KEY, true);
}
