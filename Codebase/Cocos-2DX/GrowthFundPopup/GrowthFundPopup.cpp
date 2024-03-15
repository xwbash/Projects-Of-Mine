//
//  GrowthFundPopup.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 5.03.2024.
//

#include "GrowthFundPopup.hpp"
#include "GameManager.h"
#include "IAPManager.h"
#include "HapticFeedbackManager.h"
#include "AdManager.h"
#include "AudioManager.h"
#include "../../Menu/MenuNode.h"
#include "../../UI/ScrollNode.h"

GrowthFundPopup* GrowthFundPopup::setUp()
{
    isFullScreen = true;
    popupName = "battlePassPopup";
    
    /// MARK: UPDATE CONTENT
    
    auto updateContent = [=](){
        for (int i = 0; i < bubbleNodes.size(); i++)
        {
            bubbleNodes[i]->updateBubble();
        }
        
        for(int i = 0; i < growthFundNodes.size(); i++)
        {
            growthFundNodes[i]->updateContent();
        }
        
        if(progressBar)
        {
            progressBar->updateContent(GrowthFundManager::getInstance().getLevel(), GrowthFundManager::getInstance().getRewardCount());
        }
        
    };

    MenuNode::current()->currencyBarPanel->pushArrangement({});

    float width = GET_WIDTH * 1.1f *  getSafeSceneScale();

    /// MARK: POPUP BACKGROUND
    auto popUpBackground = Sprite::create("empty.png");
    FIT_WH(popUpBackground, GET_WIDTH, GET_HEIGHT);
    popUpBackground->setColor(Color3B(28,28,57));
    addChild(popUpBackground);

    Node* scrollBackgroundNode = Node::create();
    addChild(scrollBackgroundNode);
    
    ScrollNode* scrollNode = ScrollNode::create();
    addChild(scrollNode);
    
    Node* topNode = Node::create();
    addChild(topNode);
    
    if(ADMIN_PLAYER)
    {
        ButtonImage* infoButton = ButtonImage::create()->init("ui/common/button/Button_Info.png");
        infoButton->fitW(width * .1f);
        infoButton->setPosition(GET_WIDTH * -.5f + infoButton->getBoundingBox().size.width, GET_HEIGHT * .5f - getSafeTopOffset() - infoButton->getBoundingBox().size.height);
        infoButton->setPopupButtonPriority();
        addChild(infoButton);
        
        infoButton->onTap([=](){
            if(ADMIN_PLAYER)
            {
                updateContent();
            }
            
        });
    }
    
     
    ButtonImage* exitIcon = ButtonImage::create()->init("ui/common/button/Button_Cancel.png");
    exitIcon->fitH(width * .1f);
    exitIcon->setPosition(GET_WIDTH * .415f, GET_HEIGHT * .5f - getSafeTopOffset() - exitIcon->getBoundingBox().size.height);
    exitIcon->setPopupButtonPriority();
    exitIcon->onTap([=](){
        PopupLayer::current()->destroyPopup();
    });
    addChild(exitIcon);
    
    
    auto background = Sprite::create("ui/popup/growthFundPopup/Background_GrowthFund.png");
    FIT_W(background, width);
    background->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    background->setPositionY(GET_HEIGHT * .5f);
    topNode->addChild(background);
    
    /// MARK: BUBBLE NODES

    bool growthPremiumRowBoughted = GrowthFundManager::getInstance().isRowPurchased(GROWTH_PREMIUM_ROW);
    bool growthExclusiveRowBoughted = GrowthFundManager::getInstance().isRowPurchased(GROWTH_EXCLUSIVE_ROW);
    
    BubbleNode* bubbleNodeFree = BubbleNode::create()->setup(width * .27f, true, GROWTH_FREE_ROW, updateContent);
    BubbleNode* bubbleNodePremium = BubbleNode::create()->setup(width * .27f, growthPremiumRowBoughted, GROWTH_PREMIUM_ROW, updateContent);
    BubbleNode* bubbleNodeExclusive = BubbleNode::create()->setup(width * .27f, growthExclusiveRowBoughted, GROWTH_EXCLUSIVE_ROW, updateContent);
    
    bubbleNodeFree->setPositionY(background->getBoundingBox().getMinY());
    bubbleNodePremium->setPositionY(background->getBoundingBox().getMinY());
    bubbleNodeExclusive->setPositionY(background->getBoundingBox().getMinY());
    
    topNode->addChild(bubbleNodeFree);
    topNode->addChild(bubbleNodePremium);
    topNode->addChild(bubbleNodeExclusive);
    
    HBLabel* descriptionLabel= HBLabel::create(FormattedStringBuilder("You can swipe this offer as cloud account by using left."), FONTS::Barlow, width * .035f);
    descriptionLabel->setColor(Color3B(208, 211, 221));
    descriptionLabel->setDimensions(width * .65f, 0);
    descriptionLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    descriptionLabel->setPositionY(bubbleNodePremium->getPositionY() + bubbleNodePremium->bubbleSize.height * .9f);
    topNode->addChild(descriptionLabel);
    
    
    HBLabel* titleLabel = HBLabel::createWithOutlineStyle(FormattedStringBuilder("GROWTH FUND"), FONTS::Barlow, width * .06f);
    titleLabel->enableGradient(Color4B(82, 225, 245, 255), -45 , 90 , 2.f);
    titleLabel->setPositionY(descriptionLabel->getBoundingBox().getMaxY() + titleLabel->getBoundingBox().size.height * .65f);
    topNode->addChild(titleLabel);
    
    Sprite* dividerPremium = Sprite::create("ui/popup/growthFundPopup/Divider_GrowthFund_Yellow.png");
    Sprite* dividerFree= Sprite::create("ui/popup/growthFundPopup/Divider_GrowthFund_Free.png");
    Sprite* dividerExclusive= Sprite::create("ui/popup/growthFundPopup/Divider_GrowthFund_Purple.png");
    
    dividerFree->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    dividerPremium->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    dividerExclusive->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    
    FIT_W(dividerFree, (width * .95f)/3);
    FIT_W(dividerExclusive, (width * .95f)/3);
    FIT_W(dividerPremium, (width * .95f)/3);
    
    dividerExclusive->setPosition(dividerPremium->getBoundingBox().getMaxX(), (bubbleNodeFree->getPositionY() - bubbleNodeFree->bubbleSize.height * .85f));
    dividerFree->setPosition(dividerPremium->getBoundingBox().getMinX(), (bubbleNodeFree->getPositionY() - bubbleNodeFree->bubbleSize.height * .85f));
    dividerPremium->setPosition(0, (bubbleNodeFree->getPositionY() - bubbleNodeFree->bubbleSize.height * .85f));
    
    topNode->addChild(dividerFree);
    topNode->addChild(dividerPremium);
    topNode->addChild(dividerExclusive);
    
    bubbleNodeFree->setPositionX(dividerFree->getBoundingBox().getMidX());
    bubbleNodePremium->setPositionX(dividerPremium->getBoundingBox().getMidX());
    bubbleNodeExclusive->setPositionX(dividerExclusive->getBoundingBox().getMidX());
    
    bubbleNodes.push_back(bubbleNodeFree);
    bubbleNodes.push_back(bubbleNodePremium);
    bubbleNodes.push_back(bubbleNodeExclusive);

    /// MARK: SCROLL BACKGROUND
    Sprite* freeContainerBackground = Sprite::create("ui/popup/growthFundPopup/Container_GrowtFundRewards_Free.png");
    Sprite* premiumContainerBackground = Sprite::create("ui/popup/growthFundPopup/Container_GrowtFundRewards_Yellow.png");
    Sprite* exclusiveContainerBackground = Sprite::create("ui/popup/growthFundPopup/Container_GrowtFundRewards_Purple.png");
    
    auto containerSize = (GET_HEIGHT * -.5f) - dividerFree->getBoundingBox().getMidY();
    
    FIT_WH(freeContainerBackground, dividerFree->getBoundingBox().size.width, abs(containerSize));
    FIT_WH(premiumContainerBackground, dividerPremium->getBoundingBox().size.width, abs(containerSize));
    FIT_WH(exclusiveContainerBackground, dividerExclusive->getBoundingBox().size.width, abs(containerSize));
    
    freeContainerBackground->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    premiumContainerBackground->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    exclusiveContainerBackground->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    
    freeContainerBackground->setPosition(dividerFree->getBoundingBox().getMidX(), dividerFree->getBoundingBox().getMidY());
    premiumContainerBackground->setPosition(dividerPremium->getBoundingBox().getMidX(), dividerPremium->getBoundingBox().getMidY());
    exclusiveContainerBackground->setPosition(dividerExclusive->getBoundingBox().getMidX(), dividerExclusive->getBoundingBox().getMidY());
    
    scrollBackgroundNode->addChild(freeContainerBackground);
    scrollBackgroundNode->addChild(premiumContainerBackground);
    scrollBackgroundNode->addChild(exclusiveContainerBackground);
    
    /// MARK: SCROLL
    
    auto scrollArea = Rect(0, 0, width * .95f, abs(containerSize));
    scrollArea.origin = scrollArea.size * -.5f;
    scrollNode->setup(scrollArea, true, true, false, true, true);
    scrollNode->setPositionY(freeContainerBackground->getBoundingBox().getMidY());
    
    scrollNode->setTopScrollLimit(0);
    
    /// MARK: Reward Nodes
    
    for (int i = 1; i <= GrowthFundManager::getInstance().getRewardCount(); i++)
    {
        auto levelUnlock = GrowthFundManager::getInstance().getUnlockLevel(i, GROWTH_FREE_ROW);
        
        auto growthFundRewardNodeFree = GrowthFundRewardNode::create()->setup(levelUnlock, width * .15f, GROWTH_FREE_ROW, updateContent);
        auto growthFundRewardNodePremium = GrowthFundRewardNode::create()->setup(levelUnlock, width * .15f, GROWTH_PREMIUM_ROW, updateContent);
        auto growthFundRewardNodeExclusive = GrowthFundRewardNode::create()->setup(levelUnlock, width * .15f, GROWTH_EXCLUSIVE_ROW, updateContent);
        
        float yPosition = (i-1) * (width * -.25f) + width * .35f;
        
        growthFundRewardNodeFree->setPosition(Vec2(bubbleNodeFree->getPositionX(), yPosition));
        growthFundRewardNodePremium->setPosition(Vec2(bubbleNodePremium->getPositionX(), yPosition));
        growthFundRewardNodeExclusive->setPosition(Vec2(bubbleNodeExclusive->getPositionX(), yPosition));
        
        scrollNode->contentNode->addChild(growthFundRewardNodeFree);
        scrollNode->contentNode->addChild(growthFundRewardNodePremium);
        scrollNode->contentNode->addChild(growthFundRewardNodeExclusive);
        
        growthFundNodes.push_back(growthFundRewardNodeFree);
        growthFundNodes.push_back(growthFundRewardNodePremium);
        growthFundNodes.push_back(growthFundRewardNodeExclusive);
    }
    
    scrollNode->setBottomScrollLimit(-growthFundNodes.back()->getPositionY() - scrollArea.size.height * .5f + width * .15f);


    /// MARK: Progress Level Bar
    
    auto bottomPosition = growthFundNodes.back()->getPositionY();
    auto upPosition = growthFundNodes[0]->getPositionY();
    auto yPosition = growthFundNodes.back()->getPositionY() + growthFundNodes[0]->getPositionY();
    
    progressBar = ProgressLevelBar::create()->setup(GrowthFundManager::getInstance().getLevel(), GrowthFundManager::getInstance().getRewardCount(), width * .03f, abs(bottomPosition - upPosition));
    progressBar->setPosition(freeContainerBackground->getBoundingBox().getMaxX(), yPosition/2);
    scrollNode->contentNode->addChild(progressBar);
    
    return this;
}

