#include "TournamentPopup.h"
#include "../../Menu/PopupLayer.h"
#include "../../GameManager.h"
#include "../../UserSettings.h"
#include "../../Menu/MenuNode.h"
#include "TournamentClaimRewardPopup.h"
#include "../../UI/ToolTipNode.h"
#include "../../UI/Grid.h"
#include "../../HapticFeedbackManager.h"
#include "../../AudioManager.h"
#include "../../StorageManager.h"
#include "../../UI/BottomCloseNode.hpp"
#include "../../UI/TimerNode.hpp"

static bool editNicknameShown = false;

// MARK: Common Node

TournamentCommonNode* TournamentCommonNode::setup(float width, TournamentPlayerDataUI playerData)
{
    this->width = width;
    this->playerData = playerData;
    
    bg = Sprite::create(playerData.isMine ? "ui/popup/tournament/Container_Rank_Tournament_Green.png" : "ui/popup/tournament/Container_Rank_Tournament_Default.png");
    FIT_W(bg, width);
    addChild(bg);
    
    //
    
    Sprite* rankSprite = Sprite::create("ui/popup/tournament/Background_CircleRank.png");
    FIT_W(rankSprite, width * .11f);
    rankSprite->setPosition(width * -.41f, width * .0025f);
    addChild(rankSprite);
    
    HBLabel* standingLabel = HBLabel::create(FormattedStringBuilder("%d", playerData.index + 1).ignoreLocalization(), FONTS::USE_LOCALIZATION, rankSprite->getBoundingBox().size.width * .45f);
    standingLabel->setPosition(rankSprite->getPosition());
    standingLabel->setTextColor(Color4B(225, 230, 255, 255));
    addChild(standingLabel);
    
    //
    FormattedStringBuilder nameLabelFormat = FormattedStringBuilder(playerData.playerName);
    if(nameLabelFormat.getOriginalString() != "You")
    {
        (void)nameLabelFormat.ignoreLocalization();
    }
    nameLabel = HBLabel::createWithDarkStyle(nameLabelFormat, FONTS::USE_LOCALIZATION, width * .05f);
    nameLabel->limitSize(Vec2(width * .6f, width * .3f));
    nameLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    nameLabel->setAlignment(TextHAlignment::LEFT);
    nameLabel->setPosition(width * -.32f, rankSprite->getPositionY());
    
    addChild(nameLabel);
    
    //
    
    Sprite* scoreBg = Sprite::create("ui/popup/tournament/Background_RankContainer_01.png");
    FIT_H(scoreBg, bg->getBoundingBox().size.height * .906f);
    scoreBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    scoreBg->setPosition(width * .495f, bg->getBoundingBox().size.height * .02f);
    addChild(scoreBg);
    
    LabelWithIconConfig labelConfig;
    labelConfig.labelColor = Color3B(255, 236, 138);
    labelConfig.labelString = FormattedStringBuilder(std::to_string(playerData.score));
    labelConfig.labelHeight = width * .05f;
    labelConfig.iconPath = RemoteTournamentManager::getInstance()->getTournamentSmallIconPath();
    labelConfig.iconPosition = IconPosition::LEFT;
    labelConfig.iconSizeFactor = 1.75f;
    labelConfig.iconLabelSpaceFactor = width * .025f * .05f;
    
    LabelWithIcon* scoreLabel = LabelWithIcon::create();
    scoreLabel->setup(labelConfig);
    scoreLabel->setPosition(scoreBg->getBoundingBox().getMidX(), scoreBg->getBoundingBox().getMidY());
    addChild(scoreLabel);
    
    //
    
    if(!playerData.rewards.empty())
    {
        chestButton = ButtonImage::create()->init(RemoteTournamentManager::getInstance()->getChestPath(3));
        chestButton->fitW(width * .15f);
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

float TournamentCommonNode::getHeight()
{
    return bg->getBoundingBox().size.height;
}

// MARK: Top Node

TournamentTopNode* TournamentTopNode::setup(float width, TournamentPlayerDataUI playerData)
{
    this->width = width;
    this->playerData = playerData;
    
    
    Sprite* bg = Sprite::create(StringUtils::format("ui/popup/endlessPortalPopup/Container_TopRank_Tournament_0%i.png", playerData.index + 1));
    FIT_W(bg, width);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    addChild(bg);
    
    Sprite* selectedBg = Sprite::create(playerData.index == 0 ? "ui/popup/tournament/Border_TopRank_Tournament_Long.png" : "ui/popup/tournament/Border_TopRank_Tournament_Short.png");
    selectedBg->setScale(bg->getScale());
    selectedBg->setPositionY(bg->getBoundingBox().getMidY());
    selectedBg->setVisible(playerData.isMine);
    addChild(selectedBg, -1);
    
    //
    
    ButtonImage* chestButton = ButtonImage::create()->init(RemoteTournamentManager::getInstance()->getChestPath(playerData.index));
    chestButton->fitW(width * .8f);
    chestButton->setPositionY(bg->getBoundingBox().size.height * .86f);
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
    
    nameLabel = HBLabel::createWithDarkStyle(FormattedStringBuilder(playerData.playerName).ignoreLocalization(), FONTS::USE_LOCALIZATION, width * .13f);
    nameLabel->setPositionY(chestButton->getPositionY() - width * .4f);
    nameLabel->limitSize(Vec2(width * .9f, width * .3f));
    
    addChild(nameLabel);
    
    //
    
    LabelWithIconConfig labelConfig;
    labelConfig.labelColor = Color3B(255, 236, 138);
    labelConfig.labelString = FormattedStringBuilder(std::to_string(playerData.score));
    labelConfig.labelHeight = width * .13f;
    labelConfig.iconPath = RemoteTournamentManager::getInstance()->getTournamentSmallIconPath();
    labelConfig.iconPosition = IconPosition::LEFT;
    labelConfig.iconSizeFactor = 1.5f;
    labelConfig.iconLabelSpaceFactor = width * .07f * .05f;
    
    LabelWithIcon* scoreLabel = LabelWithIcon::create();
    scoreLabel->setup(labelConfig);
    scoreLabel->setPositionY(chestButton->getPositionY() - width * .6f);
    addChild(scoreLabel);
    
    return this;
}


// MARK: Popup

TournamentPopup* TournamentPopup::setup(TournamentConfig* config)
{
    isFullScreen = true;
    
    popupName = "TournamentPopup";
    
    setUninterruptible();
    
    float fullWidth = GET_WIDTH;
    float fullHeight = GET_HEIGHT;
    width = fullWidth * getSafeSceneScale();
    
    this->config = config;
    
    if(MenuNode::current() && MenuNode::current()->currencyBarPanel)
    {
        MenuNode::current()->currencyBarPanel->pushArrangement({});
    }
    
    fillPlayerDataList();
    
    LayerGradient* bgGradient = LayerGradient::create(Color4B(6, 23, 47, 255), Color4B(30, 58, 97, 255), Vec2(0, -1));
    bgGradient->setPosition(Vec2(fullWidth, fullHeight) * -.5f);
    addChild(bgGradient);
    
    Sprite* bg = Sprite::create("ui/common/container/Container_Rewards_Yellow.png");
    FIT_W(bg, width);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    bg->setPositionY(fullHeight * .5f - getSafeTopOffset() - width * .2f);
    addChild(bg);
    
    BottomCloseNode* bcn = BottomCloseNode::create()->setUp(fullWidth, [=](){
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        PopupLayer::current()->destroyPopup();
    });
    bcn->addTouchBlocker(PopupLayer::current()->currentPopupPriority - 3);
    addChild(bcn, 10);
    
    ButtonImage* infoButton = ButtonImage::create()->init("ui/common/button/Button_Info.png");
    infoButton->fitW(width * .1f);
    infoButton->setPosition(fullWidth * .5f - infoButton->getBoundingBox().size.width, fullHeight * .5f - getSafeTopOffset() - infoButton->getBoundingBox().size.height);
    infoButton->setPopupButtonPriority();
    addChild(infoButton);
    
    infoButton->onTap([=](){
        
        RemoteTournamentManager::getInstance()->showTournamentInfoPopup();
        
    });
    
    if(ADMIN_PLAYER)
    {
        infoButton->onHold([=](){
            
            PopupLayer::current()->showPopup(TOURNAMENT_CLAIM_REWARD_POPUP, TournamentClaimRewardPopup::create()->setup(0, 1000, this->config->rewards.at(0)));
            //this->config->rewards.at(0)
            
        });
    }
    
    ButtonImage* nicknameButton = ButtonImage::create()->init("ui/common/button/Button_ChangeName.png");
    nicknameButton->fitW(width * .1f);
    nicknameButton->setPosition(-infoButton->getPositionX(), infoButton->getPositionY());
    nicknameButton->setPopupButtonPriority();
    addChild(nicknameButton);
    
    nicknameButton->onTap([=](){
        
        GameManager::getInstance()->getTextInputForUsername([=](std::string username){
            onUsernameChanged(username);
        });
        
    });
    
    if(!editNicknameShown && UserSettings::getInstance()->getPlayerName() == "You")
    {
        runAction(Sequence::create({
            DelayTime::create(.5f),
            CallFunc::create([=](){
                editNicknameShown = true;
                ToolTipNode::create(nicknameButton, FormattedStringBuilder("Change your nickname"), true, ToolTipWay::VERTICAL_BOTTOM);
            })
        }));
    }
    
    HBLabel* title = HBLabel::createWithOutlineStyle(config->title1Format, FONTS::USE_LOCALIZATION, width * .07f);
    title->setTextColor(Color4B(255, 255, 200, 255));
    title->specializedFigmaStyle(70.f, .75f, Color4B(0,0,0,0), Color4B(0,0,0,255/4));
    title->enableGradient(Color4B(245, 209, 67, 255), -45 , 90 , 2.f);
    title->setPositionY(bg->getPositionY() + width * .06f);
    addChild(title);
    
    //
    
    remainingTime = RemoteTournamentManager::getInstance()->getRemainingTimeInSeconds(this->config->tournamentID, this->config->eventID);
    
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
    
    //
    
    topNode = Node::create();
    topNode->setPositionY(bg->getPositionY() - bg->getBoundingBox().size.width * .55f);
    addChild(topNode, 5);
    
    //
    
    float scrollHeight = (topNode->getPositionY() - fullHeight * -.5f) * .99f;
    scrollArea = Rect(0, 0, width * .92f, scrollHeight);
    
    scrollArea.origin = scrollArea.size * -.5f;
    
    scrollNode = ScrollNode::create();
    scrollNode->setup(scrollArea, true, true, false, true);
    scrollNode->setPositionY(fullHeight * -.5f + (topNode->getPositionY() - fullHeight * -.5f) * .505f);
    addChild(scrollNode);

    //
    
    for(int i = 0; i < this->config->playerCount; ++i)
    {
        if(i < 3)
        {
            float posX = i == 1 ? width * -.32f : (i == 2 ? width * .32f : 0.f);
            
            TournamentTopNode* ttNode = TournamentTopNode::create()->setup(i == 0 ?  width* .3f : width * .285f, playerDataList.at(i));
            ttNode->setPositionX(posX);
            topNode->addChild(ttNode);
            
            tournamentTopNodes.push_back(ttNode);
            
            if(playerDataList.at(i).isMine)
            {
                userTopNode = ttNode;
            }
        }
        else
        {
            TournamentCommonNode* tcNode = TournamentCommonNode::create()->setup(width * .92f, playerDataList.at(i));
            
            tcNode->setPositionY((i - 3) * width * -.18f);
            scrollNode->contentNode->addChild(tcNode);
            
            tournamentCommonNodes.push_back(tcNode);
            
            if(playerDataList.at(i).isMine)
            {
                userNode = tcNode;
            }
        }
    }
    
    scrollNode->setTopScrollLimit(scrollArea.size.height * .5f - width * .14f);
    scrollNode->setBottomScrollLimit(-tournamentCommonNodes.back()->getPositionY() - scrollArea.size.height * .5f + width * .4f);
    
    //
    std::string playerName = UserSettings::getInstance()->getPlayerName();
    playerName = playerName == STORAGE_EMPTY_VALUE ? "You" : playerName;
    
    TournamentPlayerDataUI data;
    data.index = currentPlayerIndex;
    data.isMine = true;
    data.playerName = FormattedStringBuilder(playerName);
    data.score = config->playerData.playerScore;
    if((int)config->rewards.size() > currentPlayerIndex)
    {
        data.rewards = config->rewards.at(currentPlayerIndex);
    }
    
    bottomNode = TournamentCommonNode::create()->setup(width * .92f, data);
    
    bottomNode->setPositionY(scrollNode->getPositionY() - scrollArea.size.height * .5f + bottomNode->getHeight() * .6f + bcn->getHeight());
    addChild(bottomNode);
    
    if(bottomNode->chestButton)
    {
        bottomNode->chestButton->setPriority(PopupLayer::current()->currentPopupPriority - 4);
    }
    
    if(currentPlayerIndex >= 3)
    {
        scrollNode->onMoved = [=](float dt){
            
            bottomNode->setVisible(bottomNode->convertToWorldSpace(Vec2::ZERO).y > tournamentCommonNodes.at(currentPlayerIndex - 3)->convertToWorldSpace(Vec2::ZERO).y);
            
            tournamentCommonNodes.at(currentPlayerIndex - 3)->setVisible(!bottomNode->isVisible());
            
        };
    }
    else
    {
        bottomNode->setVisible(false);
    }
    
    
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(TournamentPopup::onTouchBegan, this);
    listener->setSwallowTouches(true);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, PopupLayer::current()->currentPopupPriority - 3);
    
    return this;
}

void TournamentPopup::fillPlayerDataList()
{
    std::string playerName = UserSettings::getInstance()->getPlayerName();
    playerName = playerName == STORAGE_EMPTY_VALUE ? "You" : playerName;
    
    TournamentPlayerDataUI playerData;
    playerData.isMine = true;
    playerData.playerName = FormattedStringBuilder(playerName);
    playerData.score = config->playerData.playerScore;
    
//    playerDataList.push_back(playerData);
    
    for(int i = 0; i < config->playerCount - 1; ++i)
    {
        TournamentPlayerDataUI npcData;
        npcData.isMine = false;
        npcData.playerName = config->playerData.npcList.at(i).nickname;
        npcData.score = RemoteTournamentManager::getInstance()->getNpcScore(&config->playerData.npcList.at(i));
        
        playerDataList.push_back(npcData);
    }
    
    std::sort(playerDataList.begin(), playerDataList.end(), [](const TournamentPlayerDataUI& a, const TournamentPlayerDataUI& b) {
        
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
    
}

void TournamentPopup::onExit()
{
    if(listener)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
        listener = NULL;
    }
    
    if(MenuNode::current() && MenuNode::current()->currencyBarPanel)
    {
        MenuNode::current()->currencyBarPanel->popArrangement();
    }
    
    onExitPopup();
    
    Node::onExit();
}

void TournamentPopup::onUsernameChanged(std::string username)
{
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
}

bool TournamentPopup::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    if(bottomNode && bottomNode->isVisible())
    {
        return bottomNode->bg->getBoundingBox().containsPoint(bottomNode->bg->getParent()->convertTouchToNodeSpace(touch));
    }
    
    return false;
}
