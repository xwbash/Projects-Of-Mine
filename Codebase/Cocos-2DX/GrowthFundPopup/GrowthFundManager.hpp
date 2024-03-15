//
//  GrowthFundManager.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 4.03.2024.
//

#ifndef GrowthFundManager_hpp
#define GrowthFundManager_hpp

#include "Utilities/SingletonBase.h"
#include "IStorageHandler.h"
#include <json/document.h>
#include "Core.h"

#define FREE_REWARD_KEY "freeReward"
#define PREMIUM_REWARD_KEY "premiumReward"
#define EXCLUSIVE_REWARD_KEY "exclusiveReward"

enum GrowthFundType
{
    GROWTH_FREE_ROW,
    GROWTH_PREMIUM_ROW,
    GROWTH_EXCLUSIVE_ROW
};

struct GrowthNodeData
{
    int unlockLevel = 0;
    std::vector<GrowthFundType> growthType;
    std::vector<RewardData> rewardDatas;
};

struct GrowthFundConfig
{
    std::vector<GrowthNodeData> growthNodeDatas; // level - type free, exclusive, premium
    std::string premiumProductIAP;
    std::string exclusiveProductIAP;
};

class GrowthFundManager :  public SingletonBase<GrowthFundManager>, public IStorageHandler
{
    
public:
    GrowthFundConfig getGrowthData();
    RewardData getReward(int level, GrowthFundType growthFundType);
    bool isRewardClaimed(int level, GrowthFundType growthFundType);
    void setRewardClaimed(int level, GrowthFundType growthFundType);
    int getRewardCount();
    int getUnlockLevel(int level, GrowthFundType growthFundType);
    int getLevel();
    void setRowPurchased(GrowthFundType growthFundType, int isPurchased = 0);
    void handleClaimedData(std::string claimedData);
    std::string getClaimedData();
    int isRowPurchased(GrowthFundType growthFundType);
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    
private:
    //MARK Singleton
    friend class SingletonBase<GrowthFundManager>;
    GrowthFundManager();
    
    
private:
    std::string defaultJson;
    bool isInitialized = false;
    GrowthFundConfig growthFundConfig;
    int level = 1;
    std::vector<std::string> vectorData;
    
    bool manageTheData(rapidjson::Document& document);
    bool readData(GrowthFundConfig &updatedData, rapidjson::Document &document);
    
    
};

#endif /* GrowthFundManager_hpp */