void ProgressLevelBar::updateContent(int level, int objectCount)
{
    auto progressHeight = baseProgressBarNode->height;
    auto maxLevel = GrowthFundManager::getInstance().getUnlockLevel(objectCount, GROWTH_FREE_ROW);
    
    if(maxLevel <= level && divider)
    {
        divider->removeFromParent();
        divider = nullptr;
    }
    
    for (int i = 0; i < objectCount; i++)
    {
        int realLevel = GrowthFundManager::getInstance().getUnlockLevel(i + 1, GROWTH_FREE_ROW);
        std::string fileName = realLevel <= level ? "Container_GrowthFundLevel_Yellow.png" : "Container_GrowthFundLevel_Default.png";
        
        progressSprites[i]->setTexture("ui/popup/growthFundPopup/" + fileName);
    }
    
    for(int i = 1; i <= progressSprites.size(); i++)
    {
        auto realLevel = GrowthFundManager::getInstance().getUnlockLevel(i, GROWTH_FREE_ROW);
        
        if(realLevel <= level)
        {
            auto index = i - 1;
            auto positionY = progressSprites[index]->getPositionY();
               
            if((index+1) < progressSprites.size())
            {
                auto nextPositionY = progressSprites[index+1]->getPositionY();
                auto yPositionOnProgress = (positionY+nextPositionY) / 2.0f;
                
                auto progressThresh = MathUtil::inverseLerp(progressHeight * 0.5f, progressHeight * -0.5f, yPositionOnProgress);

                baseProgressBarNode->animateProgressChange(progressThresh, .2f, [=](){
                    if(divider)
                    {
                        divider->setVisible(true);
                        divider->setPositionY(yPositionOnProgress);
                    }
                });
                
                
            }
            else
            {
                auto progressThresh = MathUtil::inverseLerp(progressHeight * 0.5f, progressHeight * -0.5f, positionY);

                if(divider)
                {
                    divider->runAction(FadeOut::create(0.2f));
                }
            
                baseProgressBarNode->animateProgressChange(progressThresh, .2f);
            }
        }
    }
    

}


