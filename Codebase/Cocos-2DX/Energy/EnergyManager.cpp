#include "EnergyManager.h"
#include "HBUserDefaults.h"
#include "UserSettings.h"
#include "StoreManager.h"
#include "GameManager.h"
#include "ConfigManager.h"
#include "DataEventManager.h"
#include "RemoteAchievementManager.h"
#include "StorageManager.h"
#include "TimeManager.h"
#include "RemoteBattlePassManager.h"
#include "Notification/Manager.hpp"

static bool isInitialized = false;

void EnergyManager::init()
{
    isInitialized = true;
    ENERGY_COST_PER_PLAY = ConfigManager::getInstance()->ENERGY_COST_PER_PLAY;
    
    ENERGY_KEY = "currentEnergy18";
    ENERGY_REFILL_START_TIME_KEY = "energyRefillStartTime18";
}

void EnergyManager::increaseEnergy(int value)
{
    HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), std::max(0,  getCurrentEnergy() + value));
    
    sendGasData(HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax()));
}

bool EnergyManager::isOutOfEnergy(int customCostPerPlay)
{
    if(customCostPerPlay > 0)
    {
        return getCurrentEnergy() < customCostPerPlay;
    }
    
    return getCurrentEnergy() < ENERGY_COST_PER_PLAY;
}

void EnergyManager::clearData()
{
    HBUserDefaults::getInstance()->deleteValueForKey(ENERGY_KEY.c_str());
    HBUserDefaults::getInstance()->deleteValueForKey(ENERGY_REFILL_START_TIME_KEY.c_str());
}

void EnergyManager::spendEnergy(std::string reason, int customCostPerPlay)
{
    int energy = getCurrentEnergy();
    
    if(energy == getEnergyMax())
    {
        const double currentTimeSeconds = TimeManager::getInstance()->getTime();
        
        HBUserDefaults::getInstance()->setDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), currentTimeSeconds);
    }
    
    int amount;
    
    if(customCostPerPlay > 0)
    {
        amount = customCostPerPlay;
    }
    else
    {
        amount = ENERGY_COST_PER_PLAY;
    }
    
    energy -= amount;
    
    HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), std::max(0, energy));
    
    sendGasData(HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax()));
    
    RemoteAchievementManager::getInstance()->updateAchievement(AchievementType::SPEND_ZZ_GAS, amount);
    
    Notification::Manager::getInstance().updateNotification(this);
    
    // send event
    DataEventManager::getInstance()->send("currency_change", {
        
        {"category" , Value(reason) },
        {"change_type" , Value("spend") },
        {"currency_type" , Value("energy") },
        {"delta" , Value(amount) }
    });
}

int EnergyManager::getCurrentEnergy(bool setToRemote)
{
    int startingEnergy = HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax());
    int energy = startingEnergy;
    
    if(energy < getEnergyMax())
    {
        const auto ENERGY_SINGLE_REFILL = ConfigManager::getInstance()->ENERGY_SINGLE_REFILL;
        
        const double currentTimeSeconds = TimeManager::getInstance()->getTime();
        
        double refillStartTimeSeconds = HBUserDefaults::getInstance()->getDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), 0);
        
        while(currentTimeSeconds - refillStartTimeSeconds > ENERGY_SINGLE_REFILL)
        {
            refillStartTimeSeconds += ENERGY_SINGLE_REFILL;
            
            energy++;
            
            if(energy == getEnergyMax())
            {
                break;
            }
        }
        
        if(energy < getEnergyMax())
        {
            HBUserDefaults::getInstance()->setDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), refillStartTimeSeconds);
        }
        
        HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), energy);
        
        if(setToRemote && energy != startingEnergy)
        {
            sendGasData(energy);
        }
    }
    
    return energy;
}

int EnergyManager::getEnergyMax()
{
    return ConfigManager::getInstance()->ENERGY_MAX + RemoteBattlePassManager::getInstance()->getBenefitAmount(BattlePassBenefitType::GAS);
}

bool EnergyManager::isEnergyFull()
{
    return getCurrentEnergy() >= getEnergyMax();
}

int EnergyManager::getSecondsLeftForNextFreeEnergy()
{
    const auto ENERGY_SINGLE_REFILL = ConfigManager::getInstance()->ENERGY_SINGLE_REFILL;
    
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    const double refillStartTimeSeconds = HBUserDefaults::getInstance()->getDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), currentTimeSeconds);
   
    const double timePassed = currentTimeSeconds - refillStartTimeSeconds;
    
    return (ENERGY_SINGLE_REFILL - timePassed);
}

///

#define ENERGY_VIDEO_TIME_LIMITED_KEY "videoEnergy"

int EnergyManager::getNumberOfVideoPurchasesLeft(bool syncToRemote)
{
    const int val = GameManager::getInstance()->getTimeLimitedValue(ENERGY_VIDEO_TIME_LIMITED_KEY,
                                                           ConfigManager::getInstance()->ENERGY_VIDEO_MAX,
                                                           24 * 60 * 60);
    
    if (syncToRemote)
    {
        sendGasData(HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax()));
    }
    
    return val;
}

