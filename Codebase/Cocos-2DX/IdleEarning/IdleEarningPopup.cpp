#include "IdleEarningPopup.h"
#include "../Menu/PopupLayer.h"
#include "../GameManager.h"
#include "../UI/HBLabel.h"
#include "../UI/UIButton.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"
#include "../IdleEarningManager.h"
#include "../UI/RewardNode.h"
#include "../StoreManager.h"
#include "../AdManager.h"
#include "../EnergyManager.h"
#include "../PlayerLevelManager.h"
#include "../UserSettings.h"
#include "EnergyPopup.h"
#include "CollectedRewardsPopup.h"
#include "../SkipAdsManager.hpp"
#include "../UI/TimerNode.hpp"

QuickEarningPopup* QuickEarningPopup::setUp()
{
    auto rewards = IdleEarningManager::getInstance()->getQuickEarningRewards();
    
    //
    
    const float width = GET_WIDTH * .9f * getSafeSceneScale();
    
    PopupNode::setUp(width, Size(968, 1012),  "", FormattedStringBuilder("Quick Earnings"));
    
    
    auto subBg = Sprite::create("ui/common/container/Container_Popup_01.png");
    subBg->setContentSize(Size(912, 608));
    subBg->setCenterRectNormalized(Rect(.25f, .25f, .5f, .5f));
    FIT_W(subBg, baseRect.size.width * .936f);
    subBg->setPositionY(baseRect.size.height * .04f);
    addChild(subBg);
    
    float fontSize =  width * .07f;
    
    //

    Sprite* divider = Sprite::create("ui/common/container/Divider.png");
    divider->setCenterRectNormalized(Rect(.4f, .0f, .2f, 1.f));
    divider->setContentSize(Size(width * .8f, width * .0075f));
    FIT_W(divider, baseRect.size.width * .8f);
    divider->setPositionY(subBg->getBoundingBox().getMaxY() - subBg->getBoundingBox().size.height * .35f);
    addChild(divider);
    
    FormattedStringBuilder rewardInfoStrBuilder = FormattedStringBuilder("Get %d minutes of \nPatrol Earnings immediately!", IdleEarningManager::getInstance()->quickEarningMinutes);
    HBLabel* rewardInfoLabel = HBLabel::create("", FONTS::USE_LOCALIZATION, fontSize * .6f);
    rewardInfoLabel->setAlignment(TextHAlignment::CENTER);
    rewardInfoLabel->setString(rewardInfoStrBuilder);
    rewardInfoLabel->setTextColor(Color4B(208, 211, 221, 255));
    rewardInfoLabel->setPositionY(divider->getPositionY() + subBg->getBoundingBox().size.height * .15f);
    
    /// CLAIM BUTTONS
    
    // claim with ad buttons
    
    activeClaimWithAdButton = UIButton::create();
    activeClaimWithAdButton->setUpWithPrice(SkipAdsManager::getInstance()->canSkipAds() ? UIButtonType::PURPLE : UIButtonType::YELLOW, Price(PRICE_VIDEO, 0),
                                            SkipAdsManager::getInstance()->getButtonStringBuilder(), width * .44f, Size(428, 192), false, .75f);
    activeClaimWithAdButton->setPosition(width * -.22f, baseRect.getMinY() + activeClaimWithAdButton->getBoundingBox().size.height * .65f);
    activeClaimWithAdButton->button->setPopupButtonPriority();
    
    inactiveClaimWithAdButton = UIButton::create();
    inactiveClaimWithAdButton->setUpWithPrice(UIButtonType::GRAY, Price(PRICE_VIDEO, 0),
                                              SkipAdsManager::getInstance()->getButtonStringBuilder(), width * .44f, Size(428, 192), false, .75f);
    inactiveClaimWithAdButton->setPosition(activeClaimWithAdButton->getPosition());
    inactiveClaimWithAdButton->button->setPopupButtonPriority();
    
    // claim with energy buttons
    
    activeClaimWithEnergyButton = UIButton::create();
    activeClaimWithEnergyButton->setUpWithPrice(UIButtonType::GREEN, Price(PRICE_GAS, IdleEarningManager::getInstance()->quickEarningEnergyCost), FormattedStringBuilder("").ignoreLocalization(), width * .44f, Size(428, 192), false, .75f);
    activeClaimWithEnergyButton->setPosition(-inactiveClaimWithAdButton->getPositionX(), inactiveClaimWithAdButton->getPositionY());
    activeClaimWithEnergyButton->button->setPopupButtonPriority();
    
    inactiveClaimWithEnergyButton = UIButton::create();
    inactiveClaimWithEnergyButton->setUpWithPrice(UIButtonType::GRAY, Price(PRICE_GAS, IdleEarningManager::getInstance()->quickEarningEnergyCost), FormattedStringBuilder("").ignoreLocalization(), width * .44f, Size(428, 192), false, .75f);
    inactiveClaimWithEnergyButton->setPosition(-inactiveClaimWithAdButton->getPositionX(), inactiveClaimWithAdButton->getPositionY());
    inactiveClaimWithEnergyButton->button->setPopupButtonPriority();
    
    // rewards grid
    
    auto rewardGridNode = Node::create();
    rewardGridNode->setPositionY(divider->getPositionY());
    
    auto bgSize = baseRect.size * .85f;
    
    int itemCountPerRow = 5;
    float bgMargin = width * .025f;
    float itemWidth = (bgSize.width - (bgMargin * (itemCountPerRow + 1))) / (float) itemCountPerRow;
    
    GridData rewardsGridData;
    rewardsGridData.width = bgSize.width;
    rewardsGridData.itemCount = (int)rewards.size();
    rewardsGridData.rowNodeCount = itemCountPerRow;
    rewardsGridData.padding_horizontal = bgMargin;
    rewardsGridData.padding_vertical = bgMargin;
    rewardsGridData.topMargin = bgMargin * 1.05f;
    rewardsGridData.itemWidth = itemWidth;
    
    Grid<RewardNode> rewardsGrid;
    rewardsGrid.setUp(rewardGridNode, rewardsGridData, 0, [=](RewardNode* node, int index){
        RewardNodeData rewardNodeData;
        
        rewardNodeData.rewardData = rewards[index];
        rewardNodeData.width = itemWidth;
        rewardNodeData.state = RewardState::READY_TO_CLAIM;
            
        node->setUp(rewardNodeData);
        node->button->setPriority(PopupLayer::current()->currentPopupPriority - 2);
    });
    
    //

    addChild(rewardInfoLabel);
    addChild(rewardGridNode);
    
    addChild(activeClaimWithAdButton);
    addChild(inactiveClaimWithAdButton);
    
    addChild(activeClaimWithEnergyButton);
    addChild(inactiveClaimWithEnergyButton);
    
    //
    
    auto rewardCollect = [=]()
    {
        collectAndHandleRewards(rewards, "quick_earnings", [](){
            PlayerLevelManager::getInstance()->checkLevelUp([=](){});
        });
    };
    
    //
    
    activeClaimWithAdButton->button->onTap([=](){

        AdManager::getInstance()->onRewardedShowAttempt("freeGemsButton");
        
        if(!AdManager::getInstance()->canShowRewarded())
        {
            AdManager::getInstance()->onRewardedNotReady("quick_earnings");
            
            AudioManager::getInstance()->playEffect("locked.wav");
            GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not ready yet!"), FONTS::USE_LOCALIZATION);
            return;
        }
        
        PopupLayer::current()->getCurrentPopup()->canBeClosedByOutsideTap = false;
        
        HapticFeedbackManager::getInstance()->play(HEAVY);
        
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        AdManager::getInstance()->showRewarded("quick_earnings", "RewardVideo.WarfrontPatrol", [=](bool completed){
            
            PopupLayer::current()->getCurrentPopup()->canBeClosedByOutsideTap = true;
            
            if(!completed)
            {
                AudioManager::getInstance()->playEffect("locked.wav");
                GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not completed!"), FONTS::USE_LOCALIZATION);
                return;
            }
            
            if(!SkipAdsManager::getInstance()->canSkipAds())
            {
                activeClaimWithAdButton->updateButtonPath();
            }
            
            rewardCollect();
            
            IdleEarningManager::getInstance()->purchaseWithVideo();

            updateClaimButtons();
        });
    });
    
    inactiveClaimWithAdButton->button->onTap([=](){

        HapticFeedbackManager::getInstance()->play(HEAVY);
        AudioManager::getInstance()->playEffect("locked.wav", .5f);
    
        int nextRefillTime = IdleEarningManager::getInstance()->getVideoNextRefillTime();
        bool hideHours = nextRefillTime < 60 * 60;
        FormattedStringBuilder nextRefillTimeFormat = secondsToTimeString(nextRefillTime, hideHours);
        GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Refreshes in %s", nextRefillTimeFormat.format().c_str()), FONTS::USE_LOCALIZATION);
    });
    
    //
    
    activeClaimWithEnergyButton->button->onTap([=](){
        
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        if(EnergyManager::getInstance()->isOutOfEnergy(IdleEarningManager::getInstance()->quickEarningEnergyCost))
        {
            PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp());
        }
        else
        {
            rewardCollect();
            
            EnergyManager::getInstance()->spendEnergy("instantEarning", IdleEarningManager::getInstance()->quickEarningEnergyCost);
            
            IdleEarningManager::getInstance()->purchaseWithEnergy();
            
            updateClaimButtons();
        }
        
    });
    
    inactiveClaimWithEnergyButton->button->onTap([=](){

        HapticFeedbackManager::getInstance()->play(HEAVY);
        AudioManager::getInstance()->playEffect("locked.wav", .5f);
    
        int nextRefillTime = IdleEarningManager::getInstance()->getEnergyNextRefillTime();
        bool hideHours = nextRefillTime < 60 * 60;
        FormattedStringBuilder nextRefillTimeStr = secondsToTimeString(nextRefillTime, hideHours);
        GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Refreshes in %s", nextRefillTimeStr.format().c_str()), FONTS::USE_LOCALIZATION);
    });
    
    updateClaimButtons();
    
    return this;
}

