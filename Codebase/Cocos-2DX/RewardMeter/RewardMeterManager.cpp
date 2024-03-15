//
//  RewardMeterManager.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 31.08.2023.
//

#define REMAINING_TIME_KEY "Reward_Meter_Remaning_Time"
#define REWARD_METER_SAVED_AMOUNT "REWARD_METER_SAVED_AMOUNT"
#include "RewardMeterManager.hpp"
#include "GameManager.h"
#include "TutorialManager.hpp"
#include "TimeManager.h"
#include "StorageManager.h"

static bool isInitialized = false;

void RewardMeterManager::init()
{
    isInitialized = true;
    jsonConfig = R"(
    {
    "rewardMeterData": [
      {
        "reward": [
          {
            "gold": 250
          }
        ],
        "ID": 0,
        "ad_amount": "1"
      },
      {
        "reward": [
          {
            "shuffleToken": 1
          }
        ],
        "ID": 0,
        "ad_amount": "2"
      },
      {
        "reward": [
          {
            "reviveToken": 1
          }
        ],
        "ID": 0,
        "ad_amount": "3"
      },
      {
        "reward": [
          {
            "smallKey": 1
          }
        ],
        "ID": 1,
        "ad_amount": "5"
      }
    ]
        }
    )";
    rapidjson::Document& document = BeaconManager::getInstance()->getRemoteConfigJSON();
    auto fromJsonData = rewardMeterItemConfig.fromJSON(document);
    
    if(!fromJsonData)
    {
        rapidjson::Document defaultDocument;
        defaultDocument.Parse(jsonConfig.c_str());
        
        rewardMeterItemConfig.fromJSON(defaultDocument);
    }
}

RewardMeterManager::~RewardMeterManager()
{
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    HBUserDefaults::getInstance()->setDoubleForKey(REMAINING_TIME_KEY, currentTimeSeconds);
}

void RewardMeterManager::resetTimer()
{
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();

    HBUserDefaults::getInstance()->setDoubleForKey(REMAINING_TIME_KEY, currentTimeSeconds + ConfigManager::getInstance()->REWARD_METER_RESET_TIME);    
    syncToRemote();
}

void RewardMeterManager::resetTheRewardMeterCount()
{
    HBUserDefaults::getInstance()->setIntegerForKey(REWARD_METER_SAVED_AMOUNT, 0);
    syncToRemote();
}

bool RewardMeterManager::isAvailable()
{
    return TutorialManager::checkConditionsMet(ConfigManager::getInstance()->REWARD_METER_ENABLE_CONDITIONS) && ConfigManager::getInstance()->REWARD_METER_ACTIVE > 0;
}

double RewardMeterManager::getRemainingTime()
{
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    const double resetTime = HBUserDefaults::getInstance()->getDoubleForKey(REMAINING_TIME_KEY, 0);
    
    return resetTime - currentTimeSeconds;
}

int RewardMeterManager::getRewardMeterCount()
{
    return HBUserDefaults::getInstance()->getIntegerForKey(REWARD_METER_SAVED_AMOUNT, 0);
}

void RewardMeterManager::increaseRewardMeterCount()
{
    auto rewardMeterCount = getRewardMeterCount();
    HBUserDefaults::getInstance()->setIntegerForKey(REWARD_METER_SAVED_AMOUNT, (rewardMeterCount+1));
    
    syncToRemote();
}

//

void RewardMeterManager::syncToRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::REWARD_METER), StorageManager::mergeValueArray({
        std::to_string((long)HBUserDefaults::getInstance()->getDoubleForKey(REMAINING_TIME_KEY, 0)),
        std::to_string(HBUserDefaults::getInstance()->getIntegerForKey(REWARD_METER_SAVED_AMOUNT, 0))
    },STORAGE_SUB_DELIMITER));
}

void RewardMeterManager::syncFromRemote()
{
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::REWARD_METER), [=](std::string remoteVal){
        
        if (remoteVal.empty()) return;
        
        std::vector<std::string> splitted;
        
        StorageManager::splitValueString(remoteVal, &splitted, STORAGE_SUB_DELIMITER);
        
        if (splitted.size() != 2) return;
        
        HBUserDefaults::getInstance()->setDoubleForKey(REMAINING_TIME_KEY, StorageManager::getLong(splitted[0]));
        HBUserDefaults::getInstance()->setIntegerForKey(REWARD_METER_SAVED_AMOUNT, StorageManager::getInt(splitted[1]));
    });
}

void RewardMeterManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::REWARD_METER), STORAGE_EMPTY_VALUE);
}

//

static RewardMeterManager *sharedRewardMeterManager = NULL;

RewardMeterManager* RewardMeterManager::getInstance(bool initialize)
{
    if (!sharedRewardMeterManager)
    {
        sharedRewardMeterManager = new RewardMeterManager();
    }
    
    if (!initialize)
    {
        return sharedRewardMeterManager;
    }

    if (!isInitialized)
    {
        sharedRewardMeterManager->init();
    }

    return sharedRewardMeterManager;
}


