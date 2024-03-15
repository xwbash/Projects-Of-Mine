#ifndef IdleEarningPopup_h
#define IdleEarningPopup_h

#include "../Core.h"
#include "../Menu/PopupNode.hpp"
#include "../UI/UIButton.h"
#include "../UI/NotificationBadge.h"
#include "../UI/Grid.h"
#include "../UI/RewardNode.h"

USING_NS_CC;

class QuickEarningPopup : public PopupNode
{
public:
    
    QuickEarningPopup* setUp();
    
    int activeClaimButtonCount();
    
    CREATE_POPUP(QuickEarningPopup);
    
private:
    
    void updateClaimButtons();
    
    UIButton* activeClaimWithAdButton;
    UIButton* inactiveClaimWithAdButton;
    
    UIButton* activeClaimWithEnergyButton;
    UIButton* inactiveClaimWithEnergyButton;
};

class IdleEarningButton  : public cocos2d::Node
{
public:
    
    void setup(float width);
    
    void hide();
    void show();
   
    void updateBadge();
    
    NotificationBadge* badge;
    
    inline ButtonImage* getButton() const { return button; }
    
    CREATE_FUNC(IdleEarningButton);
    
private:
    
    ButtonImage* button;

};

class IdleEarningPopup : public PopupNode
{
public:
    
    IdleEarningPopup* setUp();
    
    void updateBadge();
    
    CREATE_POPUP(IdleEarningPopup);
    
private:
    
    void updateContent();
    void updateClaimButtons();
    
    // VARIABLES
    
    std::vector<RewardData> rewards;
        
    QuickEarningPopup* quickEarningPopup;
    
    // CLAIM BUTTONS
    
    UIButton* activeClaimButton = nullptr;
    UIButton* inactiveClaimButton = nullptr;
    
    // GRID DATAS
    
    Node* rewardGridNode;
    
    GridData rewardsGridData;
    Grid<RewardNode> rewardsGrid;
    
    float itemWidth = 0.f;
    
};

#endif
