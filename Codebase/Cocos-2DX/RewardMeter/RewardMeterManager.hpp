//
//  RewardMeterManager.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 31.08.2023.
//

#ifndef RewardMeterManager_hpp
#define RewardMeterManager_hpp

#include "Core.h"
#include "BeaconManager.h"
#include <json/document.h>
#include "RemoteRewardDataManager.h"
#include "IStorageHandler.h"

struct RewardMeterItem
{
    RewardData rewardData;
    int adAmount;
};

struct RewardMeterItemConfig
{
    std::vector<RewardMeterItem> rewardItemData;
    
    bool fromJSON(rapidjson::Document& document)
    {
        if(document.IsNull()) return false;
        if(!document.HasMember("rewardMeterData")) return false;
        
        if (!document["rewardMeterData"].IsArray()) return false;

        rewardItemData.clear();
        
        const rapidjson::Value& dataArray = document["rewardMeterData"];

        for (rapidjson::SizeType i = 0; i < dataArray.Size(); i++) {
            
            const rapidjson::Value& item = dataArray[i];
            if (item.HasMember("ad_amount") && item.HasMember("reward")) {
                
                auto rewardArray = item["reward"].GetArray();
                auto jsonData = RemoteRewardDataManager::getInstance()->deserializeRewardJson(rewardArray[0]);
                int adAmount = atoi(item["ad_amount"].GetString());
                int id = item["ID"].GetInt();

                RewardMeterItem rewardMeterItem;
                rewardMeterItem.adAmount = adAmount;
                
                auto reward = RemoteRewardDataManager::getInstance()->getRewardDatas(&jsonData[0]);
                
                if(reward.size() == 0) return false;
                
                if(reward[0].isCountValueSignificant())
                {
                    reward[0].setValue(id);
                }
                
                rewardMeterItem.rewardData = reward[0];
                
                rewardItemData.push_back(rewardMeterItem);
            }
        }
        return true;
    }
};

class RewardMeterManager : IStorageHandler
{
private:
    void init();

public:
    static RewardMeterManager* getInstance(bool initialize = true);
    ~RewardMeterManager();
    

    //JSON
    std::string jsonConfig;
    RewardMeterItemConfig rewardMeterItemConfig;
    
    
    bool isAvailable();
    //
    double getRemainingTime();
    void resetTimer();
    
    //
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    //
    
    int getRewardMeterCount();
  
    void increaseRewardMeterCount();
    void resetTheRewardMeterCount();

};

#endif /* RewardMeterManager_hpp */
