#ifndef TournamentJoinPopup_h
#define TournamentJoinPopup_h

#include "../../Core.h"
#include "../../Menu/PopupNode.hpp"

USING_NS_CC;

class TournamentJoinPopup : public PopupNode
{
    
public:
    
    TournamentJoinPopup* setup(FormattedStringBuilder infoTextBuilder);
    
    void onExit() override;
    
    CREATE_POPUP(TournamentJoinPopup);
};

#endif /* TournamentJoinPopup_h */
