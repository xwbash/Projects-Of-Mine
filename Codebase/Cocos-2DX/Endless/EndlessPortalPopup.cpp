//
//  EndlessPortalPopup.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#include "EndlessPortalPopup.hpp"
#include "../../HapticFeedbackManager.h"
#include "../../AudioManager.h"
#include "../../Menu/PopupLayer.h"
#include "../../UI/LabelWithIcon.h"
#include "../../GameManager.h"
#include "../../EnergyManager.h"
#include "../../Menu/MenuNode.h"
#include "../../UI/ToolTipNode.h"
#include "../../GameModeManager/EndlessStageGameManager.hpp"
#include "../../NicknameGeneratorManager.hpp"
#include "../../EnergyManager.h"
#include "../../Menu/PopupLayer.h"
#include "../../Popups/EnergyPopup.h"
#include "../../OfferPopupManager.hpp"
#include "../../DataEventManager.h"
#include "../../UI/TimerNode.hpp"
#include "../../LeaderboardManager.hpp"
#include "../../UI/BottomCloseNode.hpp"
#include "../../StorageManager.h"

#include "../../RemoteEndlessPortalManager.hpp"


EndlessPortalPopup* EndlessPortalPopup::setup(EndlessPortalConfig* endlessPortalConfig)
{
    isFullScreen = true;
    popupName = "EndlessPortalPopup";
    
    setUninterruptible();
    
    if(MenuNode::current() && MenuNode::current()->currencyBarPanel)
    {
        MenuNode::current()->currencyBarPanel->pushArrangement({});
    }
    
    Sprite* backgroundPopup = Sprite::create("ui/popup/endlessPortalPopup/Background_EndlessEvent.jpg");
    FIT_H(backgroundPopup, GET_HEIGHT);
    addChild(backgroundPopup);
    
    ButtonImage* exitIcon = ButtonImage::create()->init("ui/common/button/Button_Cancel.png");
    exitIcon->fitH(GET_WIDTH * getSafeSceneScale() * .11f);
    exitIcon->setPosition(GET_WIDTH * .415f, (GET_HEIGHT * .5f) - exitIcon->getBoundingBox().size.height - getSafeTopOffset());
    exitIcon->setPopupButtonPriority();
    exitIcon->onTap([=](){
        PopupLayer::current()->destroyPopup();
    });
    exitIcon->onPressFilter([=](){
        return endlessPortalRankPageNode == nullptr;
    });
    addChild(exitIcon);
    
    HBLabel* title = HBLabel::createWithDarkStyle("Endless Portal", FONTS::USE_LOCALIZATION, GET_WIDTH * .085f);
    title->setColor(Color3B(255, 216, 75));
    title->setPositionY(GET_HEIGHT * .4f);
    title->enableGradient(Color4B(245, 209, 67, 255), -45 , 90 , 2.f);
    addChild(title);
    
    // Timer
    
    remainingTime = (int)RemoteEndlessPortalManager::getInstance()->getRemainingTimeInSeconds();
    
    TimerNodeConfig timeConfig;
    timeConfig.fontSize = GET_WIDTH * .035f;
    timeConfig.startTime = remainingTime;
    timeConfig.timerLabelStrBuilder = FormattedStringBuilder("Event ends in: ");
    
    TimerNode* timerNode = TimerNode::create()->setup(timeConfig);
    timerNode->setPositionY(title->getPositionY() - timeConfig.fontSize * 2.5f);
    addChild(timerNode);
    
    auto timerAction = RepeatForever::create(Sequence::create({
        DelayTime::create(1),
        CallFunc::create([=](){
            if(remainingTime > 0)
            {
                remainingTime--;

                timerNode->updateValueString(getRemainingTimeString(remainingTime));
                
                timerNode->centerNodes();
            }
            else
            {
                PopupLayer::current()->destroyAllPopups();
            }
        })
    }));
    timerAction->setTag(777);
    runAction(timerAction);
    
    auto divider = Sprite::create("ui/popup/endlessPortalPopup/Fade_Darkblue.png");
    FIT_W(divider, GET_WIDTH * .85f);
    divider->setPositionY(GET_HEIGHT * -.215f);
    addChild(divider);
    
    auto descriptionLabel = HBLabel::create("Conquer stages, top the leaderboard, claim the rewards.", FONTS::USE_LOCALIZATION, GET_WIDTH * .045f);
    descriptionLabel->setDimensions(GET_WIDTH * .8f, 0);
    descriptionLabel->setOpacity(255 * .9f);
    descriptionLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
    descriptionLabel->setPositionY(divider->getBoundingBox().getMaxY() - descriptionLabel->getContentSize().height);
    addChild(descriptionLabel);
        
    playButton = ButtonImage::create()->init("ui/popup/endlessPortalPopup/Button_Start.png");
    playButton->fitW(GET_WIDTH * .5f * getSafeSceneScale());
    playButton->setPopupButtonPriority();
    playButton->setPositionY(descriptionLabel->getBoundingBox().getMinY() - playButton->getBoundingBox().size.height * .65f);
    playButton->cancelOnMove = true;
    playButton->onPressFilter([=](){
        return endlessPortalRankPageNode == nullptr;
    });
    addChild(playButton);
    
    
    FormattedStringBuilder buttonText = RemoteEndlessPortalManager::getInstance()->getPlayButtonText();

    auto playLabel = HBLabel::create(buttonText, FONTS::USE_LOCALIZATION, playButton->mSprite->getBoundingBox().size.height * .24f);
    playLabel->setPosition(0.f, playButton->mSprite->getBoundingBox().size.height * .18f);
    playLabel->limitSize(Vec2(playButton->mSprite->getBoundingBox().size.width * .75f, playButton->mSprite->getBoundingBox().size.height * .5f));
    playLabel->setTextColor(Color4B(255, 247, 220, 255));
    playLabel->enableHBStyle(1.25f, 0, Color4B(165, 93, 9, 255));
    playButton->mNode->addChild(playLabel);
    
    
    LabelWithIconConfig playConfig;
    
    playConfig.iconPosition = IconPosition::LEFT;
    playConfig.labelHeight =  playButton->mSprite->getBoundingBox().size.height * .225f;
    playConfig.labelString = FormattedStringBuilder("x%i", EnergyManager::getInstance()->ENERGY_COST_PER_PLAY).ignoreLocalization();
    playConfig.labelColor = Color3B(123, 63, 0);
    playConfig.iconPath = "ui/popup/energy/gasIcon.png";
    
    auto labelWithIcon = LabelWithIcon::create();

    labelWithIcon->setup(playConfig);
    
    labelWithIcon->getLabel()->disableEffect();
    
    labelWithIcon->getIcon()->setAnchorPoint(Vec2(labelWithIcon->getIcon()->getAnchorPoint().x, .4f));
    
    labelWithIcon->setPosition(playLabel->getPosition().x,
                               -playButton->mSprite->getBoundingBox().size.height * .175f);
    
    playButton->mNode->addChild(labelWithIcon);
    
    auto playButtonPressed = [=](){
        if(EnergyManager::getInstance()->isOutOfEnergy())
        {
            DataEventManager::getInstance()->send("unsufficient_currency", {
                { "type", Value("gas") },
                { "source", Value("endlessMode") }
            });
            
            OfferPopupManager::getInstance()->fireTrigger(OfferTriggerType::NotEnoughEnergy);
            
            AudioManager::getInstance()->playEffect("locked.wav");
            
            PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp([=](){
                

            }));
            
            return;
        }
            
        ButtonImage::disableAllTouches();
        
        EnergyManager::getInstance()->spendEnergy("endlessMode");
    
        if(MenuNode::current())
        {
            
            if(MenuNode::current()->currencyBarPanel->getCurrencyBar(CurrencyBarType::ENERGY_BAR))
            {
                MenuNode::current()->currencyBarPanel->getCurrencyBar(CurrencyBarType::ENERGY_BAR)->updateLabel();
            }
            
            MenuNode::current()->showSpentEnergy(playButton);
        }
        
        EndlessStageGameManager::getInstance()->startGame();
    };
    
    playButton->onTap([=](){
        playButtonPressed();
    });

    UIButton* ranksButton = UIButton::create()->setUp(UIButtonType::BLUE_CUT_OFF, FormattedStringBuilder("Ranks").ignoreSave(), playButton->mSprite->getBoundingBox().size.height * 1.f, Size(290, 212), .7f);
    ranksButton->button->setPopupButtonPriority();
    ranksButton->setPosition(GET_WIDTH * .5f - ranksButton->button->getBoundingBox().size.width * .5f, playButton->getPositionY() - playButton->getBoundingBox().size.height * .5f + ranksButton->getBoundingBox().size.height * .5f);
    ranksButton->button->onPressFilter([=](){
        return endlessPortalRankPageNode == nullptr;
    });
    addChild(ranksButton);
    
    ranksButton->label->setAnchorPoint(Vec2(.375f, .75f));
    
    auto ranksIcon = Sprite::create("ui/popup/endlessPortalPopup/Icon_Tournament.png");
    FIT_W(ranksIcon, ranksButton->button->mSprite->getBoundingBox().size.width * .8f);
    ranksIcon->setAnchorPoint(Vec2(.425f, .5f));
    ranksIcon->setPositionY(ranksButton->button->mSprite->getBoundingBox().size.height * .4f);
    ranksButton->button->mNode->addChild(ranksIcon);
    
    auto onRankNodeClosed = [=](){
        endlessPortalRankPageNode = nullptr;
    };
    
    ranksButton->button->onTap([=](){
        if(endlessPortalRankPageNode == nullptr)
        {
            endlessPortalRankPageNode = EndlessPortalRankPageNode::create()->setup(onRankNodeClosed, endlessPortalConfig);
            addChild(endlessPortalRankPageNode);
        }
    });
    
    
    return this;
}

