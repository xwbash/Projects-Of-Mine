#include "CreativeStudioManager.h"
#include "UserSettings.h"
#include "GameManager.h"
#include "HBUserDefaults.h"

void CreativeStudioManager::init()
{
    
    
}

void CreativeStudioManager::setFeatureValue(const std::string& feature, int value)
{
    
#if defined(CREATIVE_SUIT_MODE_ENABLED)
    HBUserDefaults::getInstance()->setIntegerForKey(StringUtils::format("creative_%s",feature.c_str()).c_str(), value);
#endif
    
}

int CreativeStudioManager::getFeatureValue(const std::string& feature)
{
#if defined(CREATIVE_SUIT_MODE_ENABLED)
    return HBUserDefaults::getInstance()->getIntegerForKey(StringUtils::format("creative_%s",feature.c_str()).c_str(), 0);
#endif
    
    return 0;
}


//

static CreativeStudioManager *sharedCreativeStudioManager = NULL;

CreativeStudioManager* CreativeStudioManager::getInstance()
{
    if (!sharedCreativeStudioManager)
    {
        sharedCreativeStudioManager = new CreativeStudioManager();
        sharedCreativeStudioManager->init();
    }
    return sharedCreativeStudioManager;
}
