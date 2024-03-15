#ifndef IdleEarningManager_h
#define IdleEarningManager_h

#include "Core.h"
#include "IStorageHandler.h"
#include "Notification/INotificationObserver.hpp"

class IdleEarningManager : public IStorageHandler, public Notification::INotificationObserver
{
public:
    
    // STATIC FUNCTIONS
    
    static IdleEarningManager* getInstance(bool initialize = true);
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    
    static void dispose();
    
    // FUNCTIONS
    
    void init();
    
    std::vector<RewardData> getStandardEarningRewards();
    std::vector<RewardData> getQuickEarningRewards();
    int getRewardAmountWithType(int calculatedRewardSeconds, RewardType type);

    
    void resetPassedTime();

    int getPassedTime();
    int getRemainTimeToActivation();
    
    bool isAvailable();
    bool isLocked();
    
    // LIMITED VALUES
    
    void purchaseWithEnergy();
    int getNumberOfVideoPurchasesLeft();
    int getVideoNextRefillTime();
    
    void purchaseWithVideo();
    int getNumberOfEnergyPurchasesLeft();
    int getEnergyNextRefillTime();
    
    //
    
    void onUpdateOSNotification(const Notification::IManager* manager) override;

    // VARIABLES
    
    int maxAccumulationHours = 0;
    int quickEarningMinutes = 0;
    int quickEarningEnergyCost = 0;
    int minimumActivationMinutes = 5;
    
private:
    
    
    std::vector<RewardData> getCalculatedRewards(int calculatedRewardSeconds);
    
};

#endif
