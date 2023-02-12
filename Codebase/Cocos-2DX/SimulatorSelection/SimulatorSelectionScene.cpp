//
//  SimulatorSelectionScene.cpp
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#include "SimulatorSelectionScene.h"

cocos2d::Scene* SimulatorSelectionScene::createScene()
{
    return SimulatorSelectionScene::create();
}

bool SimulatorSelectionScene::init()
{
    if (!Scene::init()) return false;
    
    auto node = SimulatorSelectionNode::create()->setUp();
    node->setPosition(GET_WIDTH*.5f,GET_HEIGHT*.5f);
    addChild(node);
    
    return true;
}
