//
//  RewardMeterPopup.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 31.08.2023.
//

#include "RewardMeterPopup.hpp"
#include "../RewardMeterManager.hpp"
#include "../GameManager.h"
#include "../UI/UIButton.h"
#include "../AdManager.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"
#include "../UserSettings.h"
#include "../Menu/PopupLayer.h"
#include "../UI/ToolTipNode.h"
#include "../SkipAdsManager.hpp"
#include "../Popups/CollectedRewardsPopup.h"
#include "../UI/TimerNode.hpp"

RewardMeterPopup* RewardMeterPopup::setup()
{
    float width = GET_WIDTH * .9f * getSafeSceneScale();
    
    PopupNode::setUp(width, Size(968, 1026),  "", FormattedStringBuilder("Reward Meter"_ignoreFormat));
    
    Sprite* artSprite = Sprite::create("ui/popup/rewardmeter/IMG_RewardMeter.png");
    FIT_W(artSprite, width);
    artSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    artSprite->setPositionY(baseRect.getMaxY());
    addChild(artSprite, -1);
    
    this->setPositionY(artSprite->getBoundingBox().size.height * -.5f);
    
    Sprite* subBg = Sprite::create("ui/common/container/Container_Popup_02_WithTimeContainer.png");
    subBg->setContentSize(Size(912, 628));
    subBg->setCenterRectNormalized(Rect(.25f, .7f, .5f, .05f));
    FIT_W(subBg, baseRect.size.width * .936f);
    subBg->setPositionY(baseRect.size.width * .03f);
    addChild(subBg);
    
    //
    
    TimerNodeConfig config;
    config.fontSize = width * .045f;
    config.startTime = RewardMeterManager::getInstance()->getRemainingTime();
    config.timerLabelStrBuilder = FormattedStringBuilder("Ends in: ");
    
    TimerNode* timerNode = TimerNode::create()->setup(config);
    timerNode->setPositionY(subBg->getBoundingBox().getMaxY() - width * .05f);
    addChild(timerNode);
    
    //
    
    auto createLoadingBar =[=](){
        if(loadingBar)
        {
            loadingBar->removeFromParent();
        }
        
        loadingBar = RewardMeterLoadingBar::create()->setup(subBg->getBoundingBox().size.width * .8f, subBg->getBoundingBox().size.width * .07f);
        loadingBar->setPosition(subBg->getBoundingBox().getMinX() + loadingBar->nodeSize.width * .575f, subBg->getBoundingBox().getMaxY() - baseRect.size.width * .25f);
        addChild(loadingBar);
    };
    
    createLoadingBar();
    
    
    Sprite* divider = Sprite::create("ui/common/container/Divider.png");
    divider->setCenterRectNormalized(Rect(.4f, .0f, .2f, 1.f));
    divider->setContentSize(Size(width * .86f, width * .005f));
    FIT_W(divider, width * .86f);
    divider->setPositionY(loadingBar->getPositionY() - width * .17f);
    addChild(divider);
    
    auto rewardedLabel = HBLabel::create(FormattedStringBuilder("Watch optional ads and earn rewards"), FONTS::USE_LOCALIZATION, subBg->getBoundingBox().size.width * .055f);
    rewardedLabel->setAlignment(TextHAlignment::CENTER);
    rewardedLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    rewardedLabel->setDimensions(width * .7f, 0);
    rewardedLabel->setTextColor(Color4B(208, 211, 221, 255));
    rewardedLabel->setPositionY(divider->getPositionY() - width * 0.015f);
    addChild(rewardedLabel);
    
    //
    
    Price price;
    price.type = PRICE_VIDEO;
    
    auto rewardItemData = RewardMeterManager::getInstance()->rewardMeterItemConfig.rewardItemData;
    auto maxAdAmount = rewardItemData[rewardItemData.size()-1].adAmount;
    
    bool buttonDisabled = maxAdAmount <= RewardMeterManager::getInstance()->getRewardMeterCount();
    
    auto watchButton = UIButton::create()->setUpWithPrice(buttonDisabled ? UIButtonType::GRAY : UIButtonType::YELLOW, price,
                                                          SkipAdsManager::getInstance()->getButtonStringBuilder(), baseRect.size.width * .44f, Size(428, 192), false, .75f);
    watchButton->button->setPopupButtonPriority();
    watchButton->setPositionY(baseRect.getMinY() + watchButton->button->getBoundingBox().size.height * .65f);
    addChild(watchButton);
    
    
    std::function<void()> onUpdate = [=](){
        auto rmTime = RewardMeterManager::getInstance()->getRemainingTime();
        auto remainingTime = getRemainingTimeString(rmTime < 0 ? 0 : rmTime);
        timerNode->updateValueString(remainingTime);

        if (RewardMeterManager::getInstance()->getRemainingTime() < 0)
        {
            RewardMeterManager::getInstance()->resetTheRewardMeterCount();
            RewardMeterManager::getInstance()->resetTimer();
            
            auto rewardItemData = RewardMeterManager::getInstance()->rewardMeterItemConfig.rewardItemData;
            
            for (int i = 0; i < rewardItemData.size(); i++)
            {
                loadingBar->clearRewardClaims(i);
            }
            createLoadingBar();
            watchButton->updateButtonStyle(UIButtonType::YELLOW);
            watchButton->setTextStyle(UIButtonType::YELLOW);
        }
        else
        {
            timerNode->updateValueString(remainingTime);
        }
    };
    onUpdate();
    
    action = RepeatForever::create(Sequence::create(DelayTime::create(1.f), CallFunc::create([=](){
        onUpdate();
    }), NULL));
    
    runAction(action);
    
    watchButton->button->onTap([=](){
        //Reward On Success
        if(!AdManager::getInstance()->canShowRewarded())
        {
            AudioManager::getInstance()->playEffect("locked.wav");
            GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not ready yet!"), FONTS::USE_LOCALIZATION);
            
            ButtonImage::enableAllTouches();
            
            canBeClosedByOutsideTap = true;
            
            return;
        }
              
        if(maxAdAmount <= RewardMeterManager::getInstance()->getRewardMeterCount())
        {
            AudioManager::getInstance()->playEffect("locked.wav");
            GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Wait Until %s", getRemainingTimeString(RewardMeterManager::getInstance()->getRemainingTime()).format().c_str()), FONTS::USE_LOCALIZATION);
            return;
        }
        
        HapticFeedbackManager::getInstance()->play(HEAVY);
        
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        ButtonImage::disableAllTouches();
        canBeClosedByOutsideTap = false;
        
        AdManager::getInstance()->showRewarded("rewardMeter","RewardVideo.RewardMeter", [=](bool completed){
            
            ButtonImage::enableAllTouches();
            canBeClosedByOutsideTap = true;
            
            if(!completed)
            {
                AudioManager::getInstance()->playEffect("locked.wav");
                GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not completed!"), FONTS::USE_LOCALIZATION);
                return;
            }
            
            RewardMeterManager::getInstance()->increaseRewardMeterCount();
            loadingBar->updateBar();
            
            bool buttonDisabled = maxAdAmount <= RewardMeterManager::getInstance()->getRewardMeterCount();
            UIButtonType buttonType = buttonDisabled ? UIButtonType::GRAY : UIButtonType::YELLOW;
            
            if(!SkipAdsManager::getInstance()->canSkipAds() && !buttonDisabled)
            {
                watchButton->updateButtonPath();
            }
            
            watchButton->updateButtonStyle(buttonType);
            watchButton->setTextStyle(buttonType);
        });
       
    });
    
    if(ADMIN_PLAYER)
    {
        watchButton->button->onHold([=](){
            RewardMeterManager::getInstance()->resetTimer();
            RewardMeterManager::getInstance()->resetTheRewardMeterCount();
            auto rewards = RewardMeterManager::getInstance()->rewardMeterItemConfig.rewardItemData;
            for(int i = 0; i < rewards.size(); i++)
            {
                loadingBar->clearRewardClaims(i);
            }
            PopupLayer::current()->showPopup(REWARD_METER_POPUP, RewardMeterPopup::create()->setup());
        });
    }
    
    return this;
}

