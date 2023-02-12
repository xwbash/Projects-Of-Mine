//
//  SimulatorSelectionScene.h
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#ifndef LanguageSelectionScene_h
#define LanguageSelectionScene_h

#include "../Core.h"
#include "LanguageSelectionNode.h"

class LanguageSelectionScene : public cocos2d::Scene
{
public:
    virtual bool init() override;
    static cocos2d::Scene* createScene();
    CREATE_FUNC(LanguageSelectionScene);

};
#endif /* LanguageSelectionScene_h */
