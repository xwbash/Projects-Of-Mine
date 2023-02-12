//
//  ResolutionSimulatorManager.h
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#ifndef ResolutionSimulatorManager_h
#define ResolutionSimulatorManager_h
#include "../Core.h"
#include "../HBUserDefaults.h"

class ResolutionSimulatorManager
{
    
public:
    static ResolutionSimulatorManager* getInstance();
    cocos2d::Size getSavedSimulatorResolution();
    void setSimulatorResolution(cocos2d::Size sizeResolution);
    void setIphoneFrameActive(bool isIphoneX);
    void setDynamicIslandActive(bool dynamicIsland);
    bool isIphoneX();
    bool isDynamicIsland();
    
};


#endif /* ResolutionSimulatorManager_h */