EndlessPortalRankPageNode* EndlessPortalRankPageNode::setup(std::function<void()> onClosed, EndlessPortalConfig* endlessPortalConfig)
{
    float fullWidth = GET_WIDTH;
    float fullHeight = GET_HEIGHT;
    float scaledWidth = fullWidth * getSafeSceneScale();

    this->config = endlessPortalConfig;
    auto onDataFills = [=](){
        
        if(playerDataList.size() < 1)
        {
            onClosed();
            return;
        }
        
        LayerGradient* bgGradient = LayerGradient::create(Color4B(6, 23, 47, 255), Color4B(30, 58, 97, 255), Vec2(0, -1));
        bgGradient->setPosition(Vec2(fullWidth, fullHeight) * -.5f);
        addChild(bgGradient);
        
        Sprite* bg = Sprite::create("ui/common/container/Container_Rewards_Blue.png");
        FIT_W(bg, scaledWidth);
        bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        bg->setPositionY(fullHeight * .5f - getSafeTopOffset() - scaledWidth * .2f);
        addChild(bg);
        
        BottomCloseNode* bcn = BottomCloseNode::create()->setUp(fullWidth, [=](){
            HapticFeedbackManager::getInstance()->play(MEDIUM);
            AudioManager::getInstance()->playEffect("click.wav", .5f);
            
            onClosed();
            removeFromParent();
        });
        bcn->closeButton->label->setString(FormattedStringBuilder("Back"_ignoreFormat));
        bcn->addTouchBlocker(PopupLayer::current()->currentPopupPriority - 3);
        addChild(bcn, 10);
        
        ButtonImage* infoButton = ButtonImage::create()->init("ui/common/button/Button_Info.png");
        infoButton->fitW(scaledWidth * .1f);
        infoButton->setPosition(fullWidth * -.5f + infoButton->getBoundingBox().size.width, fullHeight * .5f - getSafeTopOffset() - infoButton->getBoundingBox().size.height);
        infoButton->setPopupButtonPriority();
        addChild(infoButton);
        
        infoButton->onTap([=](){
            
            RemoteEndlessPortalManager::getInstance()->showEndlessInfoPopup();
        });
        
        
        HBLabel* title = HBLabel::createWithOutlineStyle(FormattedStringBuilder("Leaderboard"_ignoreFormat), FONTS::USE_LOCALIZATION, scaledWidth * .07f);
        title->setTextColor(Color4B(200, 255, 255, 255));
        title->specializedFigmaStyle(70.f, .75f, Color4B(0,0,0,0), Color4B(0,0,0,255/4));
        title->enableGradient(Color4B(132, 233, 255, 255), -45 , 90 , 2.f);
        title->setPositionY(bg->getPositionY() + scaledWidth * .06f);
        addChild(title);
        
        ButtonImage* exitIcon = ButtonImage::create()->init("ui/common/button/Button_Cancel_02.png");
        exitIcon->fitW(scaledWidth * .1f);
        exitIcon->setPosition(fullWidth * .5f - exitIcon->getBoundingBox().size.width * .8f, (GET_HEIGHT * .5f) - exitIcon->getBoundingBox().size.height - getSafeTopOffset());
        exitIcon->setPopupButtonPriority();
        exitIcon->onTap([=](){
            PopupLayer::current()->destroyPopup();
        });
        addChild(exitIcon);
        
        auto onUsernameChanged = [=](std::string username){
            if(bottomNode)
            {
                bottomNode->nameLabel->setString(username);
            }
            
            if(userNode)
            {
                userNode->nameLabel->setString(username);
            }
            
            if(userTopNode)
            {
                userTopNode->nameLabel->setString(username);
            }
            
            if(ConfigManager::getInstance()->ENABLE_LIVE_LEADERBOARD == 1)
            {
                LeaderboardManager::getInstance()->changeNickname(username);
            }
        };
        
        ButtonImage* nicknameButton = ButtonImage::create()->init("ui/common/button/Button_ChangeName.png");
        nicknameButton->fitW(scaledWidth * .1f);
        nicknameButton->setPosition(infoButton->getPositionX() + nicknameButton->getBoundingBox().size.width * 1.f, infoButton->getPositionY());
        nicknameButton->setPopupButtonPriority();
        addChild(nicknameButton);
        
        nicknameButton->onTap([=](){
            
            GameManager::getInstance()->getTextInputForUsername([=](std::string username){
                onUsernameChanged(username);
            });
            
        });
        
        remainingTime = (int)RemoteEndlessPortalManager::getInstance()->getRemainingTimeInSeconds();
        
        TimerNodeConfig timeConfig;
        timeConfig.fontSize = title->getBoundingBox().size.height * .45f;
        timeConfig.startTime = remainingTime;
        timeConfig.timerLabelStrBuilder = FormattedStringBuilder("Ends in: ");
        
        TimerNode* timerNode = TimerNode::create()->setup(timeConfig);
        timerNode->setPositionY(title->getPositionY() - title->getBoundingBox().size.height);
        addChild(timerNode);
        
        auto timerAction = RepeatForever::create(Sequence::create({
            DelayTime::create(1),
            CallFunc::create([=](){
                if(remainingTime > 0)
                {
                    remainingTime--;

                    timerNode->updateValueString(getRemainingTimeString(remainingTime));
                    
                    timerNode->centerNodes();
                }
                else
                {
                    PopupLayer::current()->destroyAllPopups();
                }
            })
        }));
        timerAction->setTag(777);
        runAction(timerAction);
        
        topNode = Node::create();
        topNode->setPositionY(bg->getPositionY() - bg->getBoundingBox().size.width * .55f);
        addChild(topNode, 5);
        
        //
        
        float scrollHeight = (topNode->getPositionY() - fullHeight * -.5f) * .99f;
        scrollArea = Rect(0, 0, scaledWidth * .92f, scrollHeight);
        
        scrollArea.origin = scrollArea.size * -.5f;
        
        scrollNode = ScrollNode::create();
        scrollNode->setup(scrollArea, true, true, false, true);
        scrollNode->setPositionY(fullHeight * -.5f + (topNode->getPositionY() - fullHeight * -.5f) * .505f);
        addChild(scrollNode);
        
        //
        
        for(int i = 0; i < playerDataList.size(); ++i)
        {
            if(i < 3)
            {
                float posX = i == 1 ? scaledWidth * -.32f : (i == 2 ? scaledWidth * .32f : 0.f);
                
                EndlessPortalTopNode* ttNode = EndlessPortalTopNode::create()->setup(i == 0 ?  scaledWidth* .3f : scaledWidth * .285f, playerDataList.at(i));
                ttNode->setPositionX(posX);
                topNode->addChild(ttNode);
                
                endlessPortalTopNodes.push_back(ttNode);
                
                if(playerDataList.at(i).isMine)
                {
                    userTopNode = ttNode;
                }
            }
            else
            {
                EndlessPortalCommonNode* tcNode = EndlessPortalCommonNode::create()->setup(scaledWidth * .9f, playerDataList.at(i));
                
                tcNode->setPositionY((i - 3) * scaledWidth * -.15f);
                scrollNode->contentNode->addChild(tcNode);
                
                endlessPortalCommonNodes.push_back(tcNode);
                
                if(playerDataList.at(i).isMine)
                {
                    userNode = tcNode;
                }
            }
        }
        
        scrollNode->setTopScrollLimit(scrollArea.size.height * .5f - scaledWidth * .14f);
        
        if(endlessPortalCommonNodes.size() > 0)
        {
            scrollNode->setBottomScrollLimit(-endlessPortalCommonNodes.back()->getPositionY() - scrollArea.size.height * .5f + scaledWidth * .4f);
        }
        else
        {
            scrollNode->setBottomScrollLimit(scaledWidth * .4f);
        }
        
        
//        if(endlessPortalCommonNodes.size() > 0)
//        {
//            scrollNode->setBottomScrollLimit(-endlessPortalCommonNodes.back()->getPositionY() - scrollArea.size.height * .5f + scaledWidth * .14f);
//        }
        
        //
        
        EndlessPortalPlayerDataUI data;
        data.index = currentPlayerIndex;
        data.isMine = true;
        data.playerName = FormattedStringBuilder(UserSettings::getInstance()->getPlayerName());
        data.score = endlessPortalConfig->playerData.stageCount;
        if((int)endlessPortalConfig->rewards.size() > currentPlayerIndex)
        {
            data.rewards = endlessPortalConfig->rewards.at(currentPlayerIndex);
        }
        
        bottomNode = EndlessPortalCommonNode::create()->setup(scaledWidth * .92f, data);
        
        bottomNode->setPositionY(scrollNode->getPositionY() - scrollArea.size.height * .5f + bottomNode->getHeight() * .6f + bcn->getHeight());
        addChild(bottomNode);
        
        if(bottomNode->chestButton)
        {
            bottomNode->chestButton->setPriority(PopupLayer::current()->currentPopupPriority - 4);
        }
        
        if(currentPlayerIndex >= 3)
        {
            scrollNode->onMoved = [=](float dt){
                
                bottomNode->setVisible(bottomNode->convertToWorldSpace(Vec2::ZERO).y > endlessPortalCommonNodes.at(currentPlayerIndex - 3)->convertToWorldSpace(Vec2::ZERO).y);
                
                endlessPortalCommonNodes.at(currentPlayerIndex - 3)->setVisible(!bottomNode->isVisible());
                
            };
        }
        else
        {
            bottomNode->setVisible(false);
        }
    };
    
    fillPlayerDataList(onDataFills);
    
    return this;
}

