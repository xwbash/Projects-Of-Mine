//
//  EndlessPortalClaimRewardPopup.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#include "EndlessPortalClaimRewardPopup.hpp"
#include "../../RemoteEndlessPortalManager.hpp"
#include "../../GameManager.h"
#include "../../AudioManager.h"
#include "../../HapticFeedbackManager.h"
#include "../../Menu/MenuNode.h"
#include "../../Menu/PopupLayer.h"
#include "../../HapticFeedbackManager.h"
#include "../../AudioManager.h"
#include "../../PlayerLevelManager.h"

EndlessPortalClaimRewardPopup* EndlessPortalClaimRewardPopup::setup(int placement, int score, std::vector<RewardData> rewards)
{
    float width = GET_WIDTH *  getSafeSceneScale();
    
    rewardsData = rewards;
    
    bool isWinner = !rewards.empty();
    
    MenuNode::current()->currencyBarPanel->pushArrangement({});
    
    //
    
    Sprite* bg = Sprite::create("ui/popup/rewardsBottomNode/Background_Rewards_02.png");
    FIT_W(bg, width);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    addChild(bg);
    
    Sprite* bgLine = Sprite::create("ui/popup/rewardsBottomNode/Background_Rewards_01.png");
    FIT_W(bgLine, width);
    bgLine->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    bgLine->setColor(Color3B(239, 189, 12));
    bgLine->setOpacity(206);
    addChild(bgLine);
    
    //
    
    Sprite* placementBg = Sprite::create("ui/popup/tournamentInfo/Background_TournamentRewards.png");
    FIT_W(placementBg, width * .6f);
    placementBg->setPositionY(width * .025f);
    addChild(placementBg);
    
    HBLabel* placementLabel = HBLabel::create(getPlacementString(placement + 1), FONTS::USE_LOCALIZATION, placementBg->getBoundingBox().size.height * .5f);
    placementLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    placementLabel->setAlignment(TextHAlignment::LEFT);
    placementLabel->setPosition(placementBg->getPositionX() + placementBg->getBoundingBox().size.width * -.4f, placementBg->getPositionY());
    addChild(placementLabel);
    
    LabelWithIconConfig labelConfig;
    labelConfig.labelColor = Color3B(255, 236, 138);
    labelConfig.labelString = FormattedStringBuilder(std::to_string(score)).ignoreLocalization();
    labelConfig.labelHeight = placementBg->getBoundingBox().size.height * .5f;
    labelConfig.iconPath = RemoteEndlessPortalManager::getInstance()->getEndlessSmallIconPath();
    labelConfig.iconPosition = IconPosition::LEFT;
    labelConfig.iconSizeFactor = 1.2f;
    labelConfig.iconLabelSpaceFactor = width * .02f * .05f;
    
    LabelWithIcon* scoreLabel = LabelWithIcon::create();
    scoreLabel->setup(labelConfig);
    scoreLabel->setPosition(placementBg->getPositionX() + placementBg->getBoundingBox().size.width * .4f - scoreLabel->getBoundingBox().size.width * .5f, placementBg->getPositionY());
    addChild(scoreLabel);
    
    //
    
    HBLabel* titleLabelUpper = HBLabel::create("Tournament", FONTS::USE_LOCALIZATION, width * .06f);
    titleLabelUpper->setPositionY(placementBg->getPositionY() + width * .25f);
    addChild(titleLabelUpper);
    
    HBLabel* titleLabelLower = HBLabel::create("Ended", FONTS::USE_LOCALIZATION, width * .09f);
    titleLabelLower->setPositionY(titleLabelUpper->getBoundingBox().getMinY() - titleLabelLower->getBoundingBox().size.height * .38f);
    titleLabelLower->setTextColor(Color4B(255, 205, 27, 255));
    addChild(titleLabelLower);
    
    //
    
    Sprite* icon = Sprite::create(getIconPath(placement, isWinner));
    FIT_WorH(icon, width * .4f, width * .4f);
    icon->setPositionY(titleLabelUpper->getBoundingBox().getMaxY() + width * .25f);
    addChild(icon);
    
    Sprite* light = Sprite::create("ui/popup/tournamentInfo/Background_TournamentInfo_01.png");
    FIT_W(light, width * .9f);
    light->setOpacity(26);
    light->setPositionY(icon->getPositionY());
    addChild(light, -1);
    
    light->runAction(RepeatForever::create(RotateBy::create(1.f, 30)));
    
    //
    
    if(isWinner)
    {
        collectButton = UIButton::create();
        collectButton->setUp(UIButtonType::GREEN, FormattedStringBuilder("Collect"), width * .44f, Size(428, 192), .75f);
        collectButton->button->setPopupButtonPriority();
        collectButton->setPositionY(width * -.7f);
        collectButton->setVisible(false);
        addChild(collectButton);
            
        collectButton->button->onTap([=](){
            
            collectButton->setVisible(false);
                    
            HapticFeedbackManager::getInstance()->play(HEAVY);
            AudioManager::getInstance()->playEffect("collect.wav");
            
            PopupLayer::current()->destroyPopup();
            
        });
        
        //
        
        int maxRewardCountPerRow = 5;
        int rewardCount = (int)rewardsData.size();
        int rewardCountPerRow = maxRewardCountPerRow;
        if(rewardCount > maxRewardCountPerRow && rewardCount % maxRewardCountPerRow > 0)
        {
            int rowNeeded = 1 + (rewardCount / maxRewardCountPerRow);
            int rowNeededTemp = 1 + (rewardCount / (maxRewardCountPerRow - 1));
            
            if(rowNeeded >= rowNeededTemp)
            {
                rewardCountPerRow--;
            }
        }
        
        float gridItemWidth = width * .17f;
        
        Node* middleContentNode = Node::create();
        
        GridData rewardsGridData;
        rewardsGridData.itemCount = (int) rewardsData.size();
        rewardsGridData.rowNodeCount = rewardCountPerRow;
        rewardsGridData.padding_horizontal = gridItemWidth * .1f;
        rewardsGridData.padding_vertical = gridItemWidth * .1f;
        rewardsGridData.itemWidth = gridItemWidth;
        rewardsGridData.backgroundMargin = width * .03f;
        
        Grid<RewardNode> rewardsGrid;
        rewardsGrid.setUp(middleContentNode, rewardsGridData, 0, [gridItemWidth, this](RewardNode* node, int index){
            
            RewardNodeData rewardNodeData;
            rewardNodeData.index = index + 100;
            rewardNodeData.rewardData = rewardsData[index];
            rewardNodeData.width = gridItemWidth;
            rewardNodeData.state = RewardState::READY_TO_CLAIM;
                
            node->setUp(rewardNodeData);
            node->button->setPopupButtonPriority();
            
            rewardNodes.push_back(node);
            
        });
        
        //
        
//        float gridHeight = std::abs(rewardsGrid.gridItemPoses.at((int) rewardsGrid.gridItemPoses.size() - 1).y) + gridItemWidth * .5f;
//        float gridCenter = gridHeight * .5f;
        middleContentNode->setPositionY(bg->getPositionY() - width * .15f);
        
        addChild(middleContentNode);
        
        //
        
        playAnimation();
        
    }
    else
    {
        HBLabel* infoLabel = HBLabel::create("You don't have any reward. Get ready for upcoming tournament", FONTS::USE_LOCALIZATION, width * .05f);
        infoLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        infoLabel->setAlignment(TextHAlignment::CENTER);
        infoLabel->setOpacity(128);
        infoLabel->setDimensions(width * .8f, 0);
        infoLabel->setPositionY(bg->getPositionY() - width * .2f);
        addChild(infoLabel);
        
        UIButton* continueButton = UIButton::create();
        continueButton->setUp(UIButtonType::GREEN, FormattedStringBuilder("Continue"), width * .44f, Size(428, 192), .75f);
        continueButton->button->setPopupButtonPriority();
        continueButton->setPositionY(width * -.7f);
        addChild(continueButton);

        continueButton->button->onTap([=](){
            
            HapticFeedbackManager::getInstance()->play(MEDIUM);
            AudioManager::getInstance()->playEffect("click.wav", .5f);
            
            PopupLayer::current()->destroyPopup();
        });
    }
    
    //
    
    return this;
}

