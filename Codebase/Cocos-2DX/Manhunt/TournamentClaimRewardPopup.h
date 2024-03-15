#ifndef TournamentClaimRewardPopup_h
#define TournamentClaimRewardPopup_h

#include "../../Core.h"
#include "../../Menu/PopupNode.hpp"
#include "../../Menu/MenuNode.h"

USING_NS_CC;

class TournamentClaimRewardPopup : public PopupNode
{
public:
    
    TournamentClaimRewardPopup* setup(int placement, int score, std::vector<RewardData> rewards);
    
    void onExit() override;
    
    CREATE_POPUP(TournamentClaimRewardPopup);
    
private:
    
    void playAnimation();
    
    std::string getPlacementString(int placement);
    
    std::string getIconPath(int placement, bool isWinner);
    
    std::vector<RewardData> rewardsData;
    
    std::vector<RewardNode*> rewardNodes;
    
    UIButton* collectButton = nullptr;
    
};

#endif /* TournamentClaimRewardPopup_h */
