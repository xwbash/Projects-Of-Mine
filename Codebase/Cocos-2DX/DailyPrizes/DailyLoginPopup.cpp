#include "DailyLoginPopup.h"
#include "../DailyLoginManager.h"
#include "../UI/ToolTipNode.h"

#include "../Menu/MenuNode.h"
#include "../Menu/PopupLayer.h"
#include "../UI/ButtonImage.h"
#include "../UI/HBLabel.h"
#include "../UI/RewardNode.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"
#include "../GameManager.h"
#include "../StoreManager.h"
#include "../UI/AchievementLoadingBar/AchievementLoadingBarNode.hpp"
#include "../AdManager.h"
#include "../BundleManager.h"
#include "../RemoteRewardDataManager.h"
#include "../PlayerLevelManager.h"
#include "../UI/TimerNode.hpp"

#define GET_DAY_PERIOD DailyLoginManager::getInstance()->getDayPeriod()

static DailyLoginPopup* currentDailyLoginPopup;
static DailyLoginButton* currentDailyLoginButton;


void DailyLoginButton::setup(float width)
{
    currentDailyLoginButton = this;
    
    button = ButtonImage::create()->init("ui/mainMenu/Icon_DailyRewards_MainMenu.png");
    button->fitW(width);
    button->setSceneButtonPriority();
    addChild(button);
    
    badge = NotificationBadge::create();
    badge->setUp(width * .5f);
    badge->setPosition(width * .4f, width * .4f);
    button->mNode->addChild(badge);
    
    button->onTap([=](){
        if(!DailyLoginManager::getInstance()->isAvailable())
        {
            AudioManager::getInstance()->playEffect("locked.wav");
            ToolTipNode::create(button, FormattedStringBuilder("this feature is locked"));
        }
        else
        {
            auto popup = DailyLoginPopup::create()->setUp(false);
            PopupLayer::current()->showPopup(DAILY_LOGIN_POPUP, popup);
            popup->priorityValue = PopupLayer::current()->currentPopupPriority;
            popup->updateContent(false);
        }
    });
    
    if(ADMIN_PLAYER)
    {
        button->onHold([=](){
            
            DailyLoginManager::getInstance()->setTime(1);
            
            if(!AdManager::getInstance()->canShowInterstitial())
            {
                GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("INTERSTITIAL NOT READY"), FONTS::USE_LOCALIZATION);
                return;
            }
            
            AdManager::getInstance()->showInterstitial("admin_test", [=](){ });
        });
    }
    
    if(GameManager::getInstance()->SHOW_MISSION_COMPLETED_ANIMATION_ON_MENU ||
       GameManager::getInstance()->SHOW_LAST_LOOT_ANIMATION_ON_MENU)
    {
        hide();
    }
    
    if(DailyLoginManager::getInstance()->isAvailable())
    {
        updateBadge();
    }
    
    if(DailyLoginManager::getInstance()->isLocked() || !DailyLoginManager::getInstance()->isAvailable())
    {
        setCascadeGrayShader(button, "dailyLoginButton", 1.f);
    }
}

void DailyLoginButton::updateButton()
{
    if(!DailyLoginManager::getInstance()->isAvailable())
    {
        setCascadeGrayShader(button, "dailyLoginButton", 1.f);
    }
    else
    {
        setCascadeGrayShader(button, "dailyLoginButton", 0.f);
    }
}

void DailyLoginButton::updateBadge()
{
    badge->updateValue(DailyLoginManager::getInstance()->getNotificationCount());
   
    if(MenuNode::current())
    {
        MenuNode::current()->dailyMainButton->updateButton();
    }
}

void DailyLoginButton::hide()
{
    setVisible(false);
}

void DailyLoginButton::show()
{
    setVisible(true);
    
    setScale(0.f);
    
    runAction(EaseSineOut::create(ScaleTo::create(.2f, 1.f)));
}


//

