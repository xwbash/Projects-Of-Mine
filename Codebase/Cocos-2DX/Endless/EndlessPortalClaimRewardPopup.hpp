//
//  EndlessPortalClaimRewardPopup.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#ifndef EndlessPortalClaimRewardPopup_hpp
#define EndlessPortalClaimRewardPopup_hpp

#include "../../Core.h"
#include "../../Menu/PopupNode.hpp"
#include "../../Menu/MenuNode.h"

USING_NS_CC;

class EndlessPortalClaimRewardPopup : public PopupNode
{
public:
    
    EndlessPortalClaimRewardPopup* setup(int placement, int score, std::vector<RewardData> rewards);
    
    void onExit() override;
    
    CREATE_POPUP(EndlessPortalClaimRewardPopup);
    
private:
    
    void playAnimation();
    
    FormattedStringBuilder getPlacementString(int placement);
    
    std::string getIconPath(int placement, bool isWinner);
    
    std::vector<RewardData> rewardsData;
    
    std::vector<RewardNode*> rewardNodes;
    
    UIButton* collectButton = nullptr;
    
};
#endif /* EndlessPortalClaimRewardPopup_hpp */
