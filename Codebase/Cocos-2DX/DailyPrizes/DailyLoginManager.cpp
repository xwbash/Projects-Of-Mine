#include "DailyLoginManager.h"
#include "HBUserDefaults.h"
#include "UserSettings.h"
#include "GameManager.h"
#include "ConfigManager.h"
#include "DataEventManager.h"
#include "Popups/DailyLoginPopup.h"
#include "Menu/PopupLayer.h"
#include "Gameplay/SpecialManager.h"
#include "RemoteRewardDataManager.h"
#include "TutorialManager.hpp"
#include "StorageManager.h"
#include "TimeManager.h"
#include "Notification/Manager.hpp"

#define TOTAL_DAY_COUNT_DATA_KEY "Total_Day_Data_Key"
#define TOTAL_WEEK_COUNT_DATA_KEY "Total_Week_Data_Key"
#define CURRENT_DAY_DATA_KEY "DailyLogin_CurrentDay"
#define NEXT_DAY_READY_TIME_DATA_KEY "DailyLogin_NextDayReady"
#define DAY_PERIOD_SECONDS 24 * 60 * 60

static bool isInitialized = false;

void DailyLoginManager::init()
{
    isInitialized = true;
    rewards = RemoteRewardDataManager::getInstance()->getDailyLoginRewards(7);
    chestRewards = RemoteRewardDataManager::getInstance()->getDailyChestRewards(4);
}

int DailyLoginManager::getCurrentDay()
{
    return HBUserDefaults::getInstance()->getIntegerForKey(CURRENT_DAY_DATA_KEY, 0);
}

int DailyLoginManager::getDayByIndex(int index)
{
    int week = getTotalWeek();
    index += 1;
    return ((week * 7) + index);
}

void DailyLoginManager::setTime(int time)
{
    auto currentTime = TimeManager::getInstance()->getTime();
    HBUserDefaults::getInstance()->setDoubleForKey(NEXT_DAY_READY_TIME_DATA_KEY, currentTime + time);
}

int DailyLoginManager::getTimeLeftForNextReward()
{
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    return HBUserDefaults::getInstance()->getDoubleForKey(NEXT_DAY_READY_TIME_DATA_KEY, 0) - currentTimeSeconds;
}

void DailyLoginManager::collectReward(std::function<void ()> callback, cocos2d::Vec2 startPos)
{
    int day = getCurrentDay();
    
    
    std::vector<RewardData> dayRewards = rewards[day];
    
    
    collectAndHandleRewards(dayRewards, "daily_login", [=](){
        callback();
    }, {startPos});
    
    
    /*DataEventManager::getInstance()->send("daily_reward_collect", {
        
        { "day" , Value(day) }
        
    });*/
    
    
    day = (day + 1) % 7;
    
    setCurrentDay(day);
    
    const double currentTimeSeconds = TimeManager::getInstance()->getTime();
    
    HBUserDefaults::getInstance()->setDoubleForKey(NEXT_DAY_READY_TIME_DATA_KEY, currentTimeSeconds + DAY_PERIOD_SECONDS);
    
    
    Notification::Manager::getInstance().updateNotification(this);
    
}

void DailyLoginManager::setCurrentDay(int value)
{
    HBUserDefaults::getInstance()->setIntegerForKey(CURRENT_DAY_DATA_KEY, value);
}

int DailyLoginManager::getDayPeriod()
{
    return DAY_PERIOD_SECONDS;
}

void DailyLoginManager::onUpdateOSNotification(const Notification::IManager* manager)
{
    const auto time = getTimeLeftForNextReward();
    
    if(time > 1)
    {
        unscheduleNotification("DailyLoginManager");
        scheduleNotification("DailyLoginManager", "🎁 Your daily reward is ready!", time + 40 * 60, true);
    }
}


#define DAILY_LOGIN_LAST_POPUP_SHOW_TIME_KEY "DailyLogin_lastPopupTime"

bool DailyLoginManager::checkShowPopup()
{
    if(isAvailable() && getNotificationCount() > 0)
    {
        const double currentTimeSeconds = TimeManager::getInstance()->getTime();
        
        const double lastPopupShownTime = HBUserDefaults::getInstance()->getDoubleForKey(DAILY_LOGIN_LAST_POPUP_SHOW_TIME_KEY, 0);
        
        const int period = 6 * 60 * 60;
        
        if(currentTimeSeconds - lastPopupShownTime > period)
        {
            HBUserDefaults::getInstance()->setDoubleForKey(DAILY_LOGIN_LAST_POPUP_SHOW_TIME_KEY, currentTimeSeconds);
            
            auto popup = DailyLoginPopup::create()->setUp(true);
            PopupLayer::current()->showPopup(DAILY_LOGIN_POPUP, popup);
            popup->priorityValue = PopupLayer::current()->currentPopupPriority;
            popup->updateContent(true);
            
            return true;
        }
    }
    
    return false;
}

int DailyLoginManager::getNotificationCount()
{
    if(getTimeLeftForNextReward() < 1)
    {
        return 1;
    }
    
    return 0;
}

void DailyLoginManager::increaseTotalDay()
{
    auto increasedDay = getTotalDay() + 1;
    HBUserDefaults::getInstance()->setIntegerForKey(TOTAL_DAY_COUNT_DATA_KEY, increasedDay);
    
    //
    
    syncToRemote();
    
    //
}

