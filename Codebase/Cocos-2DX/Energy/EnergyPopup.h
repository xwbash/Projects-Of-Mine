#ifndef EnergyPopup_h
#define EnergyPopup_h

#include "../Core.h"
#include "../EnergyManager.h"
#include "../Menu/PopupNode.hpp"

USING_NS_CC;

class EnergyPopup : public PopupNode
{
public:
    
    EnergyPopup* setUp(std::function<void()> onContinue = nullptr, std::function<void()> onClosePopup = nullptr);
    
    void onExit() override;
    
    CREATE_POPUP(EnergyPopup);
    
private:
    
    EnergyManager* energyManager;
    
    std::function<void()> onContinue = nullptr;
    
};

#endif /* EnergyPopup_hpp */