float EndlessPortalCommonNode::getHeight()
{
    return bg->getBoundingBox().size.height;
}


void EndlessPortalRankPageNode::fillPlayerDataList(std::function<void()> callback)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    EndlessPortalPlayerDataUI playerData;
    playerData.isMine = true;
    playerData.playerName = FormattedStringBuilder(UserSettings::getInstance()->getPlayerName());
    playerData.score = config->playerData.stageCount;
    
//    playerDataList.push_back(playerData);
    
    for(int i = 0; i < config->playerCount - 1; ++i)
    {
        EndlessPortalPlayerDataUI npcData;
        npcData.isMine = false;
        npcData.playerName = config->playerData.npcList.at(i).nickname;
        npcData.score = RemoteEndlessPortalManager::getInstance()->getNpcScore(&config->playerData.npcList.at(i));
        
        playerDataList.push_back(npcData);
    }
    
    std::sort(playerDataList.begin(), playerDataList.end(), [](const EndlessPortalPlayerDataUI& a, const EndlessPortalPlayerDataUI& b) {
        
        return a.score > b.score;
    });
    
    bool found = false;
    for(int i = 0; i < (int)playerDataList.size(); ++i)
    {
        if(playerDataList.at(i).score <= playerData.score)
        {
            playerDataList.insert(playerDataList.begin() + i, playerData);
            found = true;
            break;
        }
    }
    
    if(!found)
    {
        playerDataList.push_back(playerData);
    }
    
    for(int i = 0; i < config->playerCount; ++i)
    {
        playerDataList.at(i).index = i;
        if((int)config->rewards.size() > i)
        {
            playerDataList.at(i).rewards = config->rewards.at(i);
        }
        
        if(playerDataList.at(i).isMine)
        {
            currentPlayerIndex = i;
        }
    }