void RewardMeterPopup::onExit()
{
    if (ButtonImage::isAllTouchesDisabled())
    {
        ButtonImage::enableAllTouches();
    }
    
    Node::onExit();
}

RewardMeterLoadingBar* RewardMeterLoadingBar::setup(float width, float height)
{
    progressBar = BaseProgressBarNode::create()->setUpWith9slice("ui/common/progressbar/ProgressBar_Background.png", "ui/common/progressbar/ProgressBar_Green.png", width, height, 0 ,0 ,1 , false, Size(width, height), Size(width, height));
    addChild(progressBar);
    
    auto rewardMeterCounter = Sprite::create("ui/popup/rewardmeter/Icon_RewardMeterCounter.png");
    FIT_W(rewardMeterCounter, width * .15f);
    rewardMeterCounter->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    rewardMeterCounter->setPosition(progressBar->getBoundingBox().getMinX() * .9f, progressBar->getBoundingBox().getMidY() + rewardMeterCounter->getBoundingBox().size.height * .1f);
    addChild(rewardMeterCounter);
    
    rewardAdMeterLabel = HBLabel::create(std::to_string(RewardMeterManager::getInstance()->getRewardMeterCount()).c_str(), FONTS::USE_LOCALIZATION, rewardMeterCounter->getBoundingBox().size.width);
    rewardAdMeterLabel->setTextColor(Color4B(230, 244, 247, 255));
    rewardAdMeterLabel->enableHBStyle(1, 0, Color4B(17, 81, 177, 255));
    rewardAdMeterLabel->limitSize(rewardMeterCounter->getBoundingBox().size * .5f);
    rewardAdMeterLabel->setPosition(rewardMeterCounter->getBoundingBox().getMidX(), rewardMeterCounter->getBoundingBox().size.height * -.04f);
    addChild(rewardAdMeterLabel);
    
    currentRewardAmount = RewardMeterManager::getInstance()->getRewardMeterCount();
    
    auto rewardItemData = RewardMeterManager::getInstance()->rewardMeterItemConfig.rewardItemData;
    
    for(int i = 0; i < rewardItemData.size(); i++)
    {
        Node* rewardNodeContainer = Node::create();
        addChild(rewardNodeContainer);
        
        auto rewardData = rewardItemData[i].rewardData;
        auto adAmount = rewardItemData[i].adAmount;
        
        RewardNodeData rewardNodeData;
        rewardNodeData.width = width * .18f;
        rewardNodeData.rewardData = rewardData;
        rewardNodeData.state = RewardState::READY_TO_CLAIM;
        
        if(adAmount > currentRewardAmount)
        {
            rewardNodeData.playShineEffect = true;
        }
        else
        {
            rewardNodeData.playShineEffect = false;
        }
        
        auto rewardNode = RewardNode::create()->setUp(rewardNodeData);
        rewardNodeContainer->addChild(rewardNode);
        
        auto adAmountLabel = HBLabel::create(std::to_string(adAmount).c_str(), FONTS::USE_LOCALIZATION, rewardNode->button->getBoundingBox().size.width * .27f);
        adAmountLabel->setPositionY(rewardNode->button->getBoundingBox().size.height * -.75f);
        adAmountLabel->setTextColor(Color4B(230, 244, 247, 255));
        adAmountLabel->enableHBStyle(1, 0, Color4B(17, 81, 177, 255));
        rewardNodeContainer->addChild(adAmountLabel);
        
        tickedButtons.push_back(false);
        
        
        float startPosition = (-progressBar->getBoundingBox().size.width * .5f) + rewardMeterCounter->getBoundingBox().size.width * 1.2f;
        float endPosition = (progressBar->getBoundingBox().size.width * .5f) - rewardNode->getBoundingBox().size.width * .45f;
        float percentage = (float) i / (float) (rewardItemData.size() - 1);
         
        
        auto position = MathUtil::lerp(startPosition, endPosition, percentage);
        
        rewardNodeContainer->setPosition(Vec2(position, progressBar->getBoundingBox().getMidY()));
        
        
        RewardMeterRewardData rewardMeterRewardData;
        rewardMeterRewardData.rewardNode = rewardNode;
        rewardMeterRewardData.labelUnder = adAmountLabel;
        rewardMeterRewardData.rewardMeterItem = rewardItemData[i];
        
        rewardMeterRewardDatas.push_back(rewardMeterRewardData);
        
        if(adAmount <= currentRewardAmount)
        {
            rewardNode->button->onTap([=](){
                auto text = FormattedStringBuilder("You already claimed the reward.");
                ToolTipNode::create(rewardNode->button, text);

            });
            
            
            addTick(rewardNode, i, rewardNodeContainer->getPosition());
            
            adAmountLabel->setColor(Color3B::GREEN);
        }
        else
        {
            rewardNode->button->onTap([=](){
                
                auto text = FormattedStringBuilder("Watch %1$i %2$s to collect reward.", adAmount, adAmount > 1 ? "ads" : "ad");
                ToolTipNode::create(rewardNode->button, text);
            });
        }
        
        rewardNode->button->setPopupButtonPriority();
        
    }
    
    nodeSize = Size(rewardMeterCounter->getBoundingBox().size.width + width, rewardMeterCounter->getBoundingBox().size.height);
    updateBar();
    return this;
}

