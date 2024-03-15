#ifndef DailyLoginPopup_h
#define DailyLoginPopup_h

#include "../Core.h"
#include "../UI/ScrollNode.h"
#include "../UI/ButtonImage.h"
#include "../UI/NotificationBadge.h"
#include "../UI/RewardNode.h"
#include "../Menu/PopupNode.hpp"
#include "../UI/AchievementLoadingBar/AchievementLoadingBarNode.hpp"
#include "../UI/Grid.h"

USING_NS_CC;

class DailyLoginButton  : public cocos2d::Node
{
public:
    
    void setup(float width);
    void hide();
    void show();
    void updateButton();
    void updateBadge();
    
    NotificationBadge* badge;
    
    inline ButtonImage* getButton() const { return button; }
    
    CREATE_FUNC(DailyLoginButton);
    
private:
    
    ButtonImage* button;

};

//

class DailyLoginDayNode : public cocos2d::Node
{
public:
    
    void setUp(float width, int dayIndex, bool isAutoShow = false, bool isWideBackground = false, std::function<void()> refreshCallback = nullptr, Node* timerNode = nullptr);
    
    void updateContent();
    
    ButtonImage* button = nullptr;
    
    CREATE_FUNC(DailyLoginDayNode);
    
private:

    Grid<RewardNode> grid;
    
    Node* labelTitleContainer = nullptr;
    
    HBLabel* labelTitle = nullptr;
    
    Sprite* tick = nullptr;
    
    std::function<void()> refreshCallback = nullptr;
    Node* timerNode = nullptr;
    float width;
    bool isWide = false;
    int dayIndex;
    bool isActive = true;
    
    bool isAutoShow;
};

class DailyLoginPopup : public PopupNode
{
public:
    
    DailyLoginPopup* setUp(bool isAutoShow);
    
    void updateContent(bool isAutoShow);
    
    void updateDayNodes();
    
    int priorityValue;
    
    ButtonImage* closeButton;
    
    void onExit() override;
    
    CREATE_POPUP(DailyLoginPopup);
    
private:
    std::vector<AchievementData> getAchievementData();
    std::vector<DailyLoginDayNode*> dayNodes;
    
    int minValue;
    
    float width;
    
    float fontSize;
    
    int clickPriority;
    
    std::function<void()> updateFunc = nullptr;
    
    AchievementLoadingBarNode* achievementBar = nullptr;
};

#endif