#else
    if(ConfigManager::getInstance()->ENABLE_LIVE_LEADERBOARD == 0)
    {
        EndlessPortalPlayerDataUI playerData;
        playerData.isMine = true;
        playerData.playerName = FormattedStringBuilder(UserSettings::getInstance()->getPlayerName()).ignoreSave().ignoreLocalization();
        playerData.score = config->playerData.stageCount;
        
    //    playerDataList.push_back(playerData);
        
        for(int i = 0; i < config->playerCount - 1; ++i)
        {
            EndlessPortalPlayerDataUI npcData;
            npcData.isMine = false;
            npcData.playerName = config->playerData.npcList.at(i).nickname.ignoreSave().ignoreLocalization();
            npcData.score = RemoteEndlessPortalManager::getInstance()->getNpcScore(&config->playerData.npcList.at(i));
            
            playerDataList.push_back(npcData);
        }
        
        std::sort(playerDataList.begin(), playerDataList.end(), [](const EndlessPortalPlayerDataUI& a, const EndlessPortalPlayerDataUI& b) {
            
            return a.score > b.score;
        });
        
        bool found = false;
        for(int i = 0; i < (int)playerDataList.size(); ++i)
        {
            if(playerDataList.at(i).score <= playerData.score)
            {
                playerDataList.insert(playerDataList.begin() + i, playerData);
                found = true;
                break;
            }
        }
        
        if(!found)
        {
            playerDataList.push_back(playerData);
        }
        
        for(int i = 0; i < config->playerCount; ++i)
        {
            playerDataList.at(i).index = i;
            if((int)config->rewards.size() > i)
            {
                playerDataList.at(i).rewards = config->rewards.at(i);
            }
            
            if(playerDataList.at(i).isMine)
            {
                currentPlayerIndex = i;
            }
        }
        
        callback();
    }
    else
    {
        
        GameManager::getInstance()->showLoading();
        
        LeaderboardManager::getInstance()->getLeaderboard(RemoteEndlessPortalManager::getInstance()->getTournamentID(), [=](std::vector<LeaderboardData> leaderboardData){

            EndlessPortalPlayerDataUI personData;;
            
            for (auto data: playerDataList)
            {
                if(data.isMine)
                {
                    personData = data;
                    break;
                }
            }
            
            for (int i = 0; i < leaderboardData.size(); i++)
            {
                EndlessPortalPlayerDataUI npcData;
                npcData.isMine = leaderboardData[i].isMe;
                
                if(npcData.isMine && (leaderboardData[i].profileData.name.empty() || leaderboardData[i].profileData.name == STORAGE_EMPTY_VALUE))
                {
                        npcData.playerName = FormattedStringBuilder("You").ignoreLocalization();
                }
                else
                {
                    npcData.playerName = FormattedStringBuilder(leaderboardData[i].profileData.name).ignoreLocalization();
                }
                
                npcData.score = leaderboardData[i].score;
                playerDataList.push_back(npcData);
            }
            
            std::sort(playerDataList.begin(), playerDataList.end(), [](const EndlessPortalPlayerDataUI& a, const EndlessPortalPlayerDataUI& b) {
                
                return a.score > b.score;
            });
            
            for(int i = 0; i < playerDataList.size(); ++i)
            {
                playerDataList.at(i).index = i;
                if((int)config->rewards.size() > i)
                {
                    playerDataList.at(i).rewards = config->rewards.at(i);
                }
                
                if(playerDataList.at(i).isMine)
                {
                    currentPlayerIndex = i;
                }
            }
            
            callback();
            
            GameManager::getInstance()->hideLoading();
        });
        
        runAction(Sequence::create(DelayTime::create(5.f), CallFunc::create([=](){
            if(GameManager::getInstance()->isShowingLoading())
            {
                GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Server connection failed"), FONTS::USE_LOCALIZATION);
                GameManager::getInstance()->hideLoading();
                callback();
            }
        }), NULL));
    }
