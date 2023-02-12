//
//  NetworkCheckManager.cpp
//  Assassin
//
//  Created by Yiğithan Aydın on 6.10.2022.
//
#define ELAPSED_TIME_NON_NETWORK_KEY ("TotalGameplayTimeElapsed_")
#define ELAPSED_TIME ("TotalTimeElapsed_")

#include "NetworkCheckManager.h"


int NetworkCheckManager::getNonNetworkedElapsedTime()
{
    return HBUserDefaults::getInstance()->getDoubleForKey(ELAPSED_TIME_NON_NETWORK_KEY, 0);
}

double NetworkCheckManager::getTimePoint()
{
    return HBUserDefaults::getInstance()->getDoubleForKey(ELAPSED_TIME, getCurrentTime());
}

double NetworkCheckManager::getCurrentTime()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void NetworkCheckManager::checkResetTimer()
{
    const auto currentTime = getCurrentTime();

    const int elapsedTime = (currentTime - getTimePoint());
    
    // if elapsed time is 24 hrs
    if(elapsedTime >= 24 * 60 * 60)
    {
        HBUserDefaults::getInstance()->setDoubleForKey(ELAPSED_TIME_NON_NETWORK_KEY, 0);
        HBUserDefaults::getInstance()->setDoubleForKey(ELAPSED_TIME, currentTime);
    }
}

void NetworkCheckManager::increaseElapsedGameplayTime(int second)
{
    // is network close
    if(getConnectionType() == 0)
    {
        auto elapsedGamePlayerTime = second + getNonNetworkedElapsedTime();
        HBUserDefaults::getInstance()->setDoubleForKey(ELAPSED_TIME_NON_NETWORK_KEY, elapsedGamePlayerTime);
    }
}

bool NetworkCheckManager::checkShowNetworkPopUp()
{
    const int minuteLimit = getRemoteConfigInteger("dailyOfflinePlayLimitMinutes");
   
    const auto elapsedMinute = (getNonNetworkedElapsedTime()/60);
    
    checkResetTimer();
    
    const int isSystemActive = getRemoteConfigInteger("dailyOfflinePlayLimitActive");
    
    if(isSystemActive == 1)
    {
        // is network close
        //getConnectionType() == 0
        if(getConnectionType() == 0)
        {
            if(elapsedMinute >= minuteLimit)
            {
                //Network popup cikar
                MenuNode::current()->showNoInternetConnectionScreen();
                return true;
            }
        }
    }
    return false;
}
    
static NetworkCheckManager *instance = NULL;
    
NetworkCheckManager* NetworkCheckManager::getInstance()
{
    if (! instance)
    {
        instance = new NetworkCheckManager();
        instance->checkResetTimer();
    }
        
    return instance;
}
    
