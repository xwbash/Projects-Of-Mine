//
//  GrowthFundPopup.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 5.03.2024.
//

#ifndef GrowthFundPopup_hpp
#define GrowthFundPopup_hpp

#include "../../Core.h"
#include "../../Menu/PopupLayer.h"
#include "../../GrowthFundManager.hpp"
#include "../../UI/RewardNode.h"
#include "../../UI/EndlessScroll/BaseVerticalProgressBarNode.h"


class BubbleNode : public Node
{
public:
    BubbleNode* setup(float width, int boughted, GrowthFundType growthFundType, std::function<void()> updateContent);
    void updateBubble();
    CREATE_FUNC(BubbleNode);
    
    Size bubbleSize;
    
private:
    float width;
    int boughted = 0;
    GrowthFundType growthFundType;
    ButtonImage* backgroundImage = nullptr;
    HBLabel* buyLabel = nullptr;
    HBLabel* iapLabel = nullptr;
};

class GrowthFundRewardNode : public Node
{
public:
    GrowthFundRewardNode* setup(int level, float width, GrowthFundType growthFundType, std::function<void()> updateContent);
    void updateContent();
    RewardState getRewardState(int level, GrowthFundType growthFundType);
    CREATE_FUNC(GrowthFundRewardNode);

private:
    RewardNode* rewardNode = nullptr;
    Sprite* lockedSprite = nullptr;
    int level = 0;
    int type = 0; // 0 - locked, 1 - claimable, 2- claimed.
    GrowthFundType growthFundType;
    BaseProgressBarNode* baseProgressBarNode = nullptr;
};

class ProgressLevelBar : public Node
{
public:
    ProgressLevelBar* setup(int level, int maxLevel, float width, float height);
    void updateContent(int level, int maxLevel);
    int getLevel(){ return level; }
    CREATE_FUNC(ProgressLevelBar);
    
private:
    int level = 0;
    Sprite* divider = nullptr;
    std::vector<Sprite*> progressSprites;
    BaseVerticalProgressBarNode* baseProgressBarNode = nullptr;
};

class GrowthFundPopup : public PopupNode
{
public:
    GrowthFundPopup* setUp();
    
    void onExit() override;
    
    CREATE_POPUP(GrowthFundPopup);
    
private:
    std::vector<BubbleNode*> bubbleNodes;
    std::vector<GrowthFundRewardNode*> growthFundNodes;
    ProgressLevelBar* progressBar = nullptr;
};

#endif /* GrowthFundPopup_hpp */
