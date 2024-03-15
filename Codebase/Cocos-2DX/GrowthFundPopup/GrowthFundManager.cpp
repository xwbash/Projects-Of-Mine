//
//  GrowthFundManager.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 4.03.2024.
//

/// MARK: ROW NAMING
#define FREE_ROW_NAME "FREE"
#define PREMIUM_ROW_NAME "PREMIUM"
#define EXCLUSIVE_ROW_NAME "EXCLUSIVE"

#define IS_ROW_PURCHASED "IS_ROW_PURCHASED_%i"
#define IS_REWARD_CLAIMED "IS_REWARD_CLAIMED_ROW_%i_LEVEL_%i"
#define GET_LEVEL_KEY "GET_LEVEL_KEY_GROWTH"

#include "GrowthFundManager.hpp"
#include "RemoteRewardDataManager.h"
#include "PlayerLevelManager.h"
#include "HBUserDefaults.h"
#include "BeaconManager.h"
#include "StorageManager.h"


GrowthFundManager::GrowthFundManager()
{
    if(isInitialized) return;
    
    isInitialized = true;

    defaultJson = R"(
      {
        "growthFundSettings": {
          "levelSettings": [
            {
              "unlockLevel": 1,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 3,
              "freeReward": {
                "gem": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 5,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 10,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 15,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 20,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 25,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            },
            {
              "unlockLevel": 30,
              "freeReward": {
                "gold": 25
              },
              "premiumReward": {
                "gem": 100
              },
              "exclusiveReward": {
                "gem": 500
              }
            }
          ],
          "premium_product": "growthfund_premium_0999",
          "exclusive_product": "growthfund_exc_1999"
        }
      }
      )";
    
    
    if(!manageTheData(BeaconManager::getInstance()->getRemoteConfigJSON()))
    {
        rapidjson::Document document;
        manageTheData(document.Parse(defaultJson.c_str()));
    }
}

int GrowthFundManager::isRowPurchased(GrowthFundType growthFundType)
{
    if(growthFundType == GROWTH_FREE_ROW)
    {
        return true;
    }
    
    auto isRowPurchased =  StringUtils::format(IS_ROW_PURCHASED, growthFundType);
    return HBUserDefaults::getInstance()->getIntegerForKey(isRowPurchased.c_str(), 0);
}

void GrowthFundManager::setRowPurchased(GrowthFundType growthFundType, int value)
{
    auto isRowPurchased =  StringUtils::format(IS_ROW_PURCHASED, growthFundType);
    HBUserDefaults::getInstance()->setIntegerForKey(isRowPurchased.c_str(), value);
    
    syncToRemote();
}

int GrowthFundManager::getLevel()
{
    return PlayerLevelManager::getInstance()->getPlayerLevel();
}

bool GrowthFundManager::manageTheData(rapidjson::Document& document)
{
    if(document.IsNull()) 
    {
        return false;
    }
    if(!document.HasMember("growthFundSettings")) 
    {
        return false;
    }
    

    const rapidjson::Value& growthFundSettings = document["growthFundSettings"];
    
    const rapidjson::Value& levelSettings = growthFundSettings["levelSettings"];
    if (levelSettings.IsArray()) {
        for (rapidjson::SizeType i = 0; i < levelSettings.Size(); i++) {
            
            GrowthNodeData growthNodeData;
            
            const rapidjson::Value& levelSetting = levelSettings[i];

            int unlockLevel = levelSetting["unlockLevel"].GetInt();
            
            const rapidjson::Value& freeReward = levelSetting[FREE_REWARD_KEY];
            const rapidjson::Value& premiumReward = levelSetting[PREMIUM_REWARD_KEY];
            const rapidjson::Value& exclusiveReward = levelSetting[EXCLUSIVE_REWARD_KEY];
            
            auto jsonRewardFree  = RemoteRewardDataManager::getInstance()->deserializeRewardJson(freeReward);
            auto jsonRewardPremium  = RemoteRewardDataManager::getInstance()->deserializeRewardJson(premiumReward);
            auto jsonRewardExclusive  = RemoteRewardDataManager::getInstance()->deserializeRewardJson(exclusiveReward);
            
            auto rewardFree = RemoteRewardDataManager::getInstance()->getRewardDatas(jsonRewardFree);
            auto rewardPremium = RemoteRewardDataManager::getInstance()->getRewardDatas(jsonRewardPremium);
            auto rewardExclusive = RemoteRewardDataManager::getInstance()->getRewardDatas(jsonRewardExclusive);
            
            
            growthNodeData.growthType.push_back(GROWTH_FREE_ROW);
            growthNodeData.rewardDatas.push_back(rewardFree[0]);
            growthNodeData.growthType.push_back(GROWTH_PREMIUM_ROW);
            growthNodeData.rewardDatas.push_back(rewardPremium[0]);
            growthNodeData.growthType.push_back(GROWTH_EXCLUSIVE_ROW);
            growthNodeData.rewardDatas.push_back(rewardExclusive[0]);
            growthNodeData.unlockLevel = unlockLevel;
            
            growthFundConfig.growthNodeDatas.push_back(growthNodeData);
        }
    }

    growthFundConfig.premiumProductIAP = growthFundSettings["premium_product"].GetString();
    growthFundConfig.exclusiveProductIAP = growthFundSettings["exclusive_product"].GetString();
    
    return true;
}

