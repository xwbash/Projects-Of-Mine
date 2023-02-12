#include "CreativeScene.h"
#include "CreativeStudioManager.h"
#include "GameManager.h"
#include "IntroScene.h"
#include "GameScene.h"
#include "UserSettings.h"
#include "UI/ThemeEditor/ThemeManager.h"
#include "HapticFeedbackManager.h"

USING_NS_CC;

Scene* CreativeScene::createScene()
{
    return CreativeScene::create();
}

bool CreativeScene::init()
{
    if (!Scene::init()) return false;
    
    GameManager::getInstance()->ACTIVE_SCENE_TYPE = CREATIVE_SCENE;
    
    auto contentNode = Node::create();
    contentNode->setPosition(GET_WIDTH*.5f,GET_HEIGHT*.5f);
    addChild(contentNode);
    
    std::vector<ButtonImage*> categoryButtons;
    
    auto backButton = ButtonImage::create();
    backButton->init("tiles/exitArrow.png");
    contentNode->addChild(backButton);
    backButton->setSceneButtonPriority();
    backButton->setRotation(-90);
    backButton->setPosition(GET_WIDTH * -.4f, GET_HEIGHT * .45f);
    backButton->setVisible(false);
    
    auto gobackLabel = HBLabel::createWithDarkStyle("Back", FREDOKA, backButton->getBoundingBox().size.width * .35f);
    contentNode->addChild(gobackLabel);
    gobackLabel->setPosition(backButton->getPositionX() + backButton->getBoundingBox().size.width * 1.25f, backButton->getPositionY());
    gobackLabel->setVisible(false);
    
    auto gameplayNode = Node::create();
    contentNode->addChild(gameplayNode);
    gameplayNode->setVisible(false);
    
    auto guardConfigNode = Node::create();
    contentNode->addChild(guardConfigNode);
    guardConfigNode->setVisible(false);
    
    auto playerNode = Node::create();
    contentNode->addChild(playerNode);
    playerNode->setVisible(false);
    
    auto uiNode = Node::create();
    contentNode->addChild(uiNode);
    uiNode->setVisible(false);
    
    auto levelGameplayNode = Node::create();
    contentNode->addChild(levelGameplayNode);
    levelGameplayNode->setVisible(false);
    
    auto selectCategoryLabel = HBLabel::createWithDarkStyle("SELECT CATEGORY", FREDOKA, GET_WIDTH * .06f);
    contentNode->addChild(selectCategoryLabel);
    selectCategoryLabel->setPositionY(GET_HEIGHT * .4f);
        
    ButtonImage* gameplayButton = ButtonImage::create()->init("empty.png");
    gameplayButton->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .07f);
    gameplayButton->mSprite->setScaleY(gameplayButton->mSprite->getScaleY()*.85f);
    gameplayButton->setSceneButtonPriority();
    contentNode->addChild(gameplayButton);
    categoryButtons.push_back(gameplayButton);

    
    auto gameplayLabel = HBLabel::createWithDarkStyle("Gameplay Config", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
    gameplayLabel->limitSize(Vec2(gameplayButton->mSprite->getBoundingBox().size.width*.9f, 0));
    gameplayButton->mNode->addChild(gameplayLabel);
    
    ButtonImage* guardConfigButton = ButtonImage::create()->init("empty.png");
    guardConfigButton->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .07f);
    guardConfigButton->mSprite->setScaleY(guardConfigButton->mSprite->getScaleY()*.85f);
    guardConfigButton->setSceneButtonPriority();
    contentNode->addChild(guardConfigButton);
    categoryButtons.push_back(guardConfigButton);
    
    auto guardConfigLabel = HBLabel::createWithDarkStyle("Guard Config", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
    guardConfigLabel->limitSize(Vec2(guardConfigButton->mSprite->getBoundingBox().size.width*.9f, 0));
    guardConfigButton->mNode->addChild(guardConfigLabel);
    
    ButtonImage* playerButton = ButtonImage::create()->init("empty.png");
    playerButton->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .07f);
    playerButton->mSprite->setScaleY(playerButton->mSprite->getScaleY()*.85f);
    playerButton->setSceneButtonPriority();
    contentNode->addChild(playerButton);
    categoryButtons.push_back(playerButton);
    
    auto playerLabel = HBLabel::createWithDarkStyle("Player Config", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
    playerLabel->limitSize(Vec2(playerButton->mSprite->getBoundingBox().size.width*.9f, 0));
    playerButton->mNode->addChild(playerLabel);
    
    ButtonImage* uiButton = ButtonImage::create()->init("empty.png");
    uiButton->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .07f);
    uiButton->mSprite->setScaleY(uiButton->mSprite->getScaleY()*.85f);
    uiButton->setSceneButtonPriority();
    contentNode->addChild(uiButton);
    categoryButtons.push_back(uiButton);

    
    auto uiLabel = HBLabel::createWithDarkStyle("UI Config", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
    uiLabel->limitSize(Vec2(uiButton->mSprite->getBoundingBox().size.width*.9f, 0));
    uiButton->mNode->addChild(uiLabel);
    
    ButtonImage* levelGameplayButton = ButtonImage::create()->init("empty.png");
    levelGameplayButton->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .07f);
    levelGameplayButton->mSprite->setScaleY(levelGameplayButton->mSprite->getScaleY()*.85f);
    levelGameplayButton->setSceneButtonPriority();
    contentNode->addChild(levelGameplayButton);
    categoryButtons.push_back(levelGameplayButton);

    
    auto levelGameplayLabel = HBLabel::createWithDarkStyle("Level Gameplay Config", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
    levelGameplayLabel->limitSize(Vec2(levelGameplayButton->mSprite->getBoundingBox().size.width*.9f, 0));
    levelGameplayButton->mNode->addChild(levelGameplayLabel);
    
    const float offset = GET_HEIGHT * .1f;
    
    for(int i = 0; i < categoryButtons.size(); i++)
    {
        categoryButtons[i]->setPosition(0, (i * categoryButtons[i]->mSprite->getBoundingBox().size.height * -1.25f) + offset);
    }
    
    
    //
    
    auto setVisibleCategoryButtons = ([categoryButtons](bool visibility)
    {
        for(int i = 0; i < categoryButtons.size(); i++)
        {
            categoryButtons[i]->setVisible(visibility);
        }
    });
    
    guardConfigButton->onTap([=](void*sender){
        uiNode->setVisible(false);
        playerNode->setVisible(false);
        guardConfigNode->setVisible(true);
        gameplayNode->setVisible(false);
        backButton->setVisible(true);
        levelGameplayNode->setVisible(false);
        gobackLabel->setVisible(true);
        selectCategoryLabel->setVisible(false);
        
        setVisibleCategoryButtons(false);
    });
    
    playerButton->onTap([=](void*sender){
        uiNode->setVisible(false);
        playerNode->setVisible(true);
        guardConfigNode->setVisible(false);
        gameplayNode->setVisible(false);
        backButton->setVisible(true);
        levelGameplayNode->setVisible(false);
        gobackLabel->setVisible(true);
        selectCategoryLabel->setVisible(false);
        
        setVisibleCategoryButtons(false);
    });
    
    uiButton->onTap([=](void*sender){
        uiNode->setVisible(true);
        playerNode->setVisible(false);
        guardConfigNode->setVisible(false);
        gameplayNode->setVisible(false);
        backButton->setVisible(true);
        gobackLabel->setVisible(true);
        levelGameplayNode->setVisible(false);
        selectCategoryLabel->setVisible(false);
        
        setVisibleCategoryButtons(false);
    });
    
    gameplayButton->onTap([=](void*sender){
        uiNode->setVisible(false);
        playerNode->setVisible(false);
        guardConfigNode->setVisible(false);
        gameplayNode->setVisible(true);
        backButton->setVisible(true);
        gobackLabel->setVisible(true);
        selectCategoryLabel->setVisible(false);
        levelGameplayNode->setVisible(false);

        
        setVisibleCategoryButtons(false);
    });
    
    backButton->onTap([=](void* sender)
    {
        uiNode->setVisible(false);
        gameplayNode->setVisible(false);
        playerNode->setVisible(false);
        guardConfigNode->setVisible(false);
        levelGameplayNode->setVisible(false);
        backButton->setVisible(false);
        gobackLabel->setVisible(false);
        selectCategoryLabel->setVisible(true);
        
        setVisibleCategoryButtons(true);
    });
    
    levelGameplayButton->onTap([=](void* sender)
    {
        uiNode->setVisible(false);
        playerNode->setVisible(false);
        guardConfigNode->setVisible(false);
        gameplayNode->setVisible(false);
        levelGameplayNode->setVisible(true);
        backButton->setVisible(true);
        gobackLabel->setVisible(true);
        selectCategoryLabel->setVisible(false);
        
        setVisibleCategoryButtons(false);
    });
    
    
    //
    
    auto closeButton = ButtonImage::create()->init("menu/ExitIcon.png");
    closeButton->fitH(GET_HEIGHT * .065f);
    closeButton->setSceneButtonPriority();
    closeButton->setPosition(GET_WIDTH*.5f - GET_HEIGHT*.05f,GET_HEIGHT*.5f - GET_HEIGHT*.05f);
    contentNode->addChild(closeButton);
    
    closeButton->onTap([](void* sender){
        
        QuestManager::getInstance()->newQuestAttached();
        Director::getInstance()->replaceScene(IntroScene::createScene());

    });
    
    
    int themeCount = ThemeManager::getInstance()->getThemeCount()+1;
    
    //
    
    //features.push_back({ "map", 1 + (int)GameManager::getInstance()->MAP_DESIGNS.size() - 3});
    features.push_back({ "number_of_guards", 50, GAMEPLAY_CATEGORY});
    features.push_back({ "no_damage_to_walls", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "is_glass_door_active", 2, LEVEL_CATEGORY});
    features.push_back({ "guard_types", 3, GUARD_CONFIG_CATEGORY});
    features.push_back({ "guard_power", 5, GUARD_CONFIG_CATEGORY});
    features.push_back({ "bullet_damage", 5, GUARD_CONFIG_CATEGORY});
    features.push_back({ "flashlight_distance", 15, GUARD_CONFIG_CATEGORY});
    features.push_back({ "flashlight_radius", 50, GUARD_CONFIG_CATEGORY});
    features.push_back({ "map_has_spawn_points", 2, LEVEL_CATEGORY});
    features.push_back({ "trap_explosive_time", 40, LEVEL_CATEGORY});
    features.push_back({ "always_bonus_mission", 2, LEVEL_CATEGORY});
    features.push_back({ "laser_distance", 40, LEVEL_CATEGORY});
    features.push_back({ "trap_explosion_radius", 5, LEVEL_CATEGORY});
    features.push_back({ "is_rocket_on", 2, LEVEL_CATEGORY});
    features.push_back({ "boss_mission_active", 2, LEVEL_CATEGORY});
    features.push_back({ "bonus_mission_active", 2, LEVEL_CATEGORY});
    features.push_back({ "fixed_center_camera", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "unlock_all_assassins", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "player_indicator", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "invisibility", 2, PLAYER_CATEGORY});
    features.push_back({ "is_assassin_immortal", 2, PLAYER_CATEGORY});
    features.push_back({ "is_guards_immortal", 2, GUARD_CONFIG_CATEGORY});
    features.push_back({ "enable_play_with_keyboard", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "use_custom_map_design", 2, LEVEL_CATEGORY});
    features.push_back({ "green_screen_floor", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "hide_counter", 2, UI_CATEGORY});
    features.push_back({ "disable_zoom", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "disable_theme_change", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "select_theme", themeCount, LEVEL_CATEGORY});
    features.push_back({ "disable_guard_lights", 2, GUARD_CONFIG_CATEGORY});
    features.push_back({ "disable_guard_attack", 2, GUARD_CONFIG_CATEGORY});
    features.push_back({ "guards_goto_assassin", 2, GUARD_CONFIG_CATEGORY});
    features.push_back({ "disable_path_node", 2, UI_CATEGORY});
    features.push_back({ "guard_speed", 20, GUARD_CONFIG_CATEGORY});
    features.push_back({ "dark_mode_active", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "ninja_control_key", 2, PLAYER_CATEGORY});
    features.push_back({ "activate_boss_mission", 2, GAMEPLAY_CATEGORY});
    features.push_back({ "hide_ui", 2, UI_CATEGORY});
    features.push_back({ "hide_kill_messages", 2, UI_CATEGORY});
    features.push_back({ "reset_data", 2, GAMEPLAY_CATEGORY});
    //
    
    //if(GET_CREATIVE_FATURE_VALUE("disable_guard_attack"))
    //if(GET_CREATIVE_FATURE_VALUE("disable_guard_lights"))
    
    
    for(int i = 0; i < features.size(); i++)
    {
        ButtonImage* button = ButtonImage::create()->init("empty.png");
        button->fitWH(GET_HEIGHT * .25f, GET_HEIGHT * .045f);
        button->mSprite->setScaleY( button->mSprite->getScaleY()*.85f);
        button->setSceneButtonPriority();
        
        switch (features[i].category)
        {
            case PLAYER_CATEGORY:
                playerNode->addChild(button);
                break;
            case GUARD_CONFIG_CATEGORY:
                guardConfigNode->addChild(button);
                break;
            case GAMEPLAY_CATEGORY:
                gameplayNode->addChild(button);
                break;
            case UI_CATEGORY:
                uiNode->addChild(button);
                break;
            case LEVEL_CATEGORY:
                levelGameplayNode->addChild(button);
                break;
        }
        
        buttons.push_back(button);
        
        const float offset = GET_HEIGHT * .4f;
        
        button->setPositionY(button->getParent()->getChildrenCount() * button->getBoundingBox().size.height * -1.25f + offset);
            
        auto label= HBLabel::createWithDarkStyle("", FONTS::FREDOKA, GET_HEIGHT * .085f * .35f);
        label->limitSize(Vec2(button->mSprite->getBoundingBox().size.width*.9f, 0));
        button->mNode->addChild(label);
        
        buttonLabels.push_back(label);
        
        updateButton(i);
        
        button->onTap([=](void* sender){
            
            if(i == features.size()-1)
            {
                UserSettings::getInstance()->eraseData();
                exit(0);
                return;
            }
            
            HapticFeedbackManager::getInstance()->play(SOFT);
            
            int value = GET_CREATIVE_FATURE_VALUE(features.at(i).name);
            
            value = (value+1) % features.at(i).numberOfValues;
            
            CreativeStudioManager::getInstance()->setFeatureValue(features.at(i).name,value);
            
            updateButton(i);
        });
    }
    
    
    return true;
}