void QuickEarningPopup::updateClaimButtons()
{
    bool claimWithEnergyButtonVisible = IdleEarningManager::getInstance()->getNumberOfEnergyPurchasesLeft() > 0;
    
    bool claimWithAdButtonVisible = IdleEarningManager::getInstance()->getNumberOfVideoPurchasesLeft() > 0;
    activeClaimWithAdButton->setVisible(claimWithAdButtonVisible);
    inactiveClaimWithAdButton->setVisible(!claimWithAdButtonVisible);

    activeClaimWithEnergyButton->setVisible(claimWithEnergyButtonVisible);
    inactiveClaimWithEnergyButton->setVisible(!claimWithEnergyButtonVisible);
}

//

static IdleEarningPopup* currentIdleEarningPopup;
static IdleEarningButton* currentIdleEarningButton;


void IdleEarningButton::setup(float width)
{
    // MUST STAY, starts the timer at the very first time that button created
    IdleEarningManager::getInstance()->getPassedTime();
    
    currentIdleEarningButton = this;
    
    button = ButtonImage::create()->init("ui/mainMenu/Icon_IdleEarnings_MainMenu.png");
    button->fitW(width);
    button->setSceneButtonPriority();
    addChild(button);
    
    badge = NotificationBadge::create();
    badge->setUp(width * .5f);
    badge->setPosition(width * .4f, width * .4f);
    button->mNode->addChild(badge);
    
    button->onTap([=](){
        PopupLayer::current()->showPopup(IDLE_EARNING_POPUP, IdleEarningPopup::create()->setUp());
    });
    
    if(GameManager::getInstance()->SHOW_MISSION_COMPLETED_ANIMATION_ON_MENU ||
       GameManager::getInstance()->SHOW_LAST_LOOT_ANIMATION_ON_MENU)
    {
        hide();
    }
    
    updateBadge();
    
    if(IdleEarningManager::getInstance()->isLocked())
    {
        setCascadeGrayShader(button, "menuButton", 1.f);
    }
}

