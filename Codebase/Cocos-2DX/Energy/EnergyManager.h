#ifndef EnergyManager_h
#define EnergyManager_h

#include "Core.h"
#include "IStorageHandler.h"
#include "Notification/INotificationObserver.hpp"

class EnergyManager : public IStorageHandler, public Notification::INotificationObserver
{
public:
    
    void init();
   
    //
    
    bool isOutOfEnergy(int customCostPerPlay = 0);
    void spendEnergy(std::string reason, int customCostPerPlay = 0);
    
    int getCurrentEnergy(bool setToRemote = true);
    int getEnergyMax();
    bool isEnergyFull();
    
    int getSecondsLeftForNextFreeEnergy();
    
    void increaseEnergy(int value);
    
    int ENERGY_COST_PER_PLAY;
    
    //
    
    int getNumberOfVideoPurchasesLeft(bool syncToRemote = true);
    void purchaseEnergyWithVideo();
    void purchaseEnergyWithPrice(std::function<void()> callback);
    
    //
    
    void clearData();
    
    void onUpdateOSNotification(const Notification::IManager* manager) override;
    
    //
    
    void sendGasData(int gasAmount);
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    
    static void dispose();
    
    //
    
    static EnergyManager* getInstance(bool initialize = true);
    
private:
    
    
    std::string ENERGY_KEY;
    std::string ENERGY_REFILL_START_TIME_KEY;

};

#endif
