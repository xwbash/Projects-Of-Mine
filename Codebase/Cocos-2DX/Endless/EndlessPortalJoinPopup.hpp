//
//  EndlessPortalJoinPopup.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 17.11.2023.
//

#ifndef EndlessPortalJoinPopup_hpp
#define EndlessPortalJoinPopup_hpp

#include "../../Core.h"
#include "../../Menu/PopupNode.hpp"

class EndlessPortalJoinPopup : public PopupNode
{
public:
    EndlessPortalJoinPopup* setup(FormattedStringBuilder infoTextFormat);
    void onExit() override;
    CREATE_POPUP(EndlessPortalJoinPopup);
    
};

#endif /* EndlessPortalJoinPopup_hpp */
