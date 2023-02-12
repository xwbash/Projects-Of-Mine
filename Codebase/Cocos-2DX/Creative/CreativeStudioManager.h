#ifndef CreativeStudioManager_h
#define CreativeStudioManager_h

#include "Core.h"

#define GET_CREATIVE_FATURE_VALUE(__VALUE__) CreativeStudioManager::getInstance()->getFeatureValue(__VALUE__)

class CreativeStudioManager
{
    
public:
    
    bool GENERATING_MISSION = false;
    bool GENERATING_BOSS = false;
    
    void setFeatureValue(const std::string& feature, int value);
    int getFeatureValue(const std::string& feature);
    
    static CreativeStudioManager* getInstance();
    
private:
   
    void init();
};

#endif