void DailyLoginDayNode::setUp(float width, int dayIndex, bool isAutoShow, bool isWideBackground, std::function<void()> refreshCallback, Node* timerNode)
{
    this->width = width;
    this->dayIndex = dayIndex;
    this->isWide = isWideBackground;
    this->isAutoShow = isAutoShow;
    this->refreshCallback = refreshCallback;
    this->timerNode = timerNode;
    
    //
    
    button = ButtonImage::create()->init("ui/common/container/Container_Reward_Collected.png");
    button->mSprite->setContentSize(isWide ? Size(824, 320) : Size(250, 320));
    button->mSprite->setCenterRectNormalized(Rect(.3f, .6f, .4f, .16f));
    button->fitW(width);
    button->setScrollButtonPriority(true);
    addChild(button);
    
    labelTitleContainer = Node::create();
    button->mNode->addChild(labelTitleContainer);
    
    //

    
    
    updateContent();
    
}

void DailyLoginDayNode::updateContent()
{
    const bool isCollected = dayIndex < DailyLoginManager::getInstance()->getCurrentDay();
    const bool isCurrentDay = (DailyLoginManager::getInstance()->getCurrentDay() == dayIndex);
    const bool isReady = isCurrentDay && (DailyLoginManager::getInstance()->getTimeLeftForNextReward() < 0);
    
    button->stopAnimateScale();
    
    if(isCollected)
    {
        button->mSprite->setTexture("ui/common/container/Container_Reward_Collected.png");
    }
    else if(isCurrentDay)
    {
        button->mSprite->setTexture("ui/common/container/Container_Reward_Collectable.png");
        
        if(isReady)
        {
            button->animateScale();
        }
    }
    else
    {
        button->mSprite->setTexture("ui/common/container/Container_Reward_Default.png");
    }
    
    button->mSprite->setContentSize(isWide ? Size(824, 320) : Size(250, 320));
    button->mSprite->setCenterRectNormalized(Rect(.3f, .6f, .4f, .16f));
    
    button->fitW(width);
    
    //
    
    if(!grid.gridItems.empty())
    {
        grid.removeNodes();
        grid.clear();
    }
    
    auto rewards = DailyLoginManager::getInstance()->rewards[dayIndex];
    
    GridData gridData;
    gridData.itemWidth = isWide ? (width * .75f / (rewards.size() + 1)) :  width * .65f;
    gridData.rowNodeCount = (int)rewards.size();
    gridData.itemCount = gridData.rowNodeCount;
    gridData.padding_horizontal = gridData.itemWidth * .1f;
    
    float gridHeight = grid.setUp(button->mNode, gridData, gridData.itemWidth * .25f, [=](RewardNode* node, int index){
        RewardNodeData rewardNodeData;
        
        rewardNodeData.rewardData = rewards[index];
        rewardNodeData.width = gridData.itemWidth;
        rewardNodeData.state = RewardState::READY_TO_CLAIM;
        
        node->setUp(rewardNodeData);
        
        node->button->setPopupButtonPriority();
    });
    
    
    //
    
    auto positionOffset = button->mSprite->getBoundingBox().size.height * .17f;
    labelTitleContainer->setPosition(0, button->mSprite->getBoundingBox().getMaxY() - positionOffset);
    
    if(labelTitle)
    {
        labelTitle->removeFromParent();
        labelTitle = nullptr;
    }
    
    if(isCurrentDay)
    {
        if(!isReady)
        {
            auto fontSize = isWide ? width * .07f : width * .175f;
            labelTitle = HBLabel::create(FormattedStringBuilder("Day %d", DailyLoginManager::getInstance()->getDayByIndex(dayIndex)), FONTS::USE_LOCALIZATION, fontSize);
            labelTitle->setTextColor(Color4B(198, 208, 255, 255));
            
            labelTitleContainer->addChild(labelTitle);
            
            auto updateLabel = [=](){
            
                const int time = DailyLoginManager::getInstance()->getTimeLeftForNextReward();
                
                if(time < 0)
                {
//                    currentDailyLoginPopup->setUp(true);
                    refreshCallback();
                    return;
                }
                
//                labelTitle->setString(getRemainingTimeString(time));
            };
            
            updateLabel();
            
            labelTitle->runAction(RepeatForever::create(Sequence::create({
                
                DelayTime::create(1.f),
                CallFunc::create([=](){
                    
                    updateLabel();
                })
                
            })));
        }
    }
    
    if(!labelTitle)
    {
        auto fontSize = isWide ? width * .07f : width * .175f;
        labelTitle = HBLabel::create(FormattedStringBuilder("Day %i", DailyLoginManager::getInstance()->getDayByIndex(dayIndex)), FONTS::USE_LOCALIZATION, fontSize);
        labelTitleContainer->addChild(labelTitle);
    }
    
    if(isCollected)
    {
        labelTitle->setTextColor(Color4B(255, 255, 228, 255));
    }
    
    if(tick)
    {
        tick->removeFromParent();
        tick = nullptr;
    }
  
    
    if(isCollected)
    {
        button->mSprite->setOpacity(255/2);
        labelTitle->setOpacity(255/2);
        
        for(int i = 0; i < grid.gridItems.size(); i++)
        {
            grid.gridItems[i]->fade(255/2);
            grid.gridItems[i]->toggleLightedSprite(false);
        }
        
        
        tick = Sprite::create("ui/common/icon/Icon_Check.png");
        
        if(isWide)
        {
            tick->setPosition(button->mSprite->getBoundingBox().getMaxX() * .9f,
                              button->mSprite->getBoundingBox().getMaxY() * .95f);
        }
        else
        {
            tick->setPosition(button->mSprite->getBoundingBox().getMaxX() * .85f,
                              button->mSprite->getBoundingBox().getMaxY() * .9f);
        }
        
        
        FIT_W(tick, width * (isWide ? .11f : .4f));
        button->mNode->addChild(tick);
        
        button->onTap([=](){
           
            AudioManager::getInstance()->playEffect("locked.wav");
            
            ToolTipNode::create(this, FormattedStringBuilder("Already collected"));
            
            /*tick->stopAllActions();
            tick->setVisible(true);
            tick->runAction(Blink::create(1.f, 3));*/
            
        });
    }
    else
    {
        button->mSprite->setOpacity(255);
        labelTitle->setOpacity(255);
        
        if(isCurrentDay && !isCollected)
        {
            labelTitle->setTextColor(Color4B(245, 245, 245, 255));
            labelTitle->enableHBStyle(1, 0 , Color4B(165, 93, 9, 255));
        }
        else
        {
            labelTitle->setTextColor(Color4B(198, 208, 255, 255));
        }
        
        for(int i = 0; i < grid.gridItems.size(); i++)
        {
            grid.gridItems[i]->fade(255);
        }
        
        button->onTap([=](){
           

            if(isCurrentDay && !isReady)
            {
                AudioManager::getInstance()->playEffect("locked.wav");
                
                labelTitleContainer->stopAllActions();
                labelTitleContainer->setVisible(true);
                labelTitleContainer->runAction(Blink::create(1.f, 3));
                
                int currentDay = DailyLoginManager::getInstance()->getCurrentDay();
                
                const double seconds = (dayIndex - currentDay + 1) * GET_DAY_PERIOD;
                const double timeLeftForNextReward = GET_DAY_PERIOD - DailyLoginManager::getInstance()->getTimeLeftForNextReward();
                
                auto totalSecond = seconds - timeLeftForNextReward;
                
                if(totalSecond < 0)
                {
                    totalSecond = (dayIndex - currentDay) * GET_DAY_PERIOD;
                }
                
                ToolTipNode::create(this, FormattedStringBuilder("Reward will be available in %s", getRemainingTimeString(totalSecond).format().c_str()));
                
                auto scaleInOut = Sequence::create(EaseSineOut::create(ScaleTo::create(.2f, 1.15f)),EaseSineOut::create(ScaleTo::create(.2f, 1)), NULL);
                
                if(timerNode)
                {
                    for(int i = 0; i < timerNode->getChildren().size(); i++)
                    {
                        auto children = timerNode->getChildren().at(i);
                        children->runAction(scaleInOut);
                    }
                }
                
                   
            }
            else
            {
                const auto collectReward = [=](bool isDouble){
                    
                    
                    if(!isCurrentDay)
                    {
                        int currentDay = DailyLoginManager::getInstance()->getCurrentDay();
                        
                        const double seconds = (dayIndex - currentDay + 1) * GET_DAY_PERIOD;
                        const double timeLeftForNextReward = GET_DAY_PERIOD - DailyLoginManager::getInstance()->getTimeLeftForNextReward();
                        
                        auto totalSecond = seconds - timeLeftForNextReward;

                        if(totalSecond < 0)
                        {
                            totalSecond = (dayIndex - currentDay) * GET_DAY_PERIOD;
                        }

                        ToolTipNode::create(this, FormattedStringBuilder("Reward will be available in %s", getRemainingTimeString(totalSecond).format().c_str()));

                        return;
                    }
                    
                    if(!isActive)
                    {
                        return;
                    }
                    
                    ButtonImage::disableAllTouches();
                    PopupLayer::current()->getCurrentPopup()->canBeClosedByOutsideTap = false;
                    
                    isActive = false;
                    HapticFeedbackManager::getInstance()->play(HEAVY);
                    AudioManager::getInstance()->playEffect("collect.wav");
                    
                    auto rewardData = DailyLoginManager::getInstance()->rewards[DailyLoginManager::getInstance()->getCurrentDay()];
                    
                    const auto pos = this->convertToWorldSpace(Vec2::ZERO);
                    
                    DailyLoginManager::getInstance()->collectReward([=](){
                        
                        ButtonImage::enableAllTouches();

                        PlayerLevelManager::getInstance()->checkLevelUp([=](){ });
                        
                        refreshCallback();
                        
                    }, this->convertToWorldSpace(Vec2::ZERO)); // COLLECT REWARD
                    
                    currentDailyLoginButton->updateBadge();
                    
                    if(MenuNode::current()->navigationBar) MenuNode::current()->navigationBar->updateNotificationBadges();
                    
                    DailyLoginManager::getInstance()->increaseTotalDay();

                    if(dayIndex >= 6)
                    {
                        DailyLoginManager::getInstance()->increaseAnWeek();
                    }
                };
                
                collectReward(false);
            }
            
        });
    }
}

