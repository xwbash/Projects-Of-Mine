#include "NoAdsOfferPopup.h"
#include "../GameManager.h"
#include "../Menu/MenuNode.h"
#include "../Menu/PopupLayer.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"
#include "../UI/UIButton.h"
#include "../IAPManager.h"
#include "../UI/ToolTipNode.h"
#include "../AdManager.h"

NoAdsOfferPopup* NoAdsOfferPopup::setup(std::vector<RewardData> rewards, bool fromInGame)
{
    width = GET_WIDTH * .85f * getSafeSceneScale();
    
    bool isRewarded = !rewards.empty();
    
    PopupNode::setUp(width, isRewarded ? Size(968, 1468) : Size(968, 1178), "", FormattedStringBuilder("No More Ads"_ignoreFormat));
    
    auto subBg = Sprite::create("ui/common/container/Container_Popup_01.png");
    subBg->setContentSize(isRewarded ? Size(912, 1064) :Size(912, 776));
    subBg->setCenterRectNormalized(Rect(.25f, .25f, .5f, .5f));
    FIT_W(subBg, baseRect.size.width * .936f);
    subBg->setPositionY(baseRect.size.width * .03f);
    addChild(subBg);
    
    //
    
    Sprite* noAdsIcon = Sprite::create("ui/popup/noAdsOfferPopup/No_Ads_Icon_Large.png");
    FIT_W(noAdsIcon, subBg->getBoundingBox().size.width * .8f);
    noAdsIcon->setPositionY(subBg->getBoundingBox().getMaxY() - width * .35f);
    addChild(noAdsIcon);
    
    //
    
    if(isRewarded)
    {

        Sprite* plusIcon = Sprite::create("ui/common/icon/Icon_Plus.png");
        FIT_W(plusIcon, width * .135f);
        plusIcon->setPositionY(noAdsIcon->getBoundingBox().getMinY() + width * .1f);
        addChild(plusIcon);
        
        //
        
        float itemWidth = subBg->getBoundingBox().size.width * .18f;
        
        GridData rewardGridData;
        rewardGridData.topMargin = itemWidth * .15f;
        rewardGridData.width = itemWidth * 1.3f * (int)rewards.size();
        rewardGridData.backgroundTexture = "ui/common/container/Container_Popup_Fill.png";
        rewardGridData.itemCount = (int)rewards.size();
        rewardGridData.rowNodeCount = 5;
        rewardGridData.padding_horizontal = itemWidth * .15f;
        rewardGridData.padding_vertical = itemWidth * .15f;
        rewardGridData.itemWidth = itemWidth;
        rewardGridData.backgroundMargin = itemWidth * .15f;
        rewardGridData.centerItems = true;
        rewardGridData.backgroundTextureWidthMultiplier = 1;
        rewardGridData.backgroundSliceRect = Rect(.25, .25, .5f, .5f);
        rewardGridData.backgroundSize = Size(600, 216);
                
        Grid<RewardNode> grid;
        grid.setUp(this, rewardGridData, plusIcon->getPositionY() - width * .04f, [=](RewardNode* node, int index){
            RewardNodeData rewardNodeData;
            
            rewardNodeData.rewardData = rewards[index];
            rewardNodeData.width = itemWidth;
            rewardNodeData.state = RewardState::READY_TO_CLAIM;
            
            node->setUp(rewardNodeData);
            node->button->setPopupButtonPriority();
        });
        
        //
        
        HBLabel* infoLabel2 = HBLabel::create("Removes full screen ads. Keeps ads for rewards!", FONTS::USE_LOCALIZATION, width * .05f);
        infoLabel2->setPositionY(subBg->getBoundingBox().getMinY() + width * .08f);
        infoLabel2->setTextColor(Color4B(208, 211, 221, 255));
        addChild(infoLabel2);
    }
    else
    {
        HBLabel* infoLabel1 = HBLabel::create("Remove Ads", FONTS::USE_LOCALIZATION, width * .06f);
        infoLabel1->setPositionY(noAdsIcon->getBoundingBox().getMinY() + width * .1f);
        infoLabel1->setTextColor(Color4B(179, 196, 255, 255));
        addChild(infoLabel1);

        HBLabel* infoLabel2 = HBLabel::create("Remove all non-optional ads!", FONTS::USE_LOCALIZATION, width * .05f);
        infoLabel2->setPositionY(subBg->getBoundingBox().getMinY() + width * .08f);
        infoLabel2->setTextColor(Color4B(208, 211, 221, 255));
        addChild(infoLabel2);
    }
    
    ButtonImage* helpButton = ButtonImage::create()->init("ui/common/button/Button_Info.png");
    helpButton->fitW(baseCloseButton->mSprite->getBoundingBox().size.width);
    helpButton->setPopupButtonPriority();
    helpButton->setPosition(baseRect.size.width * -.4f, baseTitle->getPositionY());
    baseHeaderParent->addChild(helpButton);
    
    helpButton->onTap([=](){
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        ToolTipNode::create(helpButton, FormattedStringBuilder("Rewarded videos will still be optional"));
    });
    
    //
    
    Sprite* dividerSp = Sprite::create("ui/popup/dialogPopup/Divider_InfoPopup.png");
    FIT_W(dividerSp, width * .8f);
    dividerSp->setPositionY(baseRect.getMinY() + width * .24f);
    addChild(dividerSp);
    
    UIButton* purchaseButton = UIButton::create()->setUpWithIAPProduct(UIButtonType::GREEN, ConfigManager::getInstance()->NO_ADS_PRODUCT_ID, width * .44f, Size(428, 192), .75f);
    purchaseButton->button->setPopupButtonPriority();
    purchaseButton->setPositionY(baseRect.getMinY() + purchaseButton->button->getBoundingBox().size.height * .65f);
    purchaseButton->startScaleAnimation();
    addChild(purchaseButton);
    
    purchaseButton->button->onTap([=](){
        
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        IAPManager::getInstance()->purchaseWrapper(ConfigManager::getInstance()->NO_ADS_PRODUCT_ID, "noAdsOfferPopup", [=](bool success){
            
            if(!success)
            {
                return;
            }
        
            for (const auto& item: m_onBoughtCallbacks)
            {
                if(item != nullptr)
                    item();
            }
            
            PopupLayer::current()->destroyPopup();
            
            AdManager::getInstance()->noAdsPurchasedDirectly();
            
            if(!fromInGame && BundleButtonsNode::current() != nullptr)
            {
                BundleButtonsNode::current()->update();
            }
            
            if(isRewarded)
            {
                collectAndHandleRewards(rewards, "noAdsOfferPopup");
            }
        });
        
    });
    
    return this;
}

void NoAdsOfferPopup::onExit()
{
    for (const auto& item: m_onCloseCallbacks)
    {
        if(item != nullptr)
            item();
    }
    
    onExitPopup();
    
    Node::onExit();
}

void NoAdsOfferPopup::addOnBoughtCallback(std::function<void()> callback)
{
    m_onBoughtCallbacks.push_back(callback);
}

void NoAdsOfferPopup::addOnCloseCallback(std::function<void()> callback)
{
    m_onCloseCallbacks.push_back(callback);
}
