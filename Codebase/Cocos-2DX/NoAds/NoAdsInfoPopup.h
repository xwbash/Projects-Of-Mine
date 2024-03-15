#ifndef NoAdsInfoPopup_h
#define NoAdsInfoPopup_h

#include "../Core.h"
#include "../Menu/PopupNode.hpp"
#include "../UI/HBLabel.h"
#include "../UI/ButtonImage.h"

USING_NS_CC;

class NoAdsInfoPopup : public PopupNode
{
public:
    
    NoAdsInfoPopup* setup();
    
    void onExit() override;
    
    CREATE_POPUP(NoAdsInfoPopup);
    
private:
    
    float width = .0f;
    
};

#endif /* NoAdsInfoPopup_h */