void EndlessPortalClaimRewardPopup::playAnimation()
{
    const auto startDelay = .05f;
    const auto betweenItemDelay = .1f;
    for(int i = 0; i < (int)rewardNodes.size(); i++)
    {
        const auto delay = startDelay + i * betweenItemDelay;
        RewardNode* rewardNode = rewardNodes.at(i);
        rewardNode->appearGlow(.2f, 1.3f, delay);
    }
    
    //
    
    collectButton->runAction(Sequence::create({
        
        DelayTime::create((int)rewardNodes.size() * betweenItemDelay),
        
        CallFunc::create([=](){
            
            AudioManager::getInstance()->playEffect("keyCollect.wav");
            
            collectButton->setVisible(true);
            collectButton->button->setScale(0.f);
            
            collectButton->button->animateScale();
            
            for(auto* rewardNode : rewardNodes)
            {
                rewardNode->toggleLightedSprite(true);
            }
            
        }),
        
    }));
}

void EndlessPortalClaimRewardPopup::onExit()
{
    if(MenuNode::current())
    {
        MenuNode::current()->currencyBarPanel->popArrangement();
        
        MenuNode::current()->currencyBarPanel->pushArrangement(MenuNode::current()->currencyBarPanel->getRequiredArrangement(rewardsData));
    }
    
    AudioManager::getInstance()->playEffect("collect.wav");
    
    ButtonImage::disableAllTouches();
    
    GameManager::getInstance()->showRewardCollectAnimation({ Vec2(GET_WIDTH * .5f, GET_HEIGHT * .5f) }, rewardsData, Vec2::ZERO, Vec2::ZERO, 0.f, 0.f, [=](){
        
        if(MenuNode::current())
        {
            MenuNode::current()->currencyBarPanel->popArrangement();
        }
        
        PlayerLevelManager::getInstance()->checkLevelUp([=](){});
        
        ButtonImage::enableAllTouches();
    });
    
    onExitPopup();
    Node::onExit();
}

FormattedStringBuilder EndlessPortalClaimRewardPopup::getPlacementString(int placement)
{
    std::vector<std::string> suffixes = {"th", "st", "nd", "rd"};
    
    int ord = placement % 100;
    if (ord / 10 == 1)
    {
        ord = 0;
    }
    
    ord = ord % 10;
    
    if (ord > 3)
    {
        ord = 0;
    }
    
    return FormattedStringBuilder("%d%s Place", placement, suffixes.at(ord).c_str());
}

std::string EndlessPortalClaimRewardPopup::getIconPath(int placement, bool isWinner)
{
    if(!isWinner)
    {
        return RemoteEndlessPortalManager::getInstance()->getMainMenuIconWithTimerPath();
    }
    
    int index = (int)clampf(placement, 0 , 3);
    return RemoteEndlessPortalManager::getInstance()->getChestPath(index);
}
