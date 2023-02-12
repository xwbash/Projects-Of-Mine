//
//  VersionCheckManager.cpp
//  Assassin
//
//  Created by Yiğithan Aydın on 11.10.2022.
//

#include "VersionCheckManager.h"
#include "Objects/MenuNode.h"
#include "HBUserDefaults.h"

#define SOFT_VERSION_KEY "softForceVersion"
#define HARD_VERSION_KEY "hardForceVersion"

bool VersionCheckManager::checkVersion()
{
    
    const int softVersion = getRemoteConfigInteger(SOFT_VERSION_KEY); // Get instance soft version on data
    const int hardVersion = getRemoteConfigInteger(HARD_VERSION_KEY); // Get instance hard version on data
    const int appVersion = std::stoi(APP_VERSION);
    
    if(appVersion < softVersion)
    {
        if(appVersion < hardVersion)
        {
            MenuNode::current()->showVersionPopUp(HARD);
            return true;
        }
        
        MenuNode::current()->showVersionPopUp(SOFTY);
        return true;
    }
    
    return false;
}

static VersionCheckManager *instance = NULL;
    
VersionCheckManager* VersionCheckManager::getInstance()
{
    if (! instance)
    {
        instance = new VersionCheckManager();
    }
        
    return instance;
}
