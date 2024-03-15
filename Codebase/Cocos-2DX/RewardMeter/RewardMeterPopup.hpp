//
//  RewardMeterPopup.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 31.08.2023.
//

#ifndef RewardMeterPopup_hpp
#define RewardMeterPopup_hpp

#include "../Core.h"
#include "../UI/BaseProgressBarNode.h"
#include "../UI/HBLabel.h"
#include "../UI/RewardNode.h"
#include "../RewardMeterManager.hpp"
#include "../Menu/PopupNode.hpp"

class RewardMeterButton : public cocos2d::Node
{
public:
    RewardMeterButton* setup(float width);
    void show();
    void hide();
    CREATE_FUNC(RewardMeterButton);
    
    ButtonImage* buttonImage = nullptr;
};

struct RewardMeterRewardData {
    RewardNode* rewardNode;
    RewardMeterItem rewardMeterItem;
    HBLabel* labelUnder;
};

class RewardMeterLoadingBar : public cocos2d::Node
{
public:
    RewardMeterLoadingBar* setup(float width, float height);
    void updateBar();
    void clearRewardClaims(int index);
    Size nodeSize;

    CREATE_FUNC(RewardMeterLoadingBar);
private:
    
    
    void addTick(RewardNode* button, int index, Vec2 targetPosition);
    
    std::vector<bool> tickedButtons;
    std::vector<RewardMeterRewardData> rewardMeterRewardDatas;
    int currentRewardAmount = 0;
    BaseProgressBarNode* progressBar = nullptr;
    HBLabel* rewardAdMeterLabel = nullptr;
};

class RewardMeterPopup : public PopupNode
{
public:
    RewardMeterPopup* setup();
    
    void onExit() override;
    
    CREATE_POPUP(RewardMeterPopup);
private:
    RewardMeterLoadingBar* loadingBar = nullptr;
    Action* action = nullptr;
};



#endif /* RewardMeterPopup_hpp */