ProgressLevelBar* ProgressLevelBar::setup(int level, int objectCount, float progressWidth, float progressHeight)
{
    float width = GET_WIDTH * 1.1f *  getSafeSceneScale();
    auto maxLevel = GrowthFundManager::getInstance().getUnlockLevel(objectCount, GROWTH_FREE_ROW);

    baseProgressBarNode = BaseVerticalProgressBarNode::create();
    
    baseProgressBarNode->setUp(.0f, "empty.png", "empty.png", progressWidth, progressHeight, ContentDirection::DOWN);
    baseProgressBarNode->fill->setColor(Color3B(243, 213, 52));
    baseProgressBarNode->fillBg->setColor(Color3B(68, 100, 161));
    addChild(baseProgressBarNode);
    
    if(maxLevel > level)
    {
        divider = Sprite::create("ui/common/container/Divider_Yellow.png");
        FIT_WH(divider, width, width * .005f);
        addChild(divider);
    }
    
    for (int i = 0; i < objectCount; i++)
    {
        auto posY = MathUtil::lerp(progressHeight * 0.5f, progressHeight * -0.5f, static_cast<float>(i) / (objectCount - 1));
        
        int realLevel = GrowthFundManager::getInstance().getUnlockLevel(i + 1, GROWTH_FREE_ROW);
        std::string fileName = realLevel <= level ? "Container_GrowthFundLevel_Yellow.png" : "Container_GrowthFundLevel_Default.png";
        
        auto circleSprite = Sprite::create("ui/popup/growthFundPopup/" + fileName);
        FIT_W(circleSprite, progressWidth * 2.5f);
        circleSprite->setPositionY(posY);
        addChild(circleSprite);
        
        HBLabel* levelLabel = HBLabel::create(std::to_string(realLevel), FONTS::Barlow, progressWidth * 1.2f);
        levelLabel->setPositionY(posY);
        addChild(levelLabel);
        
        progressSprites.push_back(circleSprite);
     
    }
    
    for(int i = 1; i <= progressSprites.size(); i++)
    {
        auto realLevel = GrowthFundManager::getInstance().getUnlockLevel(i, GROWTH_FREE_ROW);
        
        if(realLevel <= level)
        {
            auto index = i - 1;
            auto positionY = progressSprites[index]->getPositionY();
               
            if((index+1) < progressSprites.size())
            {
                auto nextPositionY = progressSprites[index+1]->getPositionY();
                auto yPositionOnProgress = (positionY+nextPositionY) / 2.0f;
                
                auto progressThresh = MathUtil::inverseLerp(progressHeight * 0.5f, progressHeight * -0.5f, yPositionOnProgress);

                baseProgressBarNode->changeProgress(progressThresh);
                
                if(divider)
                {
                    divider->setVisible(true);
                    divider->setPositionY(yPositionOnProgress);
                }
            }
            else
            {
                auto progressThresh = MathUtil::inverseLerp(progressHeight * 0.5f, progressHeight * -0.5f, positionY);

                if(divider)
                {
                    divider->setVisible(false);
                }
                
                
                baseProgressBarNode->changeProgress(progressThresh);
            }
        }
    }
    
    this->level = level;
    
    return this;
}