int GrowthFundManager::getUnlockLevel(int level, GrowthFundType growthFundType)
{
    auto growthNodeDatas = growthFundConfig.growthNodeDatas;
    auto index = level-1;
    
    if(growthNodeDatas.size() < index)
    {
        return 0;
    }

    auto growthData = growthNodeDatas[index];
    
    for (int i = 0; i < growthData.growthType.size(); i++)
    {
        if(growthData.growthType[i] == growthFundType)
        {
            return growthData.unlockLevel;
        }
    }
    
    return 0;
}

RewardData GrowthFundManager::getReward(int unlockLevel, GrowthFundType growthFundType)
{
    auto growthNodeDatas = growthFundConfig.growthNodeDatas;

    
    for(int j = 0; j < growthNodeDatas.size(); j++)
    {
        auto growthData = growthNodeDatas[j];
        for (int i = 0; i < growthData.growthType.size(); i++)
        {
            if(growthData.growthType[i] == growthFundType && growthData.unlockLevel == unlockLevel)
            {
                return growthData.rewardDatas[i];
            }
        }
    }
    
    CCLOG("Not filled data");
    return RewardData(REWARD_GOLD, 1);
}

bool GrowthFundManager::isRewardClaimed(int level, GrowthFundType growthFundType)
{
    std::string id = StringUtils::format(IS_REWARD_CLAIMED, growthFundType, level);
    return HBUserDefaults::getInstance()->getBoolForKey(id.c_str(), false);
}

void GrowthFundManager::setRewardClaimed(int level, GrowthFundType growthFundType)
{
    std::string id = StringUtils::format(IS_REWARD_CLAIMED, growthFundType, level);
    HBUserDefaults::getInstance()->setBoolForKey(id.c_str(), true);
    
    syncToRemote();
}

int GrowthFundManager::getRewardCount()
{
    return growthFundConfig.growthNodeDatas.size();
}

GrowthFundConfig GrowthFundManager::getGrowthData()
{
    return growthFundConfig;
}

void GrowthFundManager::handleClaimedData(std::string claimedData)
{
    std::vector<std::string> rewards;

    size_t pos = 0;
    std::string token;
    while ((pos = claimedData.find(',')) != std::string::npos)
    {
        token = claimedData.substr(0, pos);
        vectorData.push_back(token);
        claimedData.erase(0, pos + 1);
    }

    vectorData.push_back(claimedData);

    for (const auto& reward : vectorData)
    {
        std::string rewardType = reward.substr(0, 2);
        std::string idStr = reward.substr(3);
        int id = std::stoi(idStr);
        
        if(rewardType == FREE_ROW_NAME)
        {
            setRewardClaimed(id, GROWTH_FREE_ROW);
        }
        else if(rewardType == PREMIUM_ROW_NAME)
        {
            setRewardClaimed(id, GROWTH_PREMIUM_ROW);
        }
        else if(rewardType == EXCLUSIVE_ROW_NAME)
        {
            setRewardClaimed(id, GROWTH_EXCLUSIVE_ROW);
        }
    }
}

std::string GrowthFundManager::getClaimedData()
{
    std::string claimedData = "";
    bool isFirst = true;

    for (int i = 1; i <= getRewardCount(); i++)
    {
        auto freeReward = isRewardClaimed(i, GROWTH_FREE_ROW);
        auto premiumReward = isRewardClaimed(i, GROWTH_PREMIUM_ROW);
        auto exclusiveReward = isRewardClaimed(i, GROWTH_EXCLUSIVE_ROW);

        if (freeReward || premiumReward || exclusiveReward)
        {
            if (!isFirst)
            {
                claimedData += ",";
            }
            isFirst = false;

            if (freeReward)
            {
                claimedData += StringUtils::format("%s_%i", FREE_ROW_NAME, i);
            }

            if (premiumReward)
            {
                if (!claimedData.empty())
                {
                    claimedData += ",";
                }
                claimedData += StringUtils::format("%s_%i", PREMIUM_ROW_NAME, i);
            }

            if (exclusiveReward)
            {
                if (!claimedData.empty())
                {
                    claimedData += ",";
                }
                claimedData += StringUtils::format("%s_%i", EXCLUSIVE_ROW_NAME, i);
            }
        }
        
    }
    
    return claimedData;
}


void GrowthFundManager::syncToRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::GROWTH_FUND), StorageManager::mergeValueArray({
        std::to_string((int)isRowPurchased(GROWTH_FREE_ROW)),
        std::to_string((int)isRowPurchased(GROWTH_PREMIUM_ROW)),
        std::to_string((int)isRowPurchased(GROWTH_EXCLUSIVE_ROW)),
        getClaimedData()
    },STORAGE_SUB_DELIMITER));
}

void GrowthFundManager::syncFromRemote()
{
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::GROWTH_FUND), [=](std::string remoteVal){
        
        if (remoteVal.empty()) return;
        
        std::vector<std::string> splitted;
        
        StorageManager::splitValueString(remoteVal, &splitted, STORAGE_SUB_DELIMITER);
        
        if (splitted.size() < 3) return;
        
        setRowPurchased(GROWTH_FREE_ROW, StorageManager::getInt(splitted[0]));
        setRowPurchased(GROWTH_PREMIUM_ROW, StorageManager::getInt(splitted[1]));
        setRowPurchased(GROWTH_EXCLUSIVE_ROW, StorageManager::getInt(splitted[2]));
        
        if(splitted.size() < 4) return;
        
        StorageManager::splitValueString(splitted[3], &vectorData, STORAGE_SUB_DELIMITER);
        
        handleClaimedData(getClaimedData());
        
    });
}

void GrowthFundManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::GROWTH_FUND), STORAGE_EMPTY_VALUE);
}
