#include "EnergyPopup.h"
#include "../Menu/PopupLayer.h"
#include "../Menu/MenuNode.h"
#include "../UI/ButtonImage.h"
#include "../UI/HBLabel.h"
#include "../UI/UIButton.h"
#include "../AudioManager.h"
#include "../HapticFeedbackManager.h"
#include "../GameManager.h"
#include "../ConfigManager.h"
#include "../AdManager.h"
#include "../UI/TimerNode.hpp"


EnergyPopup* EnergyPopup::setUp(std::function<void()> onContinue, std::function<void()> onClosePopup)
{
    this->onContinue = onContinue;
        
    energyManager = EnergyManager::getInstance();
    
    for(auto* bar : GameManager::getInstance()->activeEnergyBars)
    {
        bar->updateLabel();
    }
    
    Node* scene = Node::create();
    addChild(scene, 10);
    
    const float width = GET_WIDTH * .9f * getSafeSceneScale();
    
    bool isOutOfEnergy = false;
    
    std::string titleString;

    if(energyManager->isOutOfEnergy())
    {
        isOutOfEnergy = true;
        titleString = "Out Of Gas";
    }
    else
    {
        titleString = "Helicopter Gas";
    }
    
    PopupNode::setUp(width, Size(968, 914),  "", FormattedStringBuilder(titleString));
    
    Sprite* popupBottomBG = Sprite::create("ui/common/container/Container_Popup_02_WithTimeContainer.png");
    popupBottomBG->setContentSize(Size(912, 686));
    popupBottomBG->setCenterRectNormalized(Rect(.25f, .7f, .5f, .05f));
    FIT_W(popupBottomBG, baseRect.size.width * .936f);
    popupBottomBG->setPositionY(baseRect.size.height * -.06f);
    addChild(popupBottomBG);
    
    
    TimerNodeConfig config;
    config.fontSize = width * .045f;
    config.startTime = energyManager->getSecondsLeftForNextFreeEnergy();
    config.timerLabelStrBuilder = FormattedStringBuilder("+1 Gas in: ");
    
    TimerNode* timerNode = TimerNode::create()->setup(config);
    timerNode->setPositionY(popupBottomBG->getBoundingBox().getMaxY() - width * .05f);
    scene->addChild(timerNode);

    if(!energyManager->isEnergyFull())
    {
        auto updateLabel = [=](){
        
            int time = energyManager->getSecondsLeftForNextFreeEnergy();
            
            if(time < 0 ||
               energyManager->isEnergyFull() ||
               (isOutOfEnergy && !energyManager->isOutOfEnergy()))
            {
                if(!GameManager::getInstance()->isShowingLoading())
                {
                    PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp(onContinue));
                }
                
                return;
            }
            
            timerNode->updateValueString(FormattedStringBuilder("%s", getRemainingTimeString(time).format().c_str()).ignoreLocalization());
            
        };
        
        updateLabel();
        
        timerNode->runAction(RepeatForever::create(Sequence::create({
            
            DelayTime::create(1.f),
            CallFunc::create([=](){
                
                updateLabel();
            })
            
        })));
    }
    else
    {
        timerNode->timerLabel->setString("Max Gas");
        timerNode->updateValueString(FormattedStringBuilder(""));
        
        timerNode->centerNodes();
    }
    
    //
   
    baseCloseButton->setPopupButtonPriority();
    baseCloseButton->onTap([=](){
        
        HapticFeedbackManager::getInstance()->play(MEDIUM);
        AudioManager::getInstance()->playEffect("click.wav", .5f);
        
        if(onClosePopup)
        {
            onClosePopup();
        }
        else
        {
            PopupLayer::current()->destroyPopup();
        }
    });
    
    
    //
    
    const int videoPurchasesLeft = energyManager->getNumberOfVideoPurchasesLeft();

    
    Node* area1 = Node::create();
    scene->addChild(area1);
    
    if(videoPurchasesLeft > 0)
    {
        area1->setPosition(-width * .235f, -width* .085f);
    }
    else
    {
        area1->setPosition(0.f, -width* .085f);
    }
    
    Sprite* icon1 = Sprite::create("ui/popup/energy/Icon_Gas_02.png");
    FIT_W(icon1, popupBottomBG->getBoundingBox().size.width * .4f);
    icon1->setPositionY(popupBottomBG->getBoundingBox().size.height * .13f);
    area1->addChild(icon1);
    
    icon1->runAction(RepeatForever::create(Sequence::create({
        
        EaseSineInOut::create(ScaleTo::create(1.f, icon1->getScale() * 0.95f)),
        EaseSineInOut::create(ScaleTo::create(1.f, icon1->getScale()))
        
    })));
    
    
    auto amountLabel1 = HBLabel::create(FormattedStringBuilder("x%i", ConfigManager::getInstance()->ENERGY_AMOUNT_DIAMOND).ignoreLocalization(), FONTS::USE_LOCALIZATION, width * .07f);
    amountLabel1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    amountLabel1->setAlignment(TextHAlignment::RIGHT);
    amountLabel1->setTextColor(Color4B(255, 227, 226, 255));
    amountLabel1->enableHBStyle(1, 0 , Color4B(145, 10, 10, 255));
    amountLabel1->setPosition(Vec2(icon1->getBoundingBox().getMaxX() - icon1->getBoundingBox().size.width * .2f, icon1->getBoundingBox().getMinY() + icon1->getBoundingBox().size.height * .1f));
    area1->addChild(amountLabel1);
    
    auto diamondButton = UIButton::create();
    diamondButton->setUpWithPrice(UIButtonType::GREEN, ConfigManager::getInstance()->ENERGY_PRICE, FormattedStringBuilder("").ignoreLocalization(), width * .33f, Size(378, 192));
    diamondButton->button->setPopupButtonPriority();
    diamondButton->button->animateTint();
    diamondButton->setPositionY(icon1->getBoundingBox().getMinY() - diamondButton->button->getBoundingBox().size.height * .6f);
    area1->addChild(diamondButton);
    
    diamondButton->button->onTap([=](){
        
        HapticFeedbackManager::getInstance()->play(HEAVY);
        
        const bool willContinue = onContinue && energyManager->isOutOfEnergy();
        
        energyManager->purchaseEnergyWithPrice([=](){
            
            if(willContinue)
            {
                stopAllActions();
                ButtonImage::disableAllTouches();
            }
            
            GameManager::getInstance()->showRewardCollectAnimation({ convertToWorldSpace(icon1->getPosition()) },
                                                                   { RewardData(REWARD_ENERGY, ConfigManager::getInstance()->ENERGY_AMOUNT_DIAMOND) },
                                                                   Vec2(-.5f, .5f),
                                                                   Vec2(-.5f, .5f),
                                                                   0.f, 0.f,
                                                                   [=](){
                
                EnergyPopup* popup = dynamic_cast<EnergyPopup*>(PopupLayer::current()->getCurrentPopup());
                
                if (popup)
                {
                    if(willContinue)
                    {
                        popup->runAction(RepeatForever::create(Sequence::create({
                            
                            DelayTime::create(.5f),
                            CallFunc::create([=](){
                                
                                EnergyPopup* popup = dynamic_cast<EnergyPopup*>(PopupLayer::current()->getCurrentPopup());
                                if (popup)
                                {
                                    PopupLayer::current()->destroyPopup();
                                    
                                    popup->onContinue();
                                }
                            })
                            
                        })));
                    }
                }
            });
            
            if(!willContinue)
            {
                PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp());
            }
            
        });
        
    });
    
    
    if(videoPurchasesLeft > 0)
    {
        Sprite* divider = Sprite::create("ui/common/container/Divider.png");
        divider->setCenterRectNormalized(Rect(.4f, .0f, .2f, 1.f));
        divider->setContentSize(Size(width * .5f, width * .005f));
        FIT_W(divider, width * .5f);
        divider->setRotation(90);
        divider->setPosition(popupBottomBG->getPosition() - Vec2(0, width * .02f));
        scene->addChild(divider);
        
        Node* area2 = Node::create();
        area2->setPosition(-area1->getPosition().x, area1->getPosition().y);
        scene->addChild(area2);
        
        Sprite* icon2 = Sprite::create("ui/popup/energy/Icon_Gas_01.png");
        FIT_W(icon2, icon1->getBoundingBox().size.width);
        icon2->setPosition(icon1->getPosition());
        area2->addChild(icon2);
        
        icon2->runAction(RepeatForever::create(Sequence::create({
            
            EaseSineInOut::create(ScaleTo::create(1.f, icon2->getScale() * 0.95f)),
            EaseSineInOut::create(ScaleTo::create(1.f, icon2->getScale()))
            
        })));
        
        auto amountLabel2 = HBLabel::createWithDarkStyle(FormattedStringBuilder("x%i", ConfigManager::getInstance()->ENERGY_AMOUNT_VIDEO).ignoreLocalization(), FONTS::USE_LOCALIZATION, amountLabel1->getFontSize());
        amountLabel2->setAnchorPoint(amountLabel1->getAnchorPoint());
        amountLabel2->setAlignment(amountLabel1->getTextAlignment());
        amountLabel2->setTextColor(amountLabel1->getTextColor());
        amountLabel2->enableHBStyle(1, 0 , Color4B(145, 10, 10, 255));
        amountLabel2->setPosition(amountLabel1->getPosition());
        area2->addChild(amountLabel2);
        
        Price videoPrice = Price(PRICE_VIDEO, 1);
        
        auto videoButton = UIButton::create();
        videoButton->setUpWithPrice(UIButtonType::YELLOW, videoPrice, FormattedStringBuilder("").ignoreLocalization(), width * .33f, Size(378, 192));
        videoButton->button->setPopupButtonPriority();
        videoButton->button->animateTint();
        area2->addChild(videoButton);
        
        videoButton->setPosition(diamondButton->getPosition());
        
        const auto iconPos = icon2->getPosition();
        
        videoButton->button->onTap([=](){
            
            ButtonImage::disableAllTouches();
            
            canBeClosedByOutsideTap = false;
            
            const std::string reason = "energy";
            
            AdManager::getInstance()->onRewardedShowAttempt(reason);
            
            if(!AdManager::getInstance()->canShowRewarded())
            {
                
                ButtonImage::enableAllTouches();
                
                canBeClosedByOutsideTap = true;
                
                AdManager::getInstance()->onRewardedNotReady(reason);
                
                AudioManager::getInstance()->playEffect("locked.wav");
                GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not ready yet!"), FONTS::USE_LOCALIZATION);
                return;
            }
            
            HapticFeedbackManager::getInstance()->play(HEAVY);
            
            AudioManager::getInstance()->playEffect("click.wav", .5f);
            
            AdManager::getInstance()->showRewarded(reason, "RewardVideo.HelicopterGas", [=](bool completed){
                
                ButtonImage::enableAllTouches();
                
                canBeClosedByOutsideTap = true;
                
                if(!completed)
                {
                    AudioManager::getInstance()->playEffect("locked.wav");
                    GameManager::getInstance()->showMessageText(Vec2(0,0), FormattedStringBuilder("Video not completed!"), FONTS::USE_LOCALIZATION);
                    return;
                }
                
                if (videoButton && videoButton->button)
                {
                    videoButton->updateButtonPath();
                }
                
                const bool willContinue = onContinue && energyManager->isOutOfEnergy();
                
                energyManager->purchaseEnergyWithVideo();
                
                AudioManager::getInstance()->playEffect("collect.wav");
                
                if(willContinue)
                {
                    stopAllActions();
                    ButtonImage::disableAllTouches();
                }
                
                GameManager::getInstance()->showRewardCollectAnimation({ convertToWorldSpace(iconPos) },
                                                                       { RewardData(REWARD_ENERGY, ConfigManager::getInstance()->ENERGY_AMOUNT_VIDEO) },
                                                                       Vec2(-.5f, .5f),
                                                                       Vec2(-.5f, .5f),
                                                                       0.f, 0.f,
                                                                       [=](){
                    EnergyPopup* popup = dynamic_cast<EnergyPopup*>(PopupLayer::current()->getCurrentPopup());
                    if (popup)
                    {
                        if(willContinue)
                        {
                            popup->runAction(RepeatForever::create(Sequence::create({
                                
                                DelayTime::create(.5f),
                                CallFunc::create([=](){
                                    EnergyPopup* popup = dynamic_cast<EnergyPopup*>(PopupLayer::current()->getCurrentPopup());
                                    if (popup)
                                    {
                                        PopupLayer::current()->destroyPopup();
                                        
                                        popup->onContinue();
                                    }
                                    else
                                    {
                                        ButtonImage::enableAllTouches();
                                    }
                                })
                                
                            })));
                        }
                    }
                    else
                    {
                        ButtonImage::enableAllTouches();
                    }
                });
                
                if(!willContinue)
                {
                    PopupLayer::current()->showPopup(ENERGY_POPUP, EnergyPopup::create()->setUp());
                }
            });
            
        });
    }
    
    
    return this;
}


void EnergyPopup::onExit()
{
    if (ButtonImage::isAllTouchesDisabled())
    {
        ButtonImage::enableAllTouches();
    }
    
    Node::onExit();
}