RewardState GrowthFundRewardNode::getRewardState(int level, GrowthFundType growthFundType)
{
    if(!GrowthFundManager::getInstance().isRowPurchased(growthFundType))
    {
        type = 0;
        return LOCKED;
    }
    else if(GrowthFundManager::getInstance().isRewardClaimed(level, growthFundType))
    {
        type = 2;
        return CLAIMED;
    }
    else if(level <= GrowthFundManager::getInstance().getLevel())
    {
        type = 1;
        return READY_TO_CLAIM;
    }
    
    return LOCKED;
}

GrowthFundRewardNode* GrowthFundRewardNode::setup(int level, float width, GrowthFundType growthFundType, std::function<void()> updateContent)
{
    this->growthFundType = growthFundType;
    this->level = level;
    
    RewardState rewardState = getRewardState(level, growthFundType);
    
    if(growthFundType == GROWTH_PREMIUM_ROW)
    {
        Sprite* background = Sprite::create("ui/popup/battlePassPurchase/Container_SeasonPass_GoldPassNode.png");
        FIT_W(background, width * 1.3f);
        addChild(background);
    }
    else if (growthFundType == GROWTH_EXCLUSIVE_ROW)
    {
        Sprite* background = Sprite::create("ui/popup/growthFundPopup/Container_GrowthFundReward_Purple.png");
        FIT_W(background, width * 1.3f);
        addChild(background);
    }
    
    /// MARK: CLAIM BUTTON
    auto freeClaimButton = UIButton::create()->setUp(UIButtonType::CUSTOM_BUTTON, FormattedStringBuilder("Claim"_ignoreFormat), width * 1.1f, Size(327, 143), 1, "ui/popup/battlePassPopup/Button_SeasonPass_Activate.png", Rect(0,0,1,1));
    freeClaimButton->setPositionY(width * .52f);
    freeClaimButton->button->setScrollButtonPriority(true);
    
    freeClaimButton->button->onTap([=](){
       if(type == 1)
       {
           if(growthFundType == GROWTH_FREE_ROW && ConfigManager::getInstance()->IS_GROWTH_FUND_ADS_ENABLED)
           {
               if(!AdManager::getInstance()->canShowRewarded())
               {
                   AudioManager::getInstance()->playEffect("locked.wav");
                   GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not ready yet!"), FONTS::USE_LOCALIZATION);
                   
                   ButtonImage::enableAllTouches();
                   
                   
                   return;
               }
               
               HapticFeedbackManager::getInstance()->play(HEAVY);
               
               AudioManager::getInstance()->playEffect("click.wav", .5f);
               
               ButtonImage::disableAllTouches();
               
               AdManager::getInstance()->showRewarded("growthFund","GrowthFund.FreeRewardClaim", [=](bool completed){
                   ButtonImage::enableAllTouches();
                   
                   if(!completed)
                   {
                       AudioManager::getInstance()->playEffect("locked.wav");
                       GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not completed!"), FONTS::USE_LOCALIZATION);
                       return;
                   }
                   
                   rewardNode->setState(CLAIMED);
                   type = 2;
                   collectAndHandleReward(*rewardNode->getRewardData(), "growth_fund", [=](){}, convertToWorldSpace(rewardNode->getPosition()));
                   GrowthFundManager::getInstance().setRewardClaimed(level, growthFundType);
                   freeClaimButton->setVisible(false);
               });
           }
           else
           {
               rewardNode->setState(CLAIMED);
               type = 2;
               collectAndHandleReward(*rewardNode->getRewardData(), "growth_fund", [=](){}, convertToWorldSpace(rewardNode->getPosition()));
               GrowthFundManager::getInstance().setRewardClaimed(level, growthFundType);
               freeClaimButton->setVisible(false);
           }
       }
    });
    
    freeClaimButton->setTextStyle(UIButtonType::GREEN);
    
    if(growthFundType == GROWTH_FREE_ROW && ConfigManager::getInstance()->IS_GROWTH_FUND_ADS_ENABLED)
    {
        Sprite* playIcon = Sprite::create("ui/common/icon/Icon_RewardedAd.png");
        FIT_W(playIcon, width * .25f);
        playIcon->setPosition(freeClaimButton->label->getBoundingBox().getMinX() - playIcon->getBoundingBox().size.width * .3f, freeClaimButton->getBoundingBox().size.height * .075f);
        freeClaimButton->button->mNode->addChild(playIcon);
        
        freeClaimButton->label->setPosition(freeClaimButton->getBoundingBox().size.width * .1f, freeClaimButton->getBoundingBox().size.height * .1f);
    }
    else
    {
        freeClaimButton->label->setPositionY(freeClaimButton->getBoundingBox().size.height * .1f);
    }
    
    freeClaimButton->runAction(Sequence::create(ScaleTo::create(.01f, 0), CallFunc::create([=](){
        freeClaimButton->setVisible(false);
    }), NULL));
    /// MARK: REWARD NODE CREATION
    
    RewardNodeData data;
    data.width = width;
    data.rewardData = GrowthFundManager::getInstance().getReward(level, growthFundType);
    data.state = rewardState;
    
    rewardNode = RewardNode::create();
    rewardNode->setUp(data);
    rewardNode->button->setScrollButtonPriority(true);
    rewardNode->button->isInMaskedArea = true;

    addChild(rewardNode);
    addChild(freeClaimButton);

    rewardNode->button->onTap([=](){
        if(type == 1)
        {
            if(freeClaimButton->isVisible())
            {
                freeClaimButton->runAction(Sequence::create(ScaleTo::create(.1f, 0), CallFunc::create([=](){
                    freeClaimButton->setVisible(false);
                }), NULL));
            }
            else
            {
                freeClaimButton->setVisible(true);
                freeClaimButton->runAction(Sequence::create(ScaleTo::create(.1f, 1), CallFunc::create([=](){
                    
                }), NULL));
            }
            
        }
    });
     
    std::string iconLockPath = "ui/common/icon/";
    
    switch (growthFundType) {
        case GROWTH_FREE_ROW:
            iconLockPath += "Icon_Lock.png";
            break;
        case GROWTH_PREMIUM_ROW:
            iconLockPath += "Icon_Lock_Gold.png";
            break;
        case GROWTH_EXCLUSIVE_ROW:
            iconLockPath += "Icon_Lock_Purple.png";
            break;
        default:
            iconLockPath += "Icon_Lock.png";
            break;
    }
    
    
    if(rewardState == LOCKED)
    {
        lockedSprite = Sprite::create(iconLockPath);
        FIT_W(lockedSprite, rewardNode->button->getBoundingBox().size.width * .45f);
        lockedSprite->setPosition(Vec2(rewardNode->button->getBoundingBox().getMinX() * .9f, rewardNode->button->getBoundingBox().getMinY() * .9f));
        lockedSprite->setRotation(-18.2f);
        rewardNode->button->mNode->addChild(lockedSprite);
    }
    
    rewardNode->removeLockSprite();
    
    return this;
}

