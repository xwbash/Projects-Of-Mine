#include "IdleEarningManager.h"
#include "HBUserDefaults.h"
#include "ConfigManager.h"
#include "RemoteRewardDataManager.h"
#include "EnergyManager.h"
#include "GameManager.h"
#include "UserSettings.h"
#include "TutorialManager.hpp"
#include "ConverterManager.h"
#include "StoreManager.h"
#include "StorageManager.h"
#include "TimeManager.h"
#include "Notification/Manager.hpp"

#define LAST_COLLECT_TIME "IdleEarning_lastCollectTime"
#define IDLE_ENERGY_PURCHASE_KEY "idle_energy_purchase"
#define IDLE_VIDEO_PURCHASE_KEY "idle_video_purchase"

static bool isInitialized = false;

void IdleEarningManager::init()
{
    isInitialized = true;
    maxAccumulationHours = ConfigManager::getInstance()->IDLE_EARNING_MAX_ACCUMULATION_HOURS;
    quickEarningMinutes = ConfigManager::getInstance()->QUICK_EARNING_MINUTES;
    quickEarningEnergyCost = ConfigManager::getInstance()->QUICK_EARNING_ENERGY_COST;
}

int IdleEarningManager::getRewardAmountWithType(int calculatedRewardSeconds, RewardType type)
{
    int amount = 0;
    auto rewards = getCalculatedRewards(calculatedRewardSeconds);
    
    for (auto& reward : rewards)
    {
        if(reward.type != type) continue;
        
        if(reward.isCountValueSignificant()) amount += reward.getCount();
        else amount += reward.getValue();
    }
    
    return amount;
}

std::vector<RewardData> IdleEarningManager::getCalculatedRewards(int calculatedRewardSeconds)
{    
    /*const auto secondsInDay = (60 * 60 * 24);
    
    const auto singleEnergyRefillSec = ConfigManager::getInstance()->ENERGY_SINGLE_REFILL;
    auto producableEnergyCountPerDay = secondsInDay / (float)singleEnergyRefillSec;
    
    const auto energyCostPerPlay = EnergyManager::getInstance()->ENERGY_COST_PER_PLAY;
    auto playableGamePerDay = producableEnergyCountPerDay / (float) energyCostPerPlay;
    
    auto days = calculatedRewardSeconds / (float) secondsInDay;
    float rewardMultiplier = playableGamePerDay * days * ConfigManager::getInstance()->IDLE_REWARD_MULTIPLIER;
    */
    
    /// REWARDS CALCULATION
    
    std::vector<RewardData> rewards;
      
    const float gameplay = ConverterManager::getInstance()->ConvertOfflineTimeToGameplay(calculatedRewardSeconds / 60);
    
    const float gem = ConverterManager::getInstance()->ConvertGameplayToGem(gameplay, 0) / UNIQUE_REWARD_COUNT;
    
    const int gold = std::round(ConverterManager::getInstance()->ConvertGemToGold(gem, 0));
    if (gold > 0)
    {
        rewards.push_back(RewardData(REWARD_GOLD, gold));
    }
    
    const int exp = std::round(ConverterManager::getInstance()->ConvertGemToXP(gem, 0));
    if (exp > 0)
    {
        rewards.push_back(RewardData(REWARD_XP, exp));
    }

    if (TutorialManager::getInstance()->isTutorialCompleted(TutorialType::Equipment))
    {
        const int blueprint = std::round(ConverterManager::getInstance()->ConvertGemToBlueprint(gem, 0));
        if (blueprint > 0)
        {
            rewards.push_back(RewardData(REWARD_BLUEPRINT, -1, blueprint));
        }
    }
    
    if (TutorialManager::getInstance()->isTutorialCompleted(TutorialType::Store))
    {
        const int smallKey = std::round(ConverterManager::getInstance()->ConvertGemToSmallKey(gem));
        if (smallKey > 0)
        {
            rewards.push_back(RewardData(REWARD_KEY, (int)KeyType::SMALL_KEY, smallKey));
        }
    }

    if (TutorialManager::getInstance()->isTutorialCompleted(TutorialType::Heroes))
    {
        const int heroMat = std::round(ConverterManager::getInstance()->ConvertGemToHeroMaterial(gem, 0));
        if (heroMat > 0)
        {
            rewards.push_back(RewardData(REWARD_HERO_MATERIAL, heroMat));
        }
    }

    
    return rewards;
}

