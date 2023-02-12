//
//  SimulatorSelectionNode.cpp
//  Assassin
//
//  Created by Yiğithan Aydın on 17.10.2022.
//

#include "SimulatorSelectionNode.h"
#include "ResolutionSimulatorManager.h"
#include "../HBUserDefaults.h"
#include "../IntroScene.h"
#include "../HBLabel.h"


SimulatorSelectionNode* SimulatorSelectionNode::setUp()
{
    std::vector<SimulatorData> simulatorArray;
    
    auto scrollNode = Node::create();
    addChild(scrollNode);
    
    auto iphoneX = SimulatorData();
    iphoneX.name = "Iphone X";
    iphoneX.resolutionX = 946;
    iphoneX.resolutionY = 2048;
    iphoneX.isHasXFrame = true;
    iphoneX.isHasDynamicIsland = false;
    
    simulatorArray.push_back(iphoneX);
    
    auto iphoneXS = SimulatorData();
    iphoneXS.name = "Iphone XS";
    iphoneXS.resolutionX = 1125;
    iphoneXS.resolutionY = 2436;
    iphoneXS.isHasXFrame = true;
    iphoneX.isHasDynamicIsland = false;
    
    simulatorArray.push_back(iphoneXS);
    
    auto iphone5 = SimulatorData();
    iphone5.name = "Iphone 5";
    iphone5.resolutionX = 640 * 1.5f;
    iphone5.resolutionY = 1136 * 1.5f;
    iphone5.isHasXFrame = false;
    iphoneX.isHasDynamicIsland = false;

    simulatorArray.push_back(iphone5);
    
    auto ipad = SimulatorData();
    ipad.name = "Ipad";
    ipad.resolutionX = 768 * 1.5f;
    ipad.resolutionY = 1024 * 1.5f;
    ipad.isHasXFrame = false;
    iphoneX.isHasDynamicIsland = false;

    simulatorArray.push_back(ipad);
    
    auto ipad11pro = SimulatorData();
    ipad11pro.name = "Ipad Pro 11";
    ipad11pro.resolutionX = 1668 * .5f;
    ipad11pro.resolutionY = 2388 * .5f;
    ipad11pro.isHasXFrame = false;
    iphoneX.isHasDynamicIsland = false;
    
    simulatorArray.push_back(ipad11pro);
    
    
    auto iphone14ProMax = SimulatorData();
    iphone14ProMax.name = "Iphone 14 Pro Max";
    iphone14ProMax.resolutionX = 1290 * .5f;
    iphone14ProMax.resolutionY = 2796 * .5f;
    iphone14ProMax.isHasXFrame = false;
    iphone14ProMax.isHasDynamicIsland = true;
    
    simulatorArray.push_back(iphone14ProMax);

    
    float posY = + GET_WIDTH * .2f * simulatorArray.size() * .5f;
    
    auto layer = LayerColor::create(Color4B::BLACK);
    layer->setPosition(-GET_WIDTH/2, -GET_HEIGHT/2);
    scrollNode->addChild(layer);
    
    for(auto& server : simulatorArray)
    {
        auto button = newItem(server);
        button->setPosition(0, posY);
        scrollNode->addChild(button);
        
        posY -= GET_WIDTH * .2f;
    }
    
    //Director::getInstance()->getOpenGLView()->setFrameSize(900, 1600);
    //Director::getInstance()->getOpenGLView()->setDesignResolutionSize(900, 1600, ResolutionPolicy::EXACT_FIT);
    return this;
}

ButtonImage* SimulatorSelectionNode::newItem(SimulatorData data)
{
    ButtonImage* button = ButtonImage::create()->init("empty.png");
    button->setPriority(-10000);
    button->fitWH(GET_WIDTH, GET_WIDTH * .15f);
    button->mSprite->setColor(Color3B(50, 50, 50));
    
    button->cancelOnMove = true;
    
    
    HBLabel* label = HBLabel::create(data.name, FONTS::FREDOKA, GET_WIDTH * .065f);
    label->limitSize(Vec2(button->mSprite->getBoundingBox().size.width * .975f, 0.f));
    button->mNode->addChild(label);
    
    auto buttonFunction = ([=](ButtonImage*){
        Director::getInstance()->getOpenGLView()->setFrameSize(data.resolutionX, data.resolutionY);
        Director::getInstance()->getOpenGLView()->setDesignResolutionSize(data.resolutionX, data.resolutionY, ResolutionPolicy::EXACT_FIT);
    
        Size size;
        
        size.width = data.resolutionX;
        size.height = data.resolutionY;
        
        ResolutionSimulatorManager::getInstance()->setSimulatorResolution(size);
        ResolutionSimulatorManager::getInstance()->setIphoneFrameActive(data.isHasXFrame);
        ResolutionSimulatorManager::getInstance()->setDynamicIslandActive(data.isHasDynamicIsland);
        
        Director::getInstance()->replaceScene(IntroScene::create());
        
    });
    
    button->onTap(buttonFunction);

    
    
    return button;
}