void GrowthFundRewardNode::updateContent()
{
    RewardState rewardState = getRewardState(level, growthFundType);
    
    rewardNode->setState(rewardState);
    
    std::string iconLockPath = "ui/common/icon/";
    
    switch (growthFundType) {
        case GROWTH_FREE_ROW:
            iconLockPath += "Icon_Lock.png";
            break;
        case GROWTH_PREMIUM_ROW:
            iconLockPath += "Icon_Lock_Gold.png";
            break;
        case GROWTH_EXCLUSIVE_ROW:
            iconLockPath += "Icon_Lock_Purple.png";
            break;
        default:
            iconLockPath += "Icon_Lock.png";
            break;
    }
    
    if(rewardState == LOCKED && !lockedSprite)
    {
        lockedSprite = Sprite::create(iconLockPath);
        FIT_W(lockedSprite, rewardNode->button->getBoundingBox().size.width * .45f);
        lockedSprite->setPosition(Vec2(rewardNode->button->getBoundingBox().getMinX() * .9f, rewardNode->button->getBoundingBox().getMinY() * .9f));
        lockedSprite->setRotation(-18.2f);
        addChild(lockedSprite);
    }
    else if((rewardState == READY_TO_CLAIM || rewardState == CLAIMED))
    {
        runAction(Sequence::create(CallFunc::create([=](){
            if(lockedSprite)
            {
                lockedSprite->runAction(ScaleTo::create(.2f, lockedSprite->getScale() * 1.2f));
                lockedSprite->runAction(FadeOut::create(.4f));
            }
            
        }), DelayTime::create(.3f), CallFunc::create([=](){
            
            if(lockedSprite)
            {
                lockedSprite->removeFromParent();
                lockedSprite = nullptr;
            }
            
        }), NULL));
    }
    
    rewardNode->removeLockSprite();
}