void RewardMeterLoadingBar::clearRewardClaims(int index)
{
    auto key = StringUtils::format("REWARD_METER_REWARDNODE_CLAIMED_INDEX_%i", index);
    
    if(HBUserDefaults::getInstance()->getIntegerForKey(key.c_str(), 0) > 0)
    {
        HBUserDefaults::getInstance()->setIntegerForKey(key.c_str(), 0);
    }
}

void RewardMeterLoadingBar::updateBar()
{
    float rewardMeterCount = (float) RewardMeterManager::getInstance()->getRewardMeterCount();
    auto rewardItemData = RewardMeterManager::getInstance()->rewardMeterItemConfig.rewardItemData;
    float maxAdAmount = (float) rewardItemData[rewardItemData.size()-1].adAmount;
    
    auto percentage = (rewardMeterCount - 0)/(maxAdAmount - 0);
    
    rewardAdMeterLabel->setString(std::to_string((int)rewardMeterCount));
    progressBar->changeProgress(percentage);
    
    for(int i = 0; i < rewardMeterRewardDatas.size(); i++)
    {
        auto adAmount = rewardMeterRewardDatas[i].rewardMeterItem.adAmount;
        
        if(adAmount <= rewardMeterCount)
        {
            rewardMeterRewardDatas[i].labelUnder->setColor(Color3B::GREEN);
            
            addTick(rewardMeterRewardDatas[i].rewardNode, i, rewardMeterRewardDatas[i].rewardNode->getParent()->getPosition());
            
            rewardMeterRewardDatas[i].rewardNode->toggleLightedSprite(false);
            
            
            auto distanceBetweenThem = rewardMeterRewardDatas[i].rewardNode->getParent()->getPosition().x - progressBar->getPosition().x;
            
            auto minWidth = -rewardMeterRewardDatas[i].rewardNode->button->getBoundingBox().size.width;
            auto maxWidth = rewardMeterRewardDatas[i].rewardNode->button->getBoundingBox().size.width;
            
            auto widthPercentageLerp = ((distanceBetweenThem/2) - minWidth) / (maxWidth - minWidth);
            
            
            if(percentage <= widthPercentageLerp)
            {
               progressBar->changeProgress(widthPercentageLerp);
            }
            
            rewardMeterRewardDatas[i].rewardNode->button->onTap([=](){
                auto text = FormattedStringBuilder("You already claimed the reward.");
                ToolTipNode::create(rewardMeterRewardDatas[i].rewardNode->button, text);

            });
        }
        else
        {
            rewardMeterRewardDatas[i].rewardNode->button->onTap([=](){
                
                auto text = FormattedStringBuilder("Watch %i ads to collect reward.", adAmount);
                ToolTipNode::create(rewardMeterRewardDatas[i].rewardNode->button, text);
            });
        }
    }
    
}