#endif
}


EndlessPortalCommonNode* EndlessPortalCommonNode::setup(float width, EndlessPortalPlayerDataUI playerData)
{
    this->width = width;
    this->playerData = playerData;
    
    bg = Sprite::create(playerData.isMine ? "ui/popup/endlessPortalPopup/Container_ListItem_EndlessEvent_02.png" : "ui/popup/endlessPortalPopup/Container_ListItem_EndlessEvent_01.png");
    FIT_W(bg, width);
    addChild(bg);
    

    
    //
    
    Sprite* rankSprite = Sprite::create(playerData.isMine ? "ui/popup/endlessPortalPopup/Container_ListItemNumber_EndlessEvent_02.png" : "ui/popup/endlessPortalPopup/Container_ListItemNumber_EndlessEvent_01.png");
    FIT_W(rankSprite, width * .11f);
    rankSprite->setPosition(width * -.41f, width * .0025f);
    addChild(rankSprite);
    
    HBLabel* standingLabel = HBLabel::create(FormattedStringBuilder("%d", playerData.index + 1).ignoreLocalization(), FONTS::USE_LOCALIZATION, rankSprite->getBoundingBox().size.width * .45f);
    standingLabel->setPosition(rankSprite->getPosition());
    standingLabel->setTextColor(Color4B(225, 230, 255, 255));
    addChild(standingLabel);
    
    //
    
    nameLabel = HBLabel::create(playerData.playerName, FONTS::USE_LOCALIZATION, width * .05f);
    nameLabel->limitSize(Vec2(width * .6f, width * .3f));
    nameLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    nameLabel->setAlignment(TextHAlignment::LEFT);
    nameLabel->setColor(playerData.isMine ? Color3B(17, 77, 80) : Color3B(195, 219, 255));
    nameLabel->setPosition(width * -.32f, rankSprite->getPositionY());
    
    addChild(nameLabel);
    
    //
    
    
    LabelWithIconConfig labelConfig;
    labelConfig.labelColor = Color3B(109, 220, 255);
    labelConfig.labelString = FormattedStringBuilder(std::to_string(playerData.score)).ignoreLocalization();
    labelConfig.labelHeight = width * .05f;
    labelConfig.iconPath = RemoteEndlessPortalManager::getInstance()->getEndlessSmallIconPath();
    labelConfig.iconPosition = IconPosition::LEFT;
    labelConfig.isOutlined = true;
    labelConfig.iconSizeFactor = 1.75f;
    
    LabelWithIcon* scoreLabel = LabelWithIcon::create();
    scoreLabel->setup(labelConfig);
    scoreLabel->getLabel()->specializedFigmaStyle(.01f, .01f, Color4B(0,0,0,0), Color4B(0,0,0,0));
    scoreLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    scoreLabel->setPosition(width * .35f, bg->getBoundingBox().size.height * .02f);
    addChild(scoreLabel);
    
    //
    
    if(!playerData.rewards.empty())
    {
        chestButton = ButtonImage::create()->init(RemoteEndlessPortalManager::getInstance()->getChestPath(3));
        chestButton->fitW(width * .125f);
        chestButton->setPosition(width * .1f, 0);
        chestButton->setPopupButtonPriority();
        chestButton->cancelOnMove = true;
        chestButton->isInMaskedArea = true;
        chestButton->addLightShader(StringUtils::format("chestBtn_%d", playerData.index), 0,  Vec4(1.f, 1.f, 1.f, .25f), 1.f, 1.f, .1f);
        addChild(chestButton);
        
        chestButton->onTap([=](){
            
            HapticFeedbackManager::getInstance()->play(MEDIUM);
            AudioManager::getInstance()->playEffect("click.wav", .5f);
            
            float gridItemWidth = GET_WIDTH * .15f;
            int rwSize = (int)playerData.rewards.size();
            
            Node* tooltipParent = Node::create();
            tooltipParent->setContentSize(Size(gridItemWidth * (rwSize > 3 ? 3 : (rwSize % 3)) + gridItemWidth * .2f, gridItemWidth * ((rwSize / 3) + 1) + gridItemWidth * .2f));
            
            Node* middleContentNode = Node::create();
            tooltipParent->addChild(middleContentNode);
            
            auto* tooltip = ToolTipNode::create(chestButton, tooltipParent, true, ToolTipWay::HORIZONTAL_LEFT);
            
            if(!tooltip)
            {
                if(tooltipParent)
                {
                    tooltipParent->removeFromParent();
                }
                
                return;
            }
            
            setCascadeGlobalZOrder(tooltip, 0);
                           
            GridData rewardsGridData;
            rewardsGridData.width = GET_WIDTH;
            rewardsGridData.itemCount = (int) playerData.rewards.size();
            rewardsGridData.rowNodeCount = 3;
            rewardsGridData.padding_horizontal = gridItemWidth * .1f;
            rewardsGridData.padding_vertical = gridItemWidth * .1f;
            rewardsGridData.itemWidth = gridItemWidth;
            
            Grid<RewardNode> rewardsGrid;
            float gridHeight = rewardsGrid.setUp(middleContentNode, rewardsGridData, 0.f, [=](RewardNode* rewardNode, int i){
                
                RewardNodeData rewardNodeData;
                rewardNodeData.rewardData = playerData.rewards[i];
                rewardNodeData.width = gridItemWidth;
                rewardNodeData.state = RewardState::READY_TO_CLAIM;
                    
                rewardNode->setUp(rewardNodeData);
            });

            middleContentNode->setPositionY(gridHeight * .5f);
        });
    }

    
    return this;
}