BubbleNode* BubbleNode::setup(float width, int boughted, GrowthFundType growthFundType, std::function<void()> updateContent)
{
    this->width = width;
    this->boughted = boughted;
    this->growthFundType = growthFundType;
    
    std::string spritePath = "";
    std::string basePath = "ui/popup/growthFundPopup/";
    
    std::string iapText = "";
    
    switch (growthFundType)
    {
        case GROWTH_PREMIUM_ROW:
            iapText = GrowthFundManager::getInstance().getGrowthData().premiumProductIAP;
            break;
        case GROWTH_EXCLUSIVE_ROW:
            iapText = GrowthFundManager::getInstance().getGrowthData().exclusiveProductIAP;
            break;
            
        default:
            iapText = "";
            break;
    }
    
    switch (growthFundType)
    {
        case GROWTH_FREE_ROW:
            spritePath = basePath+"Container_GrowtFund_Free.png";
            break;
        case GROWTH_PREMIUM_ROW:
            spritePath = basePath + (boughted > 0 ? "Container_GrowtFund_Yellow.png" : "Button_GrowthFund_Yellow.png");
            break;
        case GROWTH_EXCLUSIVE_ROW:
            spritePath = basePath + (boughted > 0 ? "Container_GrowtFund_Purple.png" : "Button_GrowthFund_Purple.png");
            break;
    }
    
    backgroundImage = ButtonImage::create()->init(spritePath);
    backgroundImage->fitW(width);
    
    if(growthFundType != GROWTH_FREE_ROW && boughted < 1)
    {
        backgroundImage->setPopupButtonPriority();
        
        backgroundImage->onTap([=](){
            if(this->boughted > 0) return;
            
            IAPManager::getInstance()->purchaseWrapper(iapText, "growth_fund_purchase_popup", [=](bool success){
                
                if(success)
                {
                    this->boughted = 1;
                    GrowthFundManager::getInstance().setRowPurchased(growthFundType, 1);
                    updateContent();
                }
                
            });
            
        });
        
        if(ADMIN_PLAYER)
        {
            backgroundImage->onHold([=](){
                IAPManager::getInstance()->purchaseWrapper(iapText, "growth_fund_purchase_popup", [=](bool success){
                    this->boughted = 1;
                    GrowthFundManager::getInstance().setRowPurchased(growthFundType, 1);
                    updateContent();
                });
            });
        }
    }
    
    addChild(backgroundImage);
    
        
    switch (growthFundType)
    {
        case GROWTH_FREE_ROW:
            iapLabel = HBLabel::create(FormattedStringBuilder("Free").ignoreLocalization(), FONTS::Barlow, width * .15f);
            iapLabel->setColor(Color3B(213, 232, 255));
            iapLabel->setPositionY(backgroundImage->getBoundingBox().getMidY() + backgroundImage->getBoundingBox().size.height * .05f);
            break;
        case GROWTH_PREMIUM_ROW:
            if(boughted > 0)
            {
                iapLabel = HBLabel::create(FormattedStringBuilder("Bought").ignoreLocalization(), FONTS::Barlow, width * .15f);
                iapLabel->setColor(Color3B(30, 33, 61));
                iapLabel->setPositionY(backgroundImage->getBoundingBox().getMidY() + backgroundImage->getBoundingBox().size.height * .05f);
            }
            else
            {
                iapLabel = HBLabel::create(IAPManager::getInstance()->getProductPriceAsStringBuilder(iapText), FONTS::Barlow, width * .15f);
                iapLabel->setColor(Color3B(236, 255, 231));
                iapLabel->specializedFigmaStyle(50.f, .0f, Color4B(71, 113, 55,255), Color4B(0,0,0,0));
                iapLabel->setPositionY(backgroundImage->getBoundingBox().getMaxY() - iapLabel->getBoundingBox().size.height * 1.2f);
            }

            break;
        case GROWTH_EXCLUSIVE_ROW:
            if(boughted > 0)
            {
                iapLabel = HBLabel::create(FormattedStringBuilder("Bought").ignoreLocalization(), FONTS::Barlow, width * .15f);
                iapLabel->setColor(Color3B(30, 33, 61));
                iapLabel->setPositionY(backgroundImage->getBoundingBox().getMidY() + backgroundImage->getBoundingBox().size.height * .05f);
            }
            else
            {
                iapLabel = HBLabel::create(IAPManager::getInstance()->getProductPriceAsStringBuilder(iapText), FONTS::Barlow, width * .15f);
                iapLabel->setColor(Color3B(236, 255, 231));
                iapLabel->specializedFigmaStyle(50.f, .0f, Color4B(71, 113, 55,255), Color4B(0,0,0,0));
                iapLabel->setPositionY(backgroundImage->getBoundingBox().getMaxY() - iapLabel->getBoundingBox().size.height * 1.2f);
            }
            break;
    }
    
    backgroundImage->mNode->addChild(iapLabel);
    
    
    if(growthFundType == GROWTH_PREMIUM_ROW && boughted < 1)
    {
        buyLabel = HBLabel::create(FormattedStringBuilder("Buy"), FONTS::Barlow, width * .1f);
        buyLabel->setColor(Color3B(34, 77, 23));
        buyLabel->setPositionY(iapLabel->getPositionY() - buyLabel->getBoundingBox().size.height * 1.1f);
        backgroundImage->mNode->addChild(buyLabel);
    }
    else if(growthFundType == GROWTH_EXCLUSIVE_ROW && boughted < 1)
    {
        buyLabel = HBLabel::create(FormattedStringBuilder("Buy"), FONTS::Barlow, width * .1f);
        buyLabel->setColor(Color3B(34, 77, 23));
        buyLabel->setPositionY(iapLabel->getPositionY() - buyLabel->getBoundingBox().size.height * 1.1f);
        backgroundImage->mNode->addChild(buyLabel);
    }
    
    bubbleSize = Size(backgroundImage->getBoundingBox().size);
    
    return this;
}