void RewardMeterLoadingBar::addTick(RewardNode* rewardNode, int index, Vec2 targetPosition)
{
    
    auto key = StringUtils::format("REWARD_METER_REWARDNODE_CLAIMED_INDEX_%i", index);
    
    if(HBUserDefaults::getInstance()->getIntegerForKey(key.c_str(), 0) < 1)
    {
        HBUserDefaults::getInstance()->setIntegerForKey(key.c_str(), 1);
        std::vector<RewardData> rewardDatas;
        rewardDatas.push_back(*rewardNode->getRewardData());
        //PopupLayer::current()->showPopup(COLLECTED_REWARDS_POPUP, CollectedRewardsPopup::create()->setUp(rewardDatas));
        collectAndHandleReward(*rewardNode->getRewardData(), "rewardMeter");
    }
    
    if(!tickedButtons[index])
    {
        auto tickIcon = Sprite::create("ui/common/icon/Icon_Check.png");
        FIT_W(tickIcon, rewardNode->button->getBoundingBox().size.width * .75f);
        tickIcon->setPosition(rewardNode->button->getBoundingBox().size.width * .43f, rewardNode->button->getBoundingBox().size.height * .43f);
        rewardNode->button->mNode->addChild(tickIcon);
    }
    
    
    
    tickedButtons[index] = true;
}


RewardMeterButton* RewardMeterButton::setup(float width)
{
    buttonImage = ButtonImage::create()->init("ui/popup/rewardmeter/Icon_RewardMeter_MainMenu.png");
    buttonImage->fitW(width);
    addChild(buttonImage);
    
    if(GameManager::getInstance()->SHOW_MISSION_COMPLETED_ANIMATION_ON_MENU ||
       GameManager::getInstance()->SHOW_LAST_LOOT_ANIMATION_ON_MENU)
    {
        hide();
    }
    
    return this;
}

void RewardMeterButton::hide()
{
    for(auto* child : getChildren())
    {
        child->setVisible(false);
        child->setScale(0.f);
    }
}

void RewardMeterButton::show()
{
    for(auto* child : getChildren())
    {
        child->stopAllActions();
        child->setVisible(true);
        child->runAction(EaseSineOut::create(ScaleTo::create(.2f, 1.f)));
    }
}