void EnergyManager::purchaseEnergyWithVideo()
{
    GameManager::getInstance()->decreaseTimeLimitedValue(ENERGY_VIDEO_TIME_LIMITED_KEY,
                                                         ConfigManager::getInstance()->ENERGY_VIDEO_MAX,
                                                         24 * 60 * 60);
    
    //
    
    const auto ENERGY_AMOUNT_VIDEO = (ConfigManager::getInstance()->ENERGY_AMOUNT_VIDEO);
    
    HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), getCurrentEnergy() + ENERGY_AMOUNT_VIDEO);
    
    sendGasData(HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax()));
    
    Notification::Manager::getInstance().updateNotification(this);
    
    //
    
    for(auto* bar : GameManager::getInstance()->activeEnergyBars)
    {
        bar->updateLabel();
    }
}

void EnergyManager::purchaseEnergyWithPrice(std::function<void()> callback)
{
    const auto ENERGY_PRICE = (ConfigManager::getInstance()->ENERGY_PRICE);
    
    StoreManager::getInstance()->makePurchase(ENERGY_PRICE, "purchaseEnergy", [=](){
        
        const auto ENERGY_AMOUNT_DIAMOND = (ConfigManager::getInstance()->ENERGY_AMOUNT_DIAMOND);
        
        HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), getCurrentEnergy() + ENERGY_AMOUNT_DIAMOND);
        
        sendGasData(HBUserDefaults::getInstance()->getIntegerForKey(ENERGY_KEY.c_str(), getEnergyMax()));
    
        Notification::Manager::getInstance().updateNotification(this);
        
        for(auto* bar : ( GameManager::getInstance()->activeEnergyBars))
        {
            bar->updateLabel();
        }
        
        if(callback)
        {
            callback();
        }
        
    });
}


void EnergyManager::onUpdateOSNotification(const Notification::IManager* manager)
{
    if(isEnergyFull())
    {
        unscheduleNotification("EnergyManager");
        return;
    }
    
    const auto ENERGY_SINGLE_REFILL = (ConfigManager::getInstance()->ENERGY_SINGLE_REFILL);
    
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    const double refillStartTimeSeconds = HBUserDefaults::getInstance()->getDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), 0);
    
    const int timeLeftForNextFill = ENERGY_SINGLE_REFILL - (currentTimeSeconds - refillStartTimeSeconds);
    
    
    const int currentEnergy = getCurrentEnergy();
    
    const int totalTimeSeconds = timeLeftForNextFill + (getEnergyMax() - currentEnergy - 1) * ENERGY_SINGLE_REFILL;
    
    scheduleNotification("EnergyManager", "🚁 Helicopter gas is fully refilled!", totalTimeSeconds, true);
}

//
void EnergyManager::sendGasData(int gasAmount)
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::GAS), std::to_string(gasAmount));
    
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::GAS_DATA),
       StorageManager::mergeValueArray({
        std::to_string((long)HBUserDefaults::getInstance()->getDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), 0)),
        StorageManager::mergeValueArray({
            std::to_string(GameManager::getInstance()->getTimeLimitedValueStartTime(ENERGY_VIDEO_TIME_LIMITED_KEY)),
            std::to_string(getNumberOfVideoPurchasesLeft(false))
        }, STORAGE_SUB_DELIMITER)
    })
   );
}

void EnergyManager::syncToRemote()
{
    sendGasData(getCurrentEnergy(false));
}

void EnergyManager::syncFromRemote()
{
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::GAS), [=](std::string valueStr){
        if (valueStr.empty()) return;
        
        HBUserDefaults::getInstance()->setIntegerForKey(ENERGY_KEY.c_str(), StorageManager::getInt(valueStr));
    });
    
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::GAS_DATA), [=](std::string valueStr){
        if (valueStr.empty()) return;

        std::vector<std::string> splitted;
        
        StorageManager::splitValueString(valueStr, &splitted);
        
        if (splitted.size() != 2) return;
        
        HBUserDefaults::getInstance()->setDoubleForKey(ENERGY_REFILL_START_TIME_KEY.c_str(), StorageManager::getLong(splitted[0]));
        
        std::vector<std::string> videoLimitedValues;
        
        StorageManager::splitValueString(splitted[1], &videoLimitedValues, STORAGE_SUB_DELIMITER);

        GameManager::getInstance()->setTimeLimitedValueStartTime(ENERGY_VIDEO_TIME_LIMITED_KEY, StorageManager::getLong(videoLimitedValues[0]));
        GameManager::getInstance()->setTimeLimitedValueAmount(ENERGY_VIDEO_TIME_LIMITED_KEY, StorageManager::getInt(videoLimitedValues[1]));
        
    });
}

void EnergyManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::GAS), STORAGE_EMPTY_VALUE);
}

///

static EnergyManager *_sharedEnergyManager = NULL;

EnergyManager* EnergyManager::getInstance(bool initialize)
{
    if (! _sharedEnergyManager)
    {
        _sharedEnergyManager = new EnergyManager();
        

    }
    if (!initialize)
    {
        return _sharedEnergyManager;
    }
    if (!isInitialized)
    _sharedEnergyManager->init();

    return _sharedEnergyManager;
}

void EnergyManager::dispose()
{
    isInitialized = false;
    
    Notification::Manager::getInstance().unbindListener(_sharedEnergyManager);

    if(_sharedEnergyManager)
    {
        delete _sharedEnergyManager;
        _sharedEnergyManager = NULL;
    }
}