void BubbleNode::updateBubble()
{
    if(growthFundType == GROWTH_FREE_ROW) return;
    this->boughted = GrowthFundManager::getInstance().isRowPurchased(growthFundType);
    
    std::string basePath = "ui/popup/growthFundPopup/";
    
    if(boughted < 1)
    {
        switch (growthFundType) {
            case GROWTH_PREMIUM_ROW:
                backgroundImage->changeTexture(basePath+"Button_GrowthFund_Yellow.png");
                break;
            case GROWTH_EXCLUSIVE_ROW:
                backgroundImage->changeTexture(basePath+"Button_GrowthFund_Purple.png");
                break;
            default:
                break;
        }
        
        if(buyLabel)
        {
            buyLabel->removeFromParent();
            buyLabel = nullptr;
        }
        
        iapLabel->removeFromParent();
        iapLabel = nullptr;
        
        std::string iapText = "";
        
        switch (growthFundType)
        {
            case GROWTH_PREMIUM_ROW:
                iapText = GrowthFundManager::getInstance().getGrowthData().premiumProductIAP;
                break;
            case GROWTH_EXCLUSIVE_ROW:
                iapText = GrowthFundManager::getInstance().getGrowthData().exclusiveProductIAP;
                break;
                
            default:
                iapText = "";
                break;
        }
        
        if(growthFundType == GROWTH_PREMIUM_ROW)
        {
            iapLabel = HBLabel::create(IAPManager::getInstance()->getProductPriceAsStringBuilder(iapText), FONTS::Barlow, width * .15f);
            iapLabel->setColor(Color3B(236, 255, 231));
            iapLabel->specializedFigmaStyle(100.0f, 0.f, Color4B(71, 113, 55,0), Color4B(0,0,0,0));
            iapLabel->setPositionY(backgroundImage->getBoundingBox().getMaxY() - iapLabel->getBoundingBox().size.height * 1.f);
            backgroundImage->mNode->addChild(iapLabel);
            
            buyLabel = HBLabel::create(FormattedStringBuilder("Buy"), FONTS::Barlow, width * .1f);
            buyLabel->setColor(Color3B(34, 77, 23));
            buyLabel->setPositionY(iapLabel->getPositionY() - buyLabel->getBoundingBox().size.height * 1.1f);
            backgroundImage->mNode->addChild(buyLabel);
        }
        
        else if(growthFundType == GROWTH_EXCLUSIVE_ROW)
        {
            iapLabel = HBLabel::create(IAPManager::getInstance()->getProductPriceAsStringBuilder(iapText), FONTS::Barlow, width * .15f);
            iapLabel->setColor(Color3B(236, 255, 231));
            iapLabel->specializedFigmaStyle(100.f, 0.f, Color4B(71, 113, 55,0), Color4B(0,0,0,0));
            iapLabel->setPositionY(backgroundImage->getBoundingBox().getMaxY() - iapLabel->getBoundingBox().size.height * 1.f);
            backgroundImage->mNode->addChild(iapLabel);
            
            buyLabel = HBLabel::create(FormattedStringBuilder("Buy"), FONTS::Barlow, width * .1f);
            buyLabel->setColor(Color3B(34, 77, 23));
            buyLabel->setPositionY(iapLabel->getPositionY() - buyLabel->getBoundingBox().size.height * 1.1f);
            backgroundImage->mNode->addChild(buyLabel);
        }
    }
    else
    {
        switch (growthFundType) {
            case GROWTH_PREMIUM_ROW:
                backgroundImage->changeTexture(basePath+"Container_GrowtFund_Yellow.png");
                break;
            case GROWTH_EXCLUSIVE_ROW:
                backgroundImage->changeTexture(basePath+"Container_GrowtFund_Purple.png");
                break;
            default:
                break;
        }
        
        if(buyLabel)
        {
            buyLabel->removeFromParent();
            buyLabel = nullptr;
        }
        
        iapLabel->removeFromParent();
        iapLabel = nullptr;
        
        iapLabel = HBLabel::create(FormattedStringBuilder("Bought").ignoreLocalization(), FONTS::Barlow, width * .15f);
        iapLabel->setColor(Color3B(30, 33, 61));
        iapLabel->setPositionY(backgroundImage->getBoundingBox().getMidY() + backgroundImage->getBoundingBox().size.height * .05f);
        backgroundImage->mNode->addChild(iapLabel);
    }
}

void GrowthFundPopup::onExit()
{
    PopupNode::onExit();
}