void CreativeScene::updateButton(int index)
{
    auto button = buttons.at(index);
    auto feature = features.at(index);
    
    if(index == features.size()-1)
    {
        buttonLabels.at(index)->setString(feature.name);
        button->mSprite->setColor(Color3B(100, 20, 20));
        return;
    }
    
    int value = GET_CREATIVE_FATURE_VALUE(feature.name);
    std::string valueString = "";
    
    if(value == 0)
    {
        button->mSprite->setColor(Color3B(50, 50, 50));
        
        if(feature.numberOfValues == 2)
        {
            valueString = "OFF";
        }
        else
        {
            valueString = "-";
        }
    }
    else
    {
        button->mSprite->setColor(Color3B(0, 153,204));
        
        if(feature.numberOfValues == 2)
        {
            valueString = "ON";
        }
        else
        {
            if(feature.name == "number_of_guards")
            {
                valueString = std::to_string(value * 2);
            }
            else if(feature.name == "flashlight_radius")
            {
                valueString = std::to_string(value * 5);
            }
            else if(feature.name == "guard_types")
            {
                if(value == 0)
                {
                    valueString = "-";
                }
                else if(value == 1)
                {
                    valueString = "all regular";
                }
                else
                {
                    valueString = "all shield";
                }
                
            }
            else if(feature.name == "bullet_damage")
            {
                if(value == 0)
                {
                    valueString = "-";
                }
                else if(value == 1)
                {
                    valueString = "doom";
                }
                else if(value == 2)
                {
                    valueString = "hard";
                }
                else if(value == 3)
                {
                    valueString = "normal";
                }
                else if(value == 4)
                {
                    valueString = "easy";
                }
            }
            else if(feature.name == "guard_power")
            {
                if(value == 0)
                {
                    valueString = "-";
                }
                else if(value == 1)
                {
                    valueString = "easy";
                }
                else if(value == 2)
                {
                    valueString = "normal";
                }
                else if(value == 3)
                {
                    valueString = "hard";
                }
                else
                {
                    valueString = "doom";
                }
            }
            else if(feature.name == "select_theme")
            {
                valueString = ThemeManager::getInstance()->getThemeName(value-1);
            }
            else
            {
                valueString = std::to_string(value);
            }
        }
    }
    
    buttonLabels.at(index)->setString(StringUtils::format("%s: %s",feature.name.c_str(),valueString.c_str()));
}