//
//
//

DailyLoginPopup* DailyLoginPopup::setUp(bool isAutoShow)
{
    currentDailyLoginPopup = this;
    
    width = GET_WIDTH * .9f * getSafeSceneScale();
    
    PopupNode::setUp(width, Size(968, 1607), "", FormattedStringBuilder("Daily Prizes"));

    //
   
    fontSize =  width * .075f;

    baseCloseButton->onTap([=](){
        
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        PopupLayer::current()->destroyPopup();
        
        if(isAutoShow)
        {
            BundleManager::getInstance()->checkShowBundlePopup();
        }
        
    });
    
    //
    
    
    auto dailyLoginBackground = Sprite::create("ui/common/container/Container_Popup_02_WithTimeContainer.png");
    dailyLoginBackground->setContentSize(Size(912, 1385));
    dailyLoginBackground->setCenterRectNormalized(Rect(.25f, .7f, .5f, .05f));
    FIT_W(dailyLoginBackground, baseRect.size.width * .936f);
    dailyLoginBackground->setPositionY(baseRect.size.height * -.04f);
    addChild(dailyLoginBackground);
    
    auto bottomBg = Sprite::create("ui/common/container/Container_PopupScroll.png");
    bottomBg->setContentSize(Size(912, 1099));
    bottomBg->setCenterRectNormalized(Rect(.42f, .25f, .16f, .5f));
    FIT_W(bottomBg, baseRect.size.width * .936f);
    bottomBg->setPositionY(dailyLoginBackground->getBoundingBox().getMinY() + bottomBg->getBoundingBox().size.height * .5f);
    addChild(bottomBg);
    
    //
    
    TimerNodeConfig config;
    config.fontSize = width * .045f;
    config.startTime = std::max(0, DailyLoginManager::getInstance()->getTimeLeftForNextReward());
    config.timerLabelStrBuilder = FormattedStringBuilder("Ready in: ");
    
    TimerNode* timerNode = TimerNode::create()->setup(config);
    timerNode->setPositionY(dailyLoginBackground->getBoundingBox().getMaxY() - width * .05f);
    addChild(timerNode);
    
    //
    
    dayNodes.clear();
    
    Node* dayNodeContainer = Node::create();
    addChild(dayNodeContainer);
    
    int dayIndex = 0;
    
    for(int y = 0; y < 2; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            auto dayNode = DailyLoginDayNode::create();
            dayNode->setUp(width * .275f, dayIndex, isAutoShow, false, [=](){
                updateContent(isAutoShow);
                //ButtonImage::enableAllTouches();
                PopupLayer::current()->getCurrentPopup()->canBeClosedByOutsideTap = true;
            }, timerNode);
            
            dayNode->setPosition((x - 1.f) * width * .285f,
                                 -y * width * .365f + width * .17f);
            dayNodeContainer->addChild(dayNode);
        
            dayNodes.push_back(dayNode);
            dayIndex++;
        }
    }
    
    auto dayNode = DailyLoginDayNode::create();
    dayNode->setUp(width * .825f, dayIndex++, isAutoShow, true, [=](){
        updateContent(isAutoShow);
    });
    dayNode->setPosition(dailyLoginBackground->getPositionX(), dayNodes[dayNodes.size()-1]->getPositionY() - width * .35f);
    dayNodeContainer->addChild(dayNode);

    dayNodes.push_back(dayNode);
    
    dayNodeContainer->setPositionY(width * -.045f);
    
    //
    
    updateFunc = [=](){
        
        const int seconds =  std::max(0, DailyLoginManager::getInstance()->getTimeLeftForNextReward());
        
        timerNode->updateValueString(getRemainingTimeString(seconds));
        
        timerNode->centerNodes();
    };
    
    updateFunc();
    
    timerNode->runAction(RepeatForever::create(Sequence::create({
           
        DelayTime::create(1.f),
        CallFunc::create([=](){
               
            updateFunc();
        })
        
    })));
    
    updateContent(isAutoShow);
    
    return this;
}