void IdleEarningButton::updateBadge()
{
    int notificationKey = 0;
    if(IdleEarningManager::getInstance()->getRemainTimeToActivation() <= 0)
    {
        notificationKey = -1099;
    }
    
    badge->updateValue(notificationKey);
}

void IdleEarningButton::hide()
{
    setVisible(false);
}

void IdleEarningButton::show()
{
    setVisible(true);
    
    setScale(0.f);
    
    runAction(EaseSineOut::create(ScaleTo::create(.2f, 1.f)));
}

//

IdleEarningPopup* IdleEarningPopup::setUp()
{
    const float width = GET_WIDTH * .9f * getSafeSceneScale();
    
    rewards = IdleEarningManager::getInstance()->getStandardEarningRewards();
    
    //
    
    PopupNode::setUp(width, Size(968, 1494),  "", FormattedStringBuilder("Warfront Patrol"));
    
    Sprite* popupBottomBG = Sprite::create("ui/common/container/Container_Popup_02_WithTimeContainer.png");
    popupBottomBG->setContentSize(Size(912, 1092));
    popupBottomBG->setCenterRectNormalized(Rect(.25f, .7f, .5f, .05f));
    FIT_W(popupBottomBG, baseRect.size.width * .936f);
    popupBottomBG->setPositionY(baseRect.size.height * .02f);
    addChild(popupBottomBG);
    
    Sprite* scrollBg = Sprite::create("ui/common/container/Container_PopupScroll.png");
    scrollBg->setContentSize(Size(912, 576));
    scrollBg->setCenterRectNormalized(Rect(.42f, .25f, .16f, .5f));
    FIT_W(scrollBg, baseRect.size.width * .936f);
    scrollBg->setPositionY(popupBottomBG->getBoundingBox().getMinY() + scrollBg->getBoundingBox().size.height * .5f);
    addChild(scrollBg);
    
    
    // title node

    float fontSize =  width * .07f;

    baseCloseButton->onTap([=](){
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        PopupLayer::current()->destroyPopup();
    });
    
    Sprite* patrolIcon = Sprite::create("ui/Icon_IdleEarnings_MainMenu.png");
    FIT_W(patrolIcon, width * .2f);
    patrolIcon->setPosition(Vec2(width * -.39f, width * -.03f));
    baseHeaderParent->addChild(patrolIcon);
    
    //
    
    TimerNodeConfig config;
    config.fontSize = width * .045f;
    config.startTime = DailyMissionManager::getInstance()->getRemainingTime();
    config.timerLabelStrBuilder = FormattedStringBuilder("Guard Time: ");
    
    TimerNode* timerNode = TimerNode::create()->setup(config);
    timerNode->setPositionY(popupBottomBG->getBoundingBox().getMaxY() - width * .05f);
    addChild(timerNode);
    
    FormattedStringBuilder passedTimeStr = secondsToTimeString(IdleEarningManager::getInstance()->getPassedTime());
    timerNode->updateValueString(passedTimeStr);
    timerNode->centerNodes();
    
    HBLabel* topInfoLabel = HBLabel::create("", FONTS::USE_LOCALIZATION, fontSize * .6f);
    topInfoLabel->setString("Clear more missions for better rewards!");
    topInfoLabel->limitSize(Vec2(baseRect.size.width * .9f, 0));
    topInfoLabel->setTextColor(Color4B(208, 211, 221, 255));
    topInfoLabel->setPositionY(popupBottomBG->getBoundingBox().getMaxY() - baseRect.size.width * .13f);
    
    //
    
    Sprite* earningsTab = Sprite::create("ui/common/container/Container_PopupTab.png");
    earningsTab->setContentSize(Size(358, 72));
    earningsTab->setCenterRectNormalized(Rect(.32f, .0f, .36f, 1.f));
    FIT_W(earningsTab, scrollBg->getBoundingBox().size.width * .393f);
    earningsTab->setPositionY(scrollBg->getBoundingBox().getMaxY() + earningsTab->getBoundingBox().size.height * .5f);
    addChild(earningsTab);
    
    HBLabel* rewardsTitleLabel = HBLabel::create("Earnings", FONTS::USE_LOCALIZATION, config.fontSize);
    rewardsTitleLabel->setTextColor(Color4B(191, 200, 255, 255));
    rewardsTitleLabel->setPositionY(earningsTab->getPositionY());
    
    Sprite* infoBg = Sprite::create("ui/common/container/Container_Popup_Fill.png");
    infoBg->setCenterRectNormalized(Rect(.25, .25, .5f, .5f));
    infoBg->setContentSize(Size(832, 158));
    FIT_W(infoBg, width * .83f);
    infoBg->setPositionY(earningsTab->getBoundingBox().getMaxY() + infoBg->getBoundingBox().size.height * .7f);

    const auto iconWidth = fontSize * 1.2f;
    
    std::map<char, Color3B> colorBetweenMap;
    colorBetweenMap.insert({ '|', Color3B(155, 244, 160) });
    
    //
    Sprite* experienceInfoIcon = Sprite::create("ui/currencyBar/Icon_LevelBadge_MainMenu.png");
    FIT_W(experienceInfoIcon, iconWidth);
    experienceInfoIcon->setPosition(width * .05f + (experienceInfoIcon->getBoundingBox().size.width * .5f), infoBg->getPositionY());

    HBLabel* experienceIconLabel = HBLabel::create(FormattedStringBuilder("xp").ignoreLocalization(), FONTS::USE_LOCALIZATION, fontSize * .45f);
    experienceIconLabel->setPosition(experienceInfoIcon->getPosition());
    experienceIconLabel->setAnchorPoint(Vec2(.5f, .2f));
    
    //
    
    auto hourlyIncomeStr = FormattedStringBuilder("|%d|/h", IdleEarningManager::getInstance()->getRewardAmountWithType(60 * 60, REWARD_GOLD)).ignoreLocalization();
    HBLabel* hourlyGoldIncomeLabel = HBLabel::create(hourlyIncomeStr, FONTS::USE_LOCALIZATION, fontSize * .7f);
    hourlyGoldIncomeLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    hourlyGoldIncomeLabel->colorWordsBetweenSymbols(colorBetweenMap, Color3B::WHITE);
    hourlyGoldIncomeLabel->setPosition(width * -.05f, infoBg->getPositionY());
    
    auto hourlyXpIncomeStr = FormattedStringBuilder("|%d|/h", IdleEarningManager::getInstance()->getRewardAmountWithType(60 * 60, REWARD_XP)).ignoreLocalization();
    HBLabel* hourlyExperienceLabel = HBLabel::create(hourlyXpIncomeStr, FONTS::USE_LOCALIZATION, fontSize * .7f);
    hourlyExperienceLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    hourlyExperienceLabel->colorWordsBetweenSymbols(colorBetweenMap, Color3B::WHITE);
    hourlyExperienceLabel->setPosition(experienceInfoIcon->getPositionX() + experienceInfoIcon->getBoundingBox().size.width * 1.0f, experienceInfoIcon->getPositionY());
    
    //
    
    Sprite* goldInfoIcon = Sprite::create("ui/currencyBar/Icon_Currency_Gold.png");
    FIT_W(goldInfoIcon, iconWidth);
    goldInfoIcon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    goldInfoIcon->setPosition(hourlyGoldIncomeLabel->getPositionX() - hourlyGoldIncomeLabel->getBoundingBox().size.width * 1.1, hourlyGoldIncomeLabel->getPositionY());
    
    //
    
    auto maxAccumulationHoursStr = FormattedStringBuilder("Rewards increase up to %d hours", IdleEarningManager::getInstance()->maxAccumulationHours);
    HBLabel* maxRewardHoursInfoLabel = HBLabel::create(maxAccumulationHoursStr, FONTS::USE_LOCALIZATION, fontSize * .6f);
    maxRewardHoursInfoLabel->setTextColor(Color4B(208, 211, 221, 255));
    maxRewardHoursInfoLabel->limitSize(Vec2(baseRect.size.width * .9f, 0));
    maxRewardHoursInfoLabel->setPositionY(topInfoLabel->getBoundingBox().getMinY() - topInfoLabel->getBoundingBox().size.height);
    
    UIButton* quickEarningButton = UIButton::create();
    quickEarningButton->setUp(UIButtonType::YELLOW, FormattedStringBuilder("Quick Earnings"), width * .44f, Size(428, 192));
    quickEarningButton->setPosition(-width * .22f, baseRect.getMinY() + quickEarningButton->getBoundingBox().size.height * .65f);
    quickEarningButton->button->setPopupButtonPriority();
    quickEarningButton->button->onTap([=](){
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        PopupLayer::current()->showPopup(QUICK_EARNING_POPUP, QuickEarningPopup::create()->setUp());
    });

    
    activeClaimButton = UIButton::create();
    activeClaimButton->setUp(UIButtonType::GREEN, FormattedStringBuilder("Claim"), width * .44f, Size(428, 192), .75f);
    activeClaimButton->setPosition(width * .22f, baseRect.getMinY() + activeClaimButton->getBoundingBox().size.height * .65f);
    activeClaimButton->button->setPopupButtonPriority();
    activeClaimButton->button->onTap([=](){
       
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        collectAndHandleRewards(rewards, "idle_earning", [](){
            PlayerLevelManager::getInstance()->checkLevelUp([=](){});
        });
        
        //
        
        IdleEarningManager::getInstance()->resetPassedTime();
        updateClaimButtons();
        updateContent();

    });
    
    
    inactiveClaimButton = UIButton::create();
    inactiveClaimButton->setUp(UIButtonType::GRAY, FormattedStringBuilder("Claim"), width * .44f, Size(428, 192), .75f);
    inactiveClaimButton->setPosition(width * .22f, baseRect.getMinY() + inactiveClaimButton->getBoundingBox().size.height * .65f);
    inactiveClaimButton->button->setPopupButtonPriority();
    inactiveClaimButton->button->onTap([=](){
       
        HapticFeedbackManager::getInstance()->play(HEAVY);
        AudioManager::getInstance()->playEffect("locked.wav", .5f);
        
        FormattedStringBuilder remainTimeToActivationStr = secondsToTimeString(IdleEarningManager::getInstance()->getRemainTimeToActivation(), true);
        GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Will be available in: %s", remainTimeToActivationStr.format().c_str()), FONTS::USE_LOCALIZATION);

    });
    
    // rewards grid
    
    rewardGridNode = Node::create();
    rewardGridNode->setPositionY(scrollBg->getBoundingBox().getMaxY());
    
    auto bgSize = scrollBg->getBoundingBox().size * .9f;
    
    int itemCountPerRow = 5;
    float bgMargin = width * .015f;
    itemWidth = (bgSize.width - (bgMargin * (itemCountPerRow + 1))) / (float) itemCountPerRow;
    
    rewardsGridData.width = bgSize.width;
    rewardsGridData.itemCount = (int)rewards.size();
    rewardsGridData.rowNodeCount = itemCountPerRow;
    rewardsGridData.padding_horizontal = bgMargin;
    rewardsGridData.padding_vertical = bgMargin;
    rewardsGridData.topMargin = bgMargin * 1.05f;
    rewardsGridData.itemWidth = itemWidth;
    updateContent();
    
    //

    addChild(rewardsTitleLabel);
    addChild(rewardGridNode);
    
    addChild(infoBg);
    addChild(goldInfoIcon);
    addChild(hourlyGoldIncomeLabel);
    
    addChild(experienceInfoIcon);
    addChild(experienceIconLabel);
    addChild(hourlyExperienceLabel);
    
    addChild(maxRewardHoursInfoLabel);
    addChild(quickEarningButton);
    addChild(activeClaimButton);
    addChild(inactiveClaimButton);
    
    addChild(topInfoLabel);
    
    // actions
    
    runAction(RepeatForever::create(Sequence::create({
       
        CallFunc::create([=](){
        
        FormattedStringBuilder passedTimeStr = secondsToTimeString(IdleEarningManager::getInstance()->getPassedTime());
        timerNode->updateValueString(passedTimeStr);
            
            int passedTime = IdleEarningManager::getInstance()->getPassedTime();
            if(passedTime % 5 == 0)
            {
                updateContent();
                updateClaimButtons();
            }
        }),
        
        DelayTime::create(1.f),
        
    })));
    
    updateClaimButtons();
    
    return this;
}

void IdleEarningPopup::updateContent()
{
    rewards = IdleEarningManager::getInstance()->getStandardEarningRewards();
    
    rewardsGrid.removeNodes();
    rewardsGridData.itemCount = (int)rewards.size();
    
    rewardsGrid.setUp(rewardGridNode, rewardsGridData, 0, [=](RewardNode* node, int index){
        RewardNodeData rewardNodeData;
        
        rewardNodeData.rewardData = rewards[index];
        rewardNodeData.width = itemWidth;
        rewardNodeData.state = RewardState::READY_TO_CLAIM;
            
        node->setUp(rewardNodeData);
        node->button->setPopupButtonPriority();
    });
}

void IdleEarningPopup::updateClaimButtons()
{
    int remainTimeToActivation = IdleEarningManager::getInstance()->getRemainTimeToActivation();
    
    if(remainTimeToActivation > 0)
    {
        inactiveClaimButton->setVisible(true);
        activeClaimButton->setVisible(false);
    }
    else if(remainTimeToActivation <= 0)
    {
        inactiveClaimButton->setVisible(false);
        activeClaimButton->setVisible(true);
    }
    
    if(MenuNode::current())
    {
        MenuNode::current()->idleEarningButton->updateBadge();
    }
}
