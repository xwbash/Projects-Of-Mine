#ifndef NoAdsOfferPopup_h
#define NoAdsOfferPopup_h

#include "../Core.h"
#include "../Menu/PopupNode.hpp"
#include "../UI/HBLabel.h"
#include "../UI/ButtonImage.h"

USING_NS_CC;

class NoAdsOfferPopup : public PopupNode
{
public:
    
    NoAdsOfferPopup* setup(std::vector<RewardData> rewards, bool fromInGame = false);
    
    void addOnBoughtCallback(std::function<void()> callback);
    
    void addOnCloseCallback(std::function<void()> callback);
    
    void onExit() override;
    
    CREATE_POPUP(NoAdsOfferPopup);
    
private:
    std::vector<std::function<void()>> m_onBoughtCallbacks;
    std::vector<std::function<void()>> m_onCloseCallbacks;
    float width = .0f;
};

#endif /* NoAdsOfferPopup_h */