void DailyLoginPopup::updateContent(bool isAutoShow)
{
    if(achievementBar)
    {
        achievementBar->removeFromParent();
        achievementBar = nullptr;
    }
    
    auto achievementList = getAchievementData();
    
    achievementBar = AchievementLoadingBarNode::create();
    achievementBar->setup("dailyLogin", width * .7f, width * .06f, DailyLoginManager::getInstance()->getTotalDay(), achievementList[achievementList.size()-1].achievementValue,minValue, achievementList, DailyLoginManager::getInstance()->chestRewards, Calendar_Icon, true, false, [=](){
        
        DailyLoginManager::getInstance()->syncToRemote();
        
    });
    achievementBar->setPositionY(baseRect.size.height * .28f);
    addChild(achievementBar);
    
    achievementBar->customProgressRange(.07f, 1.f);
    
    if(priorityValue == 0)
    {
        priorityValue = PopupLayer::current()->currentPopupPriority;
    }
    
    for(auto btn : achievementBar->buttons)
    {
        btn->setPriority(priorityValue);
    }
    
    updateDayNodes();
}

void DailyLoginPopup::updateDayNodes()
{
    for(auto* dayNode : dayNodes)
    {
        dayNode->updateContent();
    }
}

std::vector<AchievementData> DailyLoginPopup::getAchievementData()
{
    std::vector<int> startValue = {8,15,22,28};
    std::vector<AchievementData> achievementDataList;

    int totalValue = (DailyLoginManager::getInstance()->getTotalDay()-1) / startValue[startValue.size()-1];
    
    auto maxValue = (startValue[startValue.size()-1] * (totalValue));
    
    maxValue = totalValue < 1 ? 0 : maxValue;
    
    minValue = totalValue * startValue[startValue.size()-1];
    
    for(int i = 0; i < 4; i++)
    {
        AchievementData achivementData;
        achivementData.achievementValue = startValue[i] + maxValue;
        achivementData.rewardAmount = 1;
        
        if(i < 3)
        {
            achivementData.achievementRewardType = Blue_Chest_Reward;
        }
        else
        {
            achivementData.achievementRewardType = Purple_Chest_Reward;
        }
        
        achievementDataList.push_back(achivementData);
    }
    
    return achievementDataList;
}

void DailyLoginPopup::onExit()
{
    currentDailyLoginButton->updateBadge();
    
    if(MenuNode::current())
    {
        MenuNode::current()->navigationBar->getButton(PLAY)->updateNotificationBadge();
    }
    
    currentDailyLoginPopup = nullptr;
    
    Node::onExit();
}
