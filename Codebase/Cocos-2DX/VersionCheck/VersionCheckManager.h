//
//  VersionCheckManager.h
//  Assassin
//
//  Created by Yiğithan Aydın on 11.10.2022.
//

#ifndef VersionCheckManager_h	
#define VersionCheckManager_h
#include "Core.h"

enum VersionPopUpType{
    SOFTY,
    HARD
};
class VersionCheckManager
{
    public:
        
        bool checkVersion();
        static VersionCheckManager* getInstance();
};
#endif /* VersionCheckManager_h */
