//
//  SimulatorSelectionScene.cpp
//  HunterHeroes
//
//  Created by Yiğithan Aydın on 20.10.2022.
//

#include "LanguageSelectionScene.h"

cocos2d::Scene* LanguageSelectionScene::createScene()
{
    return LanguageSelectionScene::create();
}

bool LanguageSelectionScene::init()
{
    if (!Scene::init()) return false;
    
    auto node = LanguageSelectionNode::create()->setUp();
    node->setPosition(GET_WIDTH*.5f,GET_HEIGHT*.5f);
    addChild(node);
    
    return true;
}
