#ifndef DailyLoginManager_hpp
#define DailyLoginManager_hpp

#include "Core.h"
#include "IStorageHandler.h"
#include "Notification/INotificationObserver.hpp"

class DailyLoginManager : public IStorageHandler, public Notification::INotificationObserver
{
public:
    
    void init();
    
    std::vector<std::vector<RewardData>> rewards;
    std::vector<std::vector<RewardData>> chestRewards;
    
    int getCurrentDay();
    int getDayByIndex(int index);
    void setCurrentDay(int value);
    int getTimeLeftForNextReward();
    void collectReward(std::function<void ()> callback, cocos2d::Vec2 startPos);
    void increaseTotalDay();
    void increaseAnWeek();
    
    void onUpdateOSNotification(const Notification::IManager* manager) override;
    
    int getTotalWeek();
    int getTotalDay();
    int getNotificationCount();
    
    void clearData();
    
    bool checkShowPopup();
    
    bool isAvailable();
    bool isLocked();
    
    int getDayPeriod();
    
    static DailyLoginManager* getInstance(bool initialize = true);
    
    static void dispose();
    
    void syncFromRemote() override;
    
    void syncToRemote() override;
    
    void eraseRemote() override;
    
    void setTime(int time);
private:
    
    
};

#endif