std::vector<RewardData> IdleEarningManager::getStandardEarningRewards()
{
    return getCalculatedRewards(getPassedTime());
}

std::vector<RewardData> IdleEarningManager::getQuickEarningRewards()
{
    return getCalculatedRewards(quickEarningMinutes * 60);
}

void IdleEarningManager::resetPassedTime()
{
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    HBUserDefaults::getInstance()->setDoubleForKey(LAST_COLLECT_TIME, currentTimeSeconds);
    
    //
    syncToRemote();
}

int IdleEarningManager::getPassedTime()
{
    double lastCollectSeconds = HBUserDefaults::getInstance()->getDoubleForKey(LAST_COLLECT_TIME, -1);
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();

    if (lastCollectSeconds < 0)
    {
        resetPassedTime();
        lastCollectSeconds = currentTimeSeconds;
    }

    int passedTime = currentTimeSeconds - lastCollectSeconds;
    if (passedTime < 0)
    {
        passedTime += 24 * 60 * 60;
    }
    
    passedTime = std::min(maxAccumulationHours * 60 * 60, passedTime);
    return passedTime;
}

int IdleEarningManager::getRemainTimeToActivation()
{
    int lastCollectSeconds = HBUserDefaults::getInstance()->getDoubleForKey(LAST_COLLECT_TIME, -1);
    int activationSeconds = lastCollectSeconds + minimumActivationMinutes * 60;
    
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    return activationSeconds - currentTimeSeconds;
}

//

void IdleEarningManager::purchaseWithEnergy()
{
    GameManager::getInstance()->decreaseTimeLimitedValue(IDLE_ENERGY_PURCHASE_KEY,
                                                         ConfigManager::getInstance()->IDLE_ENERGY_PURCHASE_MAX,
                                                         ConfigManager::getInstance()->IDLE_ENERGY_REFRESH_HOURS * 60 * 60);
    syncToRemote();
}

int IdleEarningManager::getNumberOfEnergyPurchasesLeft()
{
    return GameManager::getInstance()->getTimeLimitedValue(IDLE_ENERGY_PURCHASE_KEY,
                                                           ConfigManager::getInstance()->IDLE_ENERGY_PURCHASE_MAX,
                                                           ConfigManager::getInstance()->IDLE_ENERGY_REFRESH_HOURS * 60 * 60);
}

int IdleEarningManager::getEnergyNextRefillTime()
{
    return GameManager::getInstance()->getTimeLimitedNextRefillTime(IDLE_ENERGY_PURCHASE_KEY,
                                                                    ConfigManager::getInstance()->IDLE_ENERGY_PURCHASE_MAX,
                                                                    ConfigManager::getInstance()->IDLE_ENERGY_REFRESH_HOURS * 60 * 60);
}

void IdleEarningManager::purchaseWithVideo()
{
    GameManager::getInstance()->decreaseTimeLimitedValue(IDLE_VIDEO_PURCHASE_KEY,
                                                         ConfigManager::getInstance()->IDLE_VIDEO_PURCHASE_MAX,
                                                         ConfigManager::getInstance()->IDLE_VIDEO_REFRESH_HOURS * 60 * 60);
    syncToRemote();
}

int IdleEarningManager::getNumberOfVideoPurchasesLeft()
{
    return GameManager::getInstance()->getTimeLimitedValue(IDLE_VIDEO_PURCHASE_KEY,
                                                           ConfigManager::getInstance()->IDLE_VIDEO_PURCHASE_MAX,
                                                           ConfigManager::getInstance()->IDLE_VIDEO_REFRESH_HOURS * 60 * 60);
}

int IdleEarningManager::getVideoNextRefillTime()
{
    return GameManager::getInstance()->getTimeLimitedNextRefillTime(IDLE_VIDEO_PURCHASE_KEY,
                                                                    ConfigManager::getInstance()->IDLE_VIDEO_PURCHASE_MAX,
                                                                    ConfigManager::getInstance()->IDLE_VIDEO_REFRESH_HOURS * 60 * 60);
}


