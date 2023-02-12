//
//  SimulatorSelectionScene.h
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#ifndef SimulatorSelectionScene_h
#define SimulatorSelectionScene_h

#include "../Core.h"
#include "SimulatorSelectionNode.h"

class SimulatorSelectionScene : public cocos2d::Scene
{
public:
    virtual bool init() override;
    static cocos2d::Scene* createScene();
    CREATE_FUNC(SimulatorSelectionScene);

};
#endif /* SimulatorSelectionScene_h */