EndlessPortalTopNode* EndlessPortalTopNode::setup(float width, EndlessPortalPlayerDataUI playerData)
{
    this->width = width;
    this->playerData = playerData;
    
    Sprite* bg = Sprite::create(StringUtils::format("ui/popup/endlessPortalPopup/Container_TopRank_Tournament_0%i.png", playerData.index+1));
    FIT_W(bg, width);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    addChild(bg);
    
    Sprite* selectedBg = Sprite::create(playerData.index == 0 ? "ui/popup/tournament/Border_TopRank_Tournament_Long.png" : "ui/popup/tournament/Border_TopRank_Tournament_Short.png");
    selectedBg->setScale(bg->getScale());
    selectedBg->setPositionY(bg->getBoundingBox().getMidY());
    selectedBg->setVisible(playerData.isMine);
    addChild(selectedBg, -1);
    
    //
    
    ButtonImage* chestButton = ButtonImage::create()->init(RemoteEndlessPortalManager::getInstance()->getChestPath(playerData.index));
    chestButton->fitW(width * .7f);
    chestButton->setPositionY(bg->getBoundingBox().size.height * .9f);
    chestButton->setPopupButtonPriority();
    chestButton->addLightShader(StringUtils::format("chestBtn_%d", playerData.index), 0,  Vec4(1.f, 1.f, 1.f, .25f), 1.f, 1.f, .1f);
    addChild(chestButton);
    
    chestButton->onTap([=](){
        
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        float gridItemWidth = GET_WIDTH * .15f;
        int rwSize = (int)playerData.rewards.size();
        
        Node* tooltipParent = Node::create();
        tooltipParent->setContentSize(Size(gridItemWidth * (rwSize > 3 ? 3 : (rwSize % 3)) + gridItemWidth * .2f, gridItemWidth * ((rwSize / 3) + 1) + gridItemWidth * .2f));
        
        Node* middleContentNode = Node::create();
        tooltipParent->addChild(middleContentNode);
        
        auto* tooltip = ToolTipNode::create(chestButton, tooltipParent, true, ToolTipWay::VERTICAL_BOTTOM);
        
        if(!tooltip)
        {
            if(tooltipParent)
            {
                tooltipParent->removeFromParent();
            }
            
            return;
        }
        
        setCascadeGlobalZOrder(tooltip, 0);
                       
        GridData rewardsGridData;
        rewardsGridData.width = GET_WIDTH;
        rewardsGridData.itemCount = (int) playerData.rewards.size();
        rewardsGridData.rowNodeCount = 3;
        rewardsGridData.padding_horizontal = gridItemWidth * .1f;
        rewardsGridData.padding_vertical = gridItemWidth * .1f;
        rewardsGridData.itemWidth = gridItemWidth;
        
        Grid<RewardNode> rewardsGrid;
        float gridHeight = rewardsGrid.setUp(middleContentNode, rewardsGridData, 0.f, [=](RewardNode* rewardNode, int i){
            
            RewardNodeData rewardNodeData;
            rewardNodeData.rewardData = playerData.rewards[i];
            rewardNodeData.width = gridItemWidth;
            rewardNodeData.state = RewardState::READY_TO_CLAIM;
                
            rewardNode->setUp(rewardNodeData);
        });

        middleContentNode->setPositionY(gridHeight * .5f);
        
    });
    
//    chestButton->runAction(RepeatForever::create(Sequence::create({
//        DelayTime::create(.2f * (playerData.index + 1)),
//        EaseSineInOut::create(MoveBy::create(1.f, Vec2(0, width * .05f))),
//        EaseSineInOut::create(MoveBy::create(1.f, Vec2(0, width * -.05f)))
//    })));
    
    //
    
    Sprite* standingBg = Sprite::create(StringUtils::format("ui/popup/tournament/Icon_Badge_Tournament_0%d.png", playerData.index + 1));
    FIT_W(standingBg, width * (playerData.index == 0 ? .4f : .35f));
    standingBg->setPositionY(bg->getPositionY() + width * .03f);
    addChild(standingBg);
    
    HBLabel* standingLabel = HBLabel::createWithDarkStyle(FormattedStringBuilder("%d", playerData.index + 1).ignoreLocalization(), FONTS::USE_LOCALIZATION, standingBg->getBoundingBox().size.width * .35f);
    standingLabel->setPosition(standingBg->getPosition() + Vec2(0, standingBg->getBoundingBox().size.width * .08f));
    addChild(standingLabel);
    
    //
    Color4B color = playerData.index == 0 ? Color4B(165, 93, 9, 255) : (playerData.index == 1 ? Color4B(90, 5, 112, 255) : Color4B(145, 10, 10, 255));
    
    nameLabel = HBLabel::createWithOutlineStyle(FormattedStringBuilder(playerData.playerName).ignoreLocalization(), FONTS::USE_LOCALIZATION, width * .13f);
    nameLabel->setPositionY(chestButton->getPositionY() - width * .45f);
    nameLabel->specializedFigmaStyle(70.f, 0, color, Color4B(0,0,0,255/4));
    nameLabel->limitSize(Vec2(width * .9f, width * .3f));
    
    addChild(nameLabel);
    //
    
    LabelWithIconConfig labelConfig;
    labelConfig.isOutlined = true;
    labelConfig.labelColor = Color3B(255, 236, 138);
    labelConfig.labelString = FormattedStringBuilder(std::to_string(playerData.score)).ignoreLocalization();
    labelConfig.labelHeight = width * .13f;
    labelConfig.iconPath = RemoteEndlessPortalManager::getInstance()->getEndlessSmallIconPath();
    labelConfig.iconPosition = IconPosition::LEFT;
    labelConfig.iconSizeFactor = 1.5f;
    labelConfig.iconLabelSpaceFactor = width * .07f * .05f;
    
    LabelWithIcon* scoreLabel = LabelWithIcon::create();
    scoreLabel->setup(labelConfig);
    scoreLabel->setPositionY(chestButton->getPositionY() - width * .65f);
    scoreLabel->getLabel()->specializedFigmaStyle(70.f, 0, color, Color4B(0,0,0,255/4));
    addChild(scoreLabel);
    
    return this;
}