bool IdleEarningManager::isAvailable() { return TutorialManager::checkConditionsMet(ConfigManager::getInstance()->IDLE_EARNING_ENABLE_CONDITIONS);}
bool IdleEarningManager::isLocked() { return !TutorialManager::getInstance()->isTutorialCompleted(TutorialType::IdleEarning) && TutorialManager::checkConditionsLocked(ConfigManager::getInstance()->IDLE_EARNING_ENABLE_CONDITIONS, "Idle_Earning_Key");}

//

void IdleEarningManager::onUpdateOSNotification(const Notification::IManager* manager)
{
    double remainingTime = getRemainTimeToActivation();
    
    if(remainingTime > 0)
    {
        unscheduleNotification("IdleEarningManager");
        scheduleNotification("IdleEarningManager", "⌛ Guard time of your helicopter is over. Come and collect your rewards.", remainingTime + 5 * 60, true);
    }
}
//

void IdleEarningManager::syncToRemote()
{
    std::vector<std::string> values;
    values.push_back(std::to_string((int)HBUserDefaults::getInstance()->getDoubleForKey(LAST_COLLECT_TIME, 0)));
    
    values.push_back(StorageManager::mergeValueArray({ std::to_string(getNumberOfVideoPurchasesLeft()), std::to_string(GameManager::getInstance()->getTimeLimitedValueStartTime(IDLE_VIDEO_PURCHASE_KEY)) }, STORAGE_SUB_DELIMITER));
    
    values.push_back(StorageManager::mergeValueArray({ std::to_string(getNumberOfEnergyPurchasesLeft()), std::to_string(GameManager::getInstance()->getTimeLimitedValueStartTime(IDLE_ENERGY_PURCHASE_KEY)) }, STORAGE_SUB_DELIMITER));
    
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::IDLE_EARNING_LCT), StorageManager::mergeValueArray(values));
}

void IdleEarningManager::syncFromRemote()
{
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::IDLE_EARNING_LCT), [](std::string remoteStr){
        
        if (remoteStr.empty()) return;
        
        std::vector<std::string> values;
        
        StorageManager::splitValueString(remoteStr, &values);
        
        if (values.size() != 3) return;
        
        HBUserDefaults::getInstance()->setDoubleForKey(LAST_COLLECT_TIME, StorageManager::getInt(values[0]));
        
        // video
        {
            std::vector<std::string> videoPurchases;
            
            StorageManager::splitValueString(values[1], &videoPurchases, STORAGE_SUB_DELIMITER);
            
            if (videoPurchases.size() == 2)
            {
                
                GameManager::getInstance()->setTimeLimitedValueAmount(IDLE_VIDEO_PURCHASE_KEY, StorageManager::getInt(videoPurchases[0]));
                GameManager::getInstance()->setTimeLimitedValueStartTime(IDLE_VIDEO_PURCHASE_KEY, StorageManager::getLong(videoPurchases[1]));
            }
        }
        
        // energy
        {
            std::vector<std::string> energyPurchases;
            
            StorageManager::splitValueString(values[2], &energyPurchases, STORAGE_SUB_DELIMITER);
            
            if (energyPurchases.size() == 2)
            {
                
                GameManager::getInstance()->setTimeLimitedValueAmount(IDLE_ENERGY_PURCHASE_KEY, StorageManager::getInt(energyPurchases[0]));
                GameManager::getInstance()->setTimeLimitedValueStartTime(IDLE_ENERGY_PURCHASE_KEY, StorageManager::getLong(energyPurchases[1]));
            }
        }
    });
}

void IdleEarningManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::IDLE_EARNING_LCT), STORAGE_EMPTY_VALUE);
}

//

static IdleEarningManager *sharedIdleEarningManager = NULL;

IdleEarningManager* IdleEarningManager::getInstance(bool initialize)
{
    if (!sharedIdleEarningManager)
    {
        sharedIdleEarningManager = new IdleEarningManager();
        
    }
    if (!initialize)
    {
        return sharedIdleEarningManager;
    }
    if (!isInitialized)
    sharedIdleEarningManager->init();

    return sharedIdleEarningManager;
}


void IdleEarningManager::dispose()
{
    isInitialized = false;

    Notification::Manager::getInstance().unbindListener(sharedIdleEarningManager);
    
    if (sharedIdleEarningManager)
    {
        delete sharedIdleEarningManager;
        sharedIdleEarningManager = NULL;
    }
}
