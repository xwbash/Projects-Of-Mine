#include "NoAdsInfoPopup.h"
#include "../GameManager.h"
#include "../Menu/MenuNode.h"
#include "../Menu/PopupLayer.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"

NoAdsInfoPopup* NoAdsInfoPopup::setup()
{
    width = GET_WIDTH * .9f * getSafeSceneScale();
    
    MenuNode::current()->currencyBarPanel->pushArrangement({});
    
    PopupNode::setUp(width, Size(968, 984), "", FormattedStringBuilder("No Ads"_ignoreFormat));
    
    auto subBg = Sprite::create("ui/common/container/Container_Popup_01.png");
    subBg->setContentSize(Size(912, 754));
    subBg->setCenterRectNormalized(Rect(.25f, .25f, .5f, .5f));
    FIT_W(subBg, baseRect.size.width * .936f);
    subBg->setPositionY(baseRect.size.width * -.05f);
    addChild(subBg);
    
    //
    
    LightedSprite* noAdsIcon = LightedSprite::create("ui/store/noAds/Icon_NoAds.png");
    FIT_W(noAdsIcon, width * .275f);
    noAdsIcon->setProperties("noAdsIconPopup", 2, Vec4(1.f, 1.f, 1.f, .2f), 1.f, 1.f, .1f);
    noAdsIcon->setPosition(width * -.25f, baseRect.size.height * .12f);
    addChild(noAdsIcon);
    
    HBLabel* infoLabel1 = HBLabel::create("Every purchase includes No Ads! Remove #forced ads!#", FONTS::USE_LOCALIZATION, width * .045f);
    infoLabel1->setTextColor(Color4B(208, 211, 221, 255));
    infoLabel1->colorWordsBetweenSymbols({{'#', Color3B(255, 134, 156)}}, Color3B::WHITE);
    infoLabel1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    infoLabel1->setAlignment(TextHAlignment::LEFT);
    infoLabel1->setDimensions(width * .5f, 0);
    infoLabel1->setPosition(width * -.05f, noAdsIcon->getPositionY());
    addChild(infoLabel1);
    
    //
    
    Sprite* divider = Sprite::create("ui/common/container/Divider.png");
    divider->setCenterRectNormalized(Rect(.4f, .0f, .2f, 1.f));
    divider->setContentSize(Size(baseRect.size.width * .86f, baseRect.size.width * .005f));
    FIT_W(divider, baseRect.size.width * .8f);
    divider->setPositionY(noAdsIcon->getBoundingBox().getMinY() - baseRect.size.height * .08f);
    addChild(divider);
    
    //
    
    Sprite* playIcon = Sprite::create("ui/store/noAds/Icon_RewardedAds_Flat.png");
    FIT_W(playIcon, width * .095f);
    playIcon->setPosition(noAdsIcon->getPositionX(), divider->getPositionY() - baseRect.size.height * .15f);
    addChild(playIcon);
    
    HBLabel* infoLabel2 = HBLabel::create("Rewarded videos will still be optional", FONTS::USE_LOCALIZATION, infoLabel1->getFontSize());
    infoLabel2->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    infoLabel2->setAlignment(TextHAlignment::LEFT);
    infoLabel2->setDimensions(width * .5f, 0);
    infoLabel2->setTextColor(Color4B(208, 211, 221, 255));
    infoLabel2->setPosition(width * -.15f, playIcon->getPositionY());
    addChild(infoLabel2);
    
    //
    
    return this;
}

void NoAdsInfoPopup::onExit()
{
    if(MenuNode::current() && MenuNode::current()->currencyBarPanel)
    {
        MenuNode::current()->currencyBarPanel->popArrangement();
    }
    
    onExitPopup();
    
    Node::onExit();
}