void EndlessPortalPopup::onExit()
{
    if(MenuNode::current() && MenuNode::current()->currencyBarPanel)
    {
            MenuNode::current()->currencyBarPanel->popArrangement();
    }
    
    onExitPopup();
    
    Node::onExit();
}

void EndlessPortalPopup::onPlayButtonPressed()
{
    if(EnergyManager::getInstance()->isOutOfEnergy())
    {
        DataEventManager::getInstance()->send("unsufficient_currency", {
            { "type", Value("gas") },
            { "source", Value("endlessMode") }
        });
        
        OfferPopupManager::getInstance()->fireTrigger(OfferTriggerType::NotEnoughEnergy);
        
        AudioManager::getInstance()->playEffect("locked.wav");
        
        PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp([=](){
            onPlayButtonPressed();
        }));
        
        return;
    }
        
    ButtonImage::disableAllTouches(true);
    
    EnergyManager::getInstance()->spendEnergy("endlessMode");

    if(MenuNode::current())
    {
        
        if(MenuNode::current()->currencyBarPanel->getCurrencyBar(CurrencyBarType::ENERGY_BAR))
        {
            MenuNode::current()->currencyBarPanel->getCurrencyBar(CurrencyBarType::ENERGY_BAR)->updateLabel();
        }
        
        MenuNode::current()->showSpentEnergy(playButton);
    }
    
    EndlessStageGameManager::getInstance()->startGame();
}
