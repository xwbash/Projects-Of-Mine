//
//  EndlessPortalJoinPopup.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 17.11.2023.
//

#include "EndlessPortalJoinPopup.hpp"
#include "../../UI/UIButton.h"
#include "../../Menu/PopupLayer.h"
#include "../../RemoteEndlessPortalManager.hpp"
#include "../../Menu/MenuNode.h"
#include "../../HapticFeedbackManager.h"
#include "../../AudioManager.h"
#include "../../LeaderboardManager.hpp"

EndlessPortalJoinPopup* EndlessPortalJoinPopup::setup(FormattedStringBuilder infoTextFormat)
{
    popupName = "EndlessJoinPortalPopup";
    isFullScreen = false;
    
    setUninterruptible();
    
    MenuNode::current()->currencyBarPanel->pushArrangement({});
    
    float width = GET_WIDTH * 1.1f *  getSafeSceneScale();
    
    auto backgroundSprite = Sprite::create(RemoteEndlessPortalManager::getInstance()->getJoinBgPath());
    FIT_W(backgroundSprite, width);
    backgroundSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    backgroundSprite->setPositionY(GET_HEIGHT * .5f);
    addChild(backgroundSprite);
    
    auto titleLabel = HBLabel::createWithOutlineStyle(FormattedStringBuilder("Event Started"_ignoreFormat), FONTS::USE_LOCALIZATION, width * .1f);
    titleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    titleLabel->setAlignment(TextHAlignment::CENTER);
    titleLabel->setTextColor(Color4B(255, 255, 200, 255));
    titleLabel->specializedFigmaStyle(70.f, .75f, Color4B(0,0,0,0), Color4B(0,0,0,255/4));
    titleLabel->enableGradient(Color4B(245, 209, 67, 255), -45 , 90 , 2.f);
    titleLabel->setLineSpacing(width * -.02f);
    titleLabel->setPositionY(backgroundSprite->getBoundingBox().getMinY());
    addChild(titleLabel);
    
    auto desc = HBLabel::create(infoTextFormat, FONTS::USE_LOCALIZATION, width * .035f);
    desc->setPositionY((titleLabel->getPositionY() - titleLabel->getBoundingBox().size.height * 1.5f));
    desc->setTextColor(Color4B(208, 211, 221, 255));
    desc->setAlignment(cocos2d::TextHAlignment::CENTER);
    desc->setDimensions(titleLabel->getBoundingBox().size.width, 0);
    this->addChild(desc);
    
    Sprite* divider = Sprite::create("ui/common/container/Divider.png");
    divider->setCenterRectNormalized(Rect(.4f, .0f, .2f, 1.f));
    divider->setContentSize(Size(width * .847f, width * .005f));
    FIT_W(divider, width * .615f);
    divider->setPositionY((desc->getPositionY() - desc->getBoundingBox().size.height * .5f) - titleLabel->getBoundingBox().size.height * .5f);
    addChild(divider);
            
    auto getButton = UIButton::create();
    getButton->setUp(UIButtonType::GREEN, FormattedStringBuilder("Go Now"_ignoreFormat), width * .44f, Size(428, 192), .75f);
    getButton->setPositionY(divider->getPositionY() - getButton->getBoundingBox().size.height);
    getButton->button->setPopupButtonPriority();
    getButton->button->onTap([](){
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        PopupLayer::current()->destroyAllPopups();
        RemoteEndlessPortalManager::getInstance()->joinEndless();
        
        if(ConfigManager::getInstance()->ENABLE_LIVE_LEADERBOARD == 1)
        {
            LeaderboardManager::getInstance()->sendJoinRequest(RemoteEndlessPortalManager::getInstance()->getTournamentID());
        }
    });
    this->addChild(getButton);

    return this;
}


void EndlessPortalJoinPopup::onExit()
{
    MenuNode::current()->currencyBarPanel->popArrangement();
    
    onExitPopup();
    
    Node::onExit();
}