void DailyLoginManager::increaseAnWeek()
{
    auto increasedWeek = getTotalWeek() + 1;
    HBUserDefaults::getInstance()->setIntegerForKey(TOTAL_WEEK_COUNT_DATA_KEY, increasedWeek);
}

int DailyLoginManager::getTotalWeek()
{
    return HBUserDefaults::getInstance()->getIntegerForKey(TOTAL_WEEK_COUNT_DATA_KEY, 0);
}

int DailyLoginManager::getTotalDay()
{
    return HBUserDefaults::getInstance()->getIntegerForKey(TOTAL_DAY_COUNT_DATA_KEY, 0);
}

void DailyLoginManager::clearData()
{
    HBUserDefaults::getInstance()->deleteValueForKey(TOTAL_DAY_COUNT_DATA_KEY);
    HBUserDefaults::getInstance()->deleteValueForKey(TOTAL_WEEK_COUNT_DATA_KEY);
    HBUserDefaults::getInstance()->deleteValueForKey(NEXT_DAY_READY_TIME_DATA_KEY);
}

bool DailyLoginManager::isAvailable() {  return TutorialManager::checkConditionsMet(ConfigManager::getInstance()->DAILY_LOGIN_ENABLE_CONDITIONS); }
bool DailyLoginManager::isLocked() {  return (!TutorialManager::getInstance()->isTutorialCompleted(TutorialType::DailyLogin) && TutorialManager::checkConditionsLocked(ConfigManager::getInstance()->DAILY_LOGIN_ENABLE_CONDITIONS, "Daily_Login_Key")); }
///

static DailyLoginManager *_sharedDailyLoginManager = NULL;

DailyLoginManager* DailyLoginManager::getInstance(bool initialize)
{
    if (! _sharedDailyLoginManager)
    {
        _sharedDailyLoginManager = new DailyLoginManager();
        
        
    }
    if (!initialize)
    {
        return _sharedDailyLoginManager;
    }
    if (!isInitialized)
    _sharedDailyLoginManager->init();
    
    return _sharedDailyLoginManager;
}

void DailyLoginManager::dispose()
{
    Notification::Manager::getInstance().unbindListener(_sharedDailyLoginManager);
    isInitialized = false;

    if(_sharedDailyLoginManager)
    {
        delete _sharedDailyLoginManager;
        _sharedDailyLoginManager = NULL;
    }
}

void DailyLoginManager::syncToRemote()
{
    //
    
    std::vector<int> claimedList;
    
    std::vector<int> startValue = {8,15,22,28};
    int totalValue = (getTotalDay()-1) / startValue[startValue.size()-1];
    auto maxValue = (startValue[startValue.size()-1] * (totalValue));
    
    maxValue = totalValue < 1 ? 0 : maxValue;
    for(auto val : startValue)
    {
        claimedList.push_back(HBUserDefaults::getInstance()->getIntegerForKey(AchievementLoadingBarNode::getFormattedSaveKey("dailyLogin", (val + maxValue)).c_str(), 0));
    }
    std::string mergedClaimStr = StorageManager::mergeValueArray(claimedList, STORAGE_SUB_DELIMITER);
    
    //
    
    std::vector<std::string> valueList = {std::to_string(getTotalDay()), StringUtils::format("%ld", (long)HBUserDefaults::getInstance()->getDoubleForKey(NEXT_DAY_READY_TIME_DATA_KEY, 0)), mergedClaimStr};
    std::string valueToSend = StorageManager::mergeValueArray(valueList);
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::DAILY_LOGIN), valueToSend);
}

void DailyLoginManager::syncFromRemote()
{
    StorageManager::getInstance()->getData(StorageKey(StorageKeyType::DAILY_LOGIN), [=](std::string valueStr){
        if (valueStr.empty()) return;
        
        std::vector<std::string> vec;
        
        StorageManager::splitValueString(valueStr, &vec);
        
        if (vec.size() > 3 || vec.size() < 2) return;
        
        int totalDay = StorageManager::getInt(vec[0]);
        HBUserDefaults::getInstance()->setIntegerForKey(TOTAL_DAY_COUNT_DATA_KEY, totalDay);
        
        HBUserDefaults::getInstance()->setDoubleForKey(NEXT_DAY_READY_TIME_DATA_KEY, StorageManager::getInt(vec[1]));
        
        setCurrentDay(totalDay % 7);
        
        //
        if(vec.size() == 3)
        {
            std::vector<std::string> claimListRaw;
            StorageManager::splitValueString(vec[2], &claimListRaw, STORAGE_SUB_DELIMITER);
            
            std::vector<int> startValue = {8,15,22,28};
            int totalValue = (getTotalDay()-1) / startValue[startValue.size()-1];
            auto maxValue = (startValue[startValue.size()-1] * (totalValue));
            
            maxValue = totalValue < 1 ? 0 : maxValue;
            
            for(int i = 0; i < (int)startValue.size(); ++i)
            {
                HBUserDefaults::getInstance()->setIntegerForKey(AchievementLoadingBarNode::getFormattedSaveKey("dailyLogin", (startValue[i] + maxValue)).c_str(), StorageManager::getInt(claimListRaw[i]));
            }
        }
        
    });
}

void DailyLoginManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::DAILY_LOGIN), STORAGE_EMPTY_VALUE);
}
