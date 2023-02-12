//
//  SimulatorSelectionNode.cpp
//  Assassin
//
//  Created by Yiğithan Aydın on 17.10.2022.
//

#include "LanguageSelectionNode.h"
#include "../HBUserDefaults.h"
#include "../IntroScene.h"
#include "../HBLabel.h"


LanguageSelectionNode* LanguageSelectionNode::setUp()
{
    std::vector<LanguageData> languageArray;
    
    auto scrollNode = Node::create();
    addChild(scrollNode);
    
    auto english = LanguageData();
    english.name = "ENGLISH";
    english.ID = 1;
    english.langType = cocos2d::LanguageType::ENGLISH;
    
    languageArray.push_back(english);
    
    auto french = LanguageData();
    french.name = "FRENCH";
    french.ID = 2;
    french.langType = cocos2d::LanguageType::FRENCH;
    
    languageArray.push_back(french);
    
    auto chinese = LanguageData();
    chinese.name = "CHINESE";
    chinese.ID = 3;
    chinese.langType = cocos2d::LanguageType::CHINESE;
    
    languageArray.push_back(chinese);
    
    auto japanese = LanguageData();
    japanese.name = "JAPANESE";
    japanese.ID = 4;
    japanese.langType = cocos2d::LanguageType::JAPANESE;
    
    languageArray.push_back(japanese);
    
    auto spanish = LanguageData();
    spanish.name = "SPANISH";
    spanish.ID = 5;
    spanish.langType = cocos2d::LanguageType::SPANISH;
    
    languageArray.push_back(spanish);
    
    auto russian = LanguageData();
    russian.name = "RUSSIAN";
    russian.ID = 6;
    russian.langType = cocos2d::LanguageType::RUSSIAN;
    
    languageArray.push_back(russian);
    
    auto italian = LanguageData();
    italian.name = "ITALIAN";
    italian.ID = 7;
    italian.langType = cocos2d::LanguageType::ITALIAN;
    
    languageArray.push_back(italian);
    
    auto german = LanguageData();
    german.name = "GERMAN";
    german.ID = 8;
    german.langType = cocos2d::LanguageType::GERMAN;
    
    languageArray.push_back(german);
    
    float posY = + GET_WIDTH * .2f * languageArray.size() * .5f;
    
    auto layer = LayerColor::create(Color4B::BLACK);
    layer->setPosition(-GET_WIDTH/2, -GET_HEIGHT/2);
    scrollNode->addChild(layer);
    
    for(auto& server : languageArray)
    {
        auto button = newItem(server);
        button->setPosition(0, posY);
        scrollNode->addChild(button);
        
        posY -= GET_WIDTH * .2f;
    }
    
    return this;
}

ButtonImage* LanguageSelectionNode::newItem(LanguageData data)
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
        
        UserDefault::getInstance()->setIntegerForKey("LANGUAGE", data.ID);
        
        exit(0);
    });
    
    button->onTap(buttonFunction);

    
    
    return button;
}
