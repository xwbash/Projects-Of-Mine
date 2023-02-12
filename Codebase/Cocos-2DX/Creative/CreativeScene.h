#ifndef CreativeScene_h
#define CreativeScene_h

#include "Core.h"
#include "UI/ButtonImage.h"
#include "HBLabel.h"

enum CreativeCategory
{
    PLAYER_CATEGORY,
    GUARD_CONFIG_CATEGORY,
    UI_CATEGORY,
    GAMEPLAY_CATEGORY,
    LEVEL_CATEGORY
};

class CreativeScene : public cocos2d::Scene
{
public:
    
    static cocos2d::Scene* createScene();
    
    virtual bool init() override;
    
    CREATE_FUNC(CreativeScene);
    
private:
    
    void updateButton(int index);
    
    struct CreativeFeature {
        
        std::string name;
        int numberOfValues;
        CreativeCategory category;
    };
    
    std::vector<CreativeFeature> features;
    std::vector<ButtonImage*> buttons;
    std::vector<HBLabel*> buttonLabels;
    
    
};

#endif

