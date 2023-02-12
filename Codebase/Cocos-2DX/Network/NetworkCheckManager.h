//
//  NetworkCheckManager.h
//  Assassin
//
//  Created by Yiğithan Aydın on 6.10.2022.
//

#ifndef NetworkCheckManager_h
#define NetworkCheckManager_h
#include "GameManager.h"
#include "Gameplay/Game.h"
#include "Core.h"
#include "UserSettings.h"
#include "HBUserDefaults.h"
#include "Objects/MenuNode.h"


class NetworkCheckManager
{
    public:
    
        double startTime;
    
        static NetworkCheckManager* getInstance();
    
        bool checkShowNetworkPopUp();
        void increaseElapsedGameplayTime(int second);
        void checkResetTimer();
        int getNonNetworkedElapsedTime();
        double getTimePoint();
        double getCurrentTime();
};


#endif /* NetworkCheckManager_h */
