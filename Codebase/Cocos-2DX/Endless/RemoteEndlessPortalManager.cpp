//
//  RemoteEndlessPortalManager.cpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#include "RemoteEndlessPortalManager.hpp"

#include "LiveOpsManager.h"
#include "UserSettings.h"
#include "TutorialManager.hpp"
#include "ConfigManager.h"
#include "DataEventManager.h"
#include "RapidJsonExtension.h"
#include "Menu/PopupLayer.h"
#include "Menu/MenuNode.h"
#include "Popups/EndlessPortal/EndlessPortalPopup.hpp"
#include "Popups/EndlessPortal/EndlessPortalClaimRewardPopup.hpp"
#include "FormulaParser/MathFormula.hpp"
#include "RemoteAssetsManager.hpp"
#include "NicknameGeneratorManager.hpp"
#include "Popups/EndlessPortal/EndlessPortalJoinPopup.hpp"
#include "StorageManager.h"
#include "TimeManager.h"
#include "LeaderboardManager.hpp"
#include "Popups/ThreeStepInfoPopup.hpp"
#include "Gameplay/QuestManager.h"

#define MAX_REACHED_STAGE_KEY "ENDLESS_MODE_MAX_REACHED_STAGE_KEY"


static bool isInitialized = false;

void RemoteEndlessPortalManager::init()
{
    isInitialized = true;
    defaultJson =
    R"(
    {
      "title1Loc": "ep_title1",
      "eventNameLoc": "ep_event_name",
      "playButtonTextLoc": "ep_play_button_text",
      "playerCount": 20,
      "claimPeriod": 48,
      "targetScoreFormula": "1000 - x",
      "reviveAmount": "2",
      "battlePassTokens": [
        3,
        5,
        1250
      ],
      "gameDifficulty": [
        -1,
        2000,
        10,
        -2
      ],
      "rewards": [
        {
          "smallKey": 5,
          "bigKey": 4,
          "crimsonStarKey": 3,
          "heroKey": 2,
          "superHeroKey": 1,
          "gold": 500,
          "reviveToken": 1,
          "gem": 10,
          "shuffleToken": 3,
          "hiddenBlueprint": 20,
          "experience": 1000
        },
        {
          "bigKey": 1
        },
        {
          "gold": 5,
          "gem": 50
        },
        {
          "gold": 5
        },
        {
          "gold": 5
        },
        {
          "gold": 5
        }
      ],
      "curveSettings": [
        "EaseInSine",
        "EaseOutSine",
        "EaseInOutSine",
        "EaseInQuad",
        "EaseOutQuad",
        "EaseInOutQuad",
        "EaseInCubic",
        "EaseOutCubic",
        "EaseInOutCubic",
        "EaseInQuart",
        "EaseOutQuart",
        "EaseInOutQuart",
        "EaseInQuint",
        "EaseOutQuint",
        "EaseInOutQuint",
        "EaseInExpo",
        "EaseOutExpo",
        "EaseInOutExpo",
        "EaseInCirc",
        "EaseOutCirc",
        "EaseInOutCirc"
      ],
      "iconSmall": "default",
      "iconLarge": "default",
      "rewardBg": "default",
      "mainMenuIcon": "default",
      "mainMenuIconWithTimer": "default",
      "joinBg": "default",
      "chest_1": "default",
      "chest_2": "default",
      "chest_3": "default",
      "chestOther": "default",
      "collectableNameLoc": "ep_collectable_name"
    }
    )";
    
    updateData();
}
 
void RemoteEndlessPortalManager::updateData()
{
    endlessConfigs.clear();
    
    if(LiveOpsManager::getInstance()->isLiveOpsReady)
    {
        setEndlessModeConfigs();
        
        loadPlayerData();
        
        syncAssetBuffer();
    }
    else
    {
        Director::getInstance()->getScheduler()->schedule([=](float) {
            
            CCLOG("endlessMode - updating data...");
            
            updateData();
            
        }, this, updateInterval, 0, 0.f, false, "endlessModeUpdateData");
    }
}

bool RemoteEndlessPortalManager::readData(EndlessPortalConfig &updatedData, rapidjson::Document &document)
{
    if(document.IsNull()) return false;
    
    if(!document.HasMember(TITLE_1_KEY)                 ||
       !document.HasMember(EVENT_NAME)                 ||
       !document.HasMember(PLAY_BUTTON_TEXT)                 ||
       !document.HasMember(REWARD_CLAIM_PERIOD_KEY)     ||
       !document.HasMember(REVIVE_AMOUNT)     ||
       !document.HasMember(BATTLE_PASS_TOKENS)     ||
       !document.HasMember(REWARDS_KEY)                 ||
       !document.HasMember(PLAYER_COUNT_KEY)            ||
       !document.HasMember(TARGET_SCORE_FORMULA_KEY)    ||
       !document.HasMember(ICON_SMALL_KEY)              ||
       !document.HasMember(GAME_DIFFICULTY)              ||
       !document.HasMember(ICON_LARGE_KEY)              ||
       !document.HasMember(REWARD_BG_KEY)               ||
       !document.HasMember(COLLECTABLE_NAME_KEY)        ||
       !document.HasMember(CURVE_SETTINGS_KEY))
    {
        return false;
    }
    
    if(document.HasMember(TITLE_1_KEY) && document[TITLE_1_KEY].IsString())
    {
        updatedData.title1Format = FormattedStringBuilder(document[TITLE_1_KEY].GetString()).ignoreSave();
    }
    
    if(document.HasMember(EVENT_NAME) && document[EVENT_NAME].IsString())
    {
       updatedData.eventNameFormat = FormattedStringBuilder(document[EVENT_NAME].GetString()).ignoreSave();
    }
    
    if(document.HasMember(PLAY_BUTTON_TEXT) && document[PLAY_BUTTON_TEXT].IsString())
    {
        updatedData.playButtonTextFormat = FormattedStringBuilder(document[PLAY_BUTTON_TEXT].GetString()).ignoreSave();
    }
    
    if(document.HasMember(PLAYER_COUNT_KEY) && document[PLAYER_COUNT_KEY].IsInt())
    {
        updatedData.playerCount = document[PLAYER_COUNT_KEY].GetInt();
    }
    
    if(document.HasMember(REVIVE_AMOUNT) && document[REVIVE_AMOUNT].IsInt())
    {
        updatedData.reviveAmount = document[REVIVE_AMOUNT].GetInt();
    }
    
    if(document.HasMember(REWARD_CLAIM_PERIOD_KEY) && document[REWARD_CLAIM_PERIOD_KEY].IsInt())
    {
        updatedData.rewardClaimPeriodInHours = document[REWARD_CLAIM_PERIOD_KEY].GetInt();
    }
    
    if(document.HasMember(REWARDS_KEY) && document[REWARDS_KEY].IsArray())
    {
        //
        
        updatedData.rewardsJson = RapidJsonExtension::getJsonAsString(document[REWARDS_KEY]);
        
        //
        
        auto rewardsArray = document[REWARDS_KEY].GetArray();
        
        for (auto& reward : rewardsArray)
        {
            auto rewards = RemoteRewardDataManager::getInstance()->getRewardDatas( RemoteRewardDataManager::getInstance()->deserializeRewardJson(reward.GetObject()));
            
            if(!rewards.empty() && rewards.size() > 0)
            {
                updatedData.rewards.push_back(rewards);
            }
        }
    }
    
    if(document.HasMember(CURVE_SETTINGS_KEY) && document[CURVE_SETTINGS_KEY].IsArray())
    {
        auto curveArray = document[CURVE_SETTINGS_KEY].GetArray();
        
        for (auto& curve : curveArray)
        {
            updatedData.curveSettings.push_back(curve.GetString());
            updatedData.curveEasings.push_back(getEasingFunction( getEnumFromString(curve.GetString()) ));
        }
    }
    
    
    if(document.HasMember(BATTLE_PASS_TOKENS) && document[BATTLE_PASS_TOKENS].IsArray())
    {
        auto bpTokenArray = document[BATTLE_PASS_TOKENS].GetArray();
        for(auto& bpToken : bpTokenArray)
        {
            if(bpToken.IsInt())
            {
                updatedData.battlePassTokens.push_back(bpToken.GetInt());
            }
        }
    }
    
    if(document.HasMember(GAME_DIFFICULTY) && document[GAME_DIFFICULTY].IsArray())
    {
        auto gameDifficultyArray = document[GAME_DIFFICULTY].GetArray();
        for(auto& gameDifficulty : gameDifficultyArray)
        {
            if(gameDifficulty.IsInt())
            {
                updatedData.gameDifficulty.push_back(gameDifficulty.GetInt());
            }
        }
    }
    
    if(document.HasMember(TARGET_SCORE_FORMULA_KEY) && document[TARGET_SCORE_FORMULA_KEY].IsString())
    {
       updatedData.targetScoreFormula = document[TARGET_SCORE_FORMULA_KEY].GetString();
    }
    
    if(document.HasMember(ICON_SMALL_KEY) && document[ICON_SMALL_KEY].IsString())
    {
       updatedData.iconSmall = document[ICON_SMALL_KEY].GetString();
    }
    
    if(document.HasMember(ICON_LARGE_KEY) && document[ICON_LARGE_KEY].IsString())
    {
       updatedData.iconLarge = document[ICON_LARGE_KEY].GetString();
    }
    
    if(document.HasMember(REWARD_BG_KEY) && document[REWARD_BG_KEY].IsString())
    {
       updatedData.rewardBg = document[REWARD_BG_KEY].GetString();
    }
    
    if(document.HasMember(MAIN_MENU_ICON_KEY) && document[MAIN_MENU_ICON_KEY].IsString())
    {
       updatedData.mainMenuIcon = document[MAIN_MENU_ICON_KEY].GetString();
    }
    
    if(document.HasMember(MAIN_MENU_ICON_TIMER_KEY) && document[MAIN_MENU_ICON_TIMER_KEY].IsString())
    {
       updatedData.mainMenuIconWithTimer = document[MAIN_MENU_ICON_TIMER_KEY].GetString();
    }
    
    if(document.HasMember(JOIN_BG_KEY) && document[JOIN_BG_KEY].IsString())
    {
       updatedData.joinBg = document[JOIN_BG_KEY].GetString();
    }
    
    if(document.HasMember(CHEST_1_KEY) && document[CHEST_1_KEY].IsString())
    {
       updatedData.chest_1 = document[CHEST_1_KEY].GetString();
    }
    
    if(document.HasMember(CHEST_2_KEY) && document[CHEST_2_KEY].IsString())
    {
       updatedData.chest_2 = document[CHEST_2_KEY].GetString();
    }
    
    if(document.HasMember(CHEST_3_KEY) && document[CHEST_3_KEY].IsString())
    {
       updatedData.chest_3 = document[CHEST_3_KEY].GetString();
    }
    
    if(document.HasMember(CHEST_OTHER_KEY) && document[CHEST_OTHER_KEY].IsString())
    {
       updatedData.chestOther = document[CHEST_OTHER_KEY].GetString();
    }
    
    if(document.HasMember(COLLECTABLE_NAME_KEY) && document[COLLECTABLE_NAME_KEY].IsString())
    {
       updatedData.collectableNameFormat = FormattedStringBuilder(document[COLLECTABLE_NAME_KEY].GetString()).ignoreSave();
    }
    
    return true;
}

void RemoteEndlessPortalManager::setEndlessModeConfigs()
{
    isEndlessModeAvailable = false;
    
    for(auto endlessID : ConfigManager::getInstance()->ENDLESS_MODE_TEMPLATE_IDS)
    {
        auto* event = LiveOpsManager::getInstance()->getActiveEventByTemplateId(endlessID);
        
        EndlessPortalConfig config;
        config.endlessID = endlessID;
        
        if(event)
        {
            config.eventID = event->eventId;
            config.endlessName = event->name;
            config.endlessStartDate = event->startDateSec;
            config.endlessEndDate = event->endDateSec;
            
            rapidjson::Document document;
            document.Parse(event->config.c_str());
            
            readData(config, document);
            
            endlessConfigs.push_back(config);
            

            isEndlessModeAvailable = true;
        }
    }
    
    if(ADMIN_PLAYER && endlessConfigs.size() <= 0 && ConfigManager::getInstance()->ENABLE_ENDLESS_MODE > 1)
    {
        EndlessPortalConfig config;
        config.endlessID = "default json";
        config.eventID = "event_temp";
        config.endlessName = "name_temp";
        config.endlessStartDate = getCurrentTime() - 7200;
        config.endlessEndDate = getCurrentTime() + 3600;
        
        rapidjson::Document document;
        document.Parse(defaultJson.c_str());
        
        readData(config, document);
        
        endlessConfigs.push_back(config);
        
        isEndlessModeAvailable = true;
    }
}

bool RemoteEndlessPortalManager::isSystemActivated()
{
    if(ConfigManager::getInstance()->ENABLE_ENDLESS_MODE <= 0 ||
       !LiveOpsManager::getInstance()->isLiveOpsReady ||
       (!isEndlessModeAvailable && !isInClaimState()) ||
       !TutorialManager::checkConditionsMet(ConfigManager::getInstance()->ENDLESS_PORTAL_ENABLE_CONDITIONS))
    {
        return false;
    }
    
    return true;
}

bool RemoteEndlessPortalManager::isLocked()
{
    return TutorialManager::checkConditionsLocked(ConfigManager::getInstance()->ENDLESS_PORTAL_ENABLE_CONDITIONS , "Endless_Mode_Key");
}

void RemoteEndlessPortalManager::onGameFinished(int stageNo)  noexcept
{
    setStageCount(stageNo);
    sendEvent("endless_mode_mission_completed",
    {
        {"reached_endless_mode_stage" , Value(stageNo) },
        {"reached_endless_mode_max_stage" , Value(getMaxReachedStage(stageNo)) },
        {"result" , Value("success") },
        {"revive_count" , Value(QUEST_DATA->resurrections) }
    });
}

void RemoteEndlessPortalManager::removeExpiredEndlessModes(std::string endlessID)
{
    for(int i = 0; i < (int)endlessConfigs.size(); ++i)
    {
        if(endlessConfigs[i].endlessID == endlessID)
        {
            endlessConfigs.erase(endlessConfigs.begin() + i);
            break;
        }
    }
    
    isEndlessModeAvailable = ((int)endlessConfigs.size() <= 0);
}



// MARK: Load / Save

void RemoteEndlessPortalManager::loadPlayerData()
{
    ValueMap endlessPlayerData = FileUtils::getInstance()->getValueMapFromFile(FileUtils::getInstance()->getWritablePath() + PLAYER_ENDLESS_MODE_DATA_SAVE_KEY);
    
    if(!endlessPlayerData.empty())
    {
        savedEndlessConfig.fromValueMap(endlessPlayerData);
        
        if((int)endlessConfigs.size() > 0 && (savedEndlessConfig.eventID.empty() || savedEndlessConfig.endlessID.empty()))
        {
            savedEndlessConfig.eventID = endlessConfigs[0].eventID;
            savedEndlessConfig.endlessID = endlessConfigs[0].endlessID;
        }
        
        double currentTime = getCurrentTime();
        
        if(currentTime > savedEndlessConfig.endlessEndDate + savedEndlessConfig.rewardClaimPeriodInHours * 3600)
        {
            clearPlayerData();
            return;
        }
        
        hasSavedData = true;
        
        CCLOG("RemoteEndlessPortalManager - has player data");
        
        if(!endlessConfigs.empty())
        {
            for(auto& endlessConfig : endlessConfigs)
            {
                if(endlessConfig.eventID == savedEndlessConfig.eventID)
                {
                    savedEndlessConfig.endlessName = endlessConfig.endlessName;
                    savedEndlessConfig.endlessStartDate = endlessConfig.endlessStartDate;
                    savedEndlessConfig.endlessEndDate = endlessConfig.endlessEndDate;
                    savedEndlessConfig.title1Format = endlessConfig.title1Format;
                    savedEndlessConfig.rewardClaimPeriodInHours = endlessConfig.rewardClaimPeriodInHours;
                    savedEndlessConfig.rewards = endlessConfig.rewards;
                    savedEndlessConfig.rewardsJson = endlessConfig.rewardsJson;
                    savedEndlessConfig.curveEasings = endlessConfig.curveEasings;
                    savedEndlessConfig.curveSettings = endlessConfig.curveSettings;
                    savedEndlessConfig.targetScoreFormula = endlessConfig.targetScoreFormula;
                    savedEndlessConfig.iconSmall = endlessConfig.iconSmall;
                    savedEndlessConfig.iconLarge = endlessConfig.iconLarge;
                    savedEndlessConfig.rewardBg = endlessConfig.rewardBg;
                    savedEndlessConfig.collectableNameFormat = endlessConfig.collectableNameFormat;
                    
                    savePlayerData(false);
                    break;
                }
            }
        }
    }
    else
    {
        hasSavedData = false;
    }
    
    CCLOG("RemoteEndlessPortalManager - loaded data");
}

int RemoteEndlessPortalManager::getBattlePassToken(int currentStage)
{
    if(currentStage <= 0) return 0;
    
    if(!endlessConfigs.empty() && !endlessConfigs[0].battlePassTokens.empty())
    {
        int perStage = endlessConfigs[0].battlePassTokens[0];
        int battlePassAmount = endlessConfigs[0].battlePassTokens[1];
        float factor = endlessConfigs[0].battlePassTokens[2] * 0.001f;
        float stagePrizeAmount = (currentStage / (float)perStage);
    
        if(currentStage == perStage)
        {
            return battlePassAmount;
        }
        else if(currentStage%perStage == 0)
        {
            return ceil(((battlePassAmount * factor) - battlePassAmount) *  stagePrizeAmount + battlePassAmount);
        }
    }
    
    return 0;
}


std::vector<int> RemoteEndlessPortalManager::getDifficulty()
{
    std::vector<int> difficultyVector;
    
    if(!endlessConfigs.empty() && !endlessConfigs[0].gameDifficulty.empty())
    {
        return endlessConfigs[0].gameDifficulty;
    }
    
    return difficultyVector;
}

void RemoteEndlessPortalManager::savePlayerData(bool syncToRemote)
{
    auto valueMap = savedEndlessConfig.toValueMap();
    
    FileUtils::getInstance()->writeValueMapToFile(valueMap, FileUtils::getInstance()->getWritablePath() + PLAYER_ENDLESS_MODE_DATA_SAVE_KEY);
    
    hasSavedData = true;
    
    //
    
    if(syncToRemote)
    {
        this->syncToRemote();
    }
    
    //
    
    CCLOG("RemoteEndlessPortalManager - saved data");
}

void RemoteEndlessPortalManager::clearPlayerData()
{
    FileUtils::getInstance()->removeFile(FileUtils::getInstance()->getWritablePath() + PLAYER_ENDLESS_MODE_DATA_SAVE_KEY);
    
    savedEndlessConfig = {};
    hasSavedData = false;
    
    CCLOG("RemoteEndlessPortalManager - cleared data");
}

// MARK: Popup Control

void RemoteEndlessPortalManager::endlessButtonClicked()
{
    if(hasSavedData)
    {
        if(isEndlessModeAvailable && !isInClaimState())
        {
            showPopup();
        }
        else if(isInClaimState())
        {
            showClaimPopup();
        }
    }
    else if(isEndlessModeAvailable)
    {
        showJoinPopup();
    }

}

void RemoteEndlessPortalManager::showPopup()
{
    if((int)endlessConfigs.size() > 0 && (int)endlessConfigs[0].rewards.size() > 0 && getRemainingTimeInSeconds(endlessConfigs[0].endlessID, endlessConfigs[0].eventID) > 0)
    {
        PopupLayer::current()->showPopup(ENDLESS_PORTAL_POPUP, EndlessPortalPopup::create()->setup(&savedEndlessConfig));
    }
}


void RemoteEndlessPortalManager::showClaimPopup()
{
    CCLOG("RemoteEndlessPortalManager - Show Claim Popup");
    std::vector<int> scores;
    for(auto npc : savedEndlessConfig.playerData.npcList)
    {
        scores.push_back(getNpcScore(&npc));
    }
    
    std::sort(scores.begin(), scores.end(), [this](const int& a, const int& b) {

        return a > b;
    });
    
    int playerPlacement = (int)scores.size();
    for(int i = 0; i < (int)scores.size(); ++i)
    {
        if(savedEndlessConfig.playerData.stageCount >= scores.at(i))
        {
            playerPlacement = i;
            break;
        }
    }
    
    std::vector<RewardData> rewards;
    if(playerPlacement < (int)savedEndlessConfig.rewards.size())
    {
        rewards = savedEndlessConfig.rewards.at(playerPlacement);
    }
    
    std::vector<RewardData> actualRewards;
    if(!rewards.empty())
    {
        // claim rewards
        for(int i = 0; i < (int)rewards.size(); ++i)
        {
            auto _rewardList = UserSettings::getInstance()->collectReward(rewards.at(i), "endless_mode");
            actualRewards.insert(actualRewards.end(), _rewardList.begin(), _rewardList.end());
        }
    }
    
    int playerScore = savedEndlessConfig.playerData.stageCount;
    
    //
    sendEvent("endless_mode_completed",
    {
        {"endless_mode_name" , Value(savedEndlessConfig.endlessName) },
        {"reached_endless_mode_stage" , Value(playerScore) },
        {"finish_rank" , Value(playerPlacement + 1) },
        {"result" , Value("fail") },
        {"revive_count" , Value(QUEST_DATA->resurrections) }
    });
    //
    
    clearPlayerData();
    
    updateData();
    
    unscheduleNotification("EndlessPortalManagerStart");
    
    PopupLayer::current()->showPopup(ENDLESS_PORTAL_CLAIM_REWARD_POPUP, EndlessPortalClaimRewardPopup::create()->setup(playerPlacement, playerScore, actualRewards));
    
    if(MenuNode::current())
    {
        MenuNode::current()->playButton->hideEndlessPortalButton();
    }
}

bool RemoteEndlessPortalManager::checkShowPopup()
{
    if(!isSystemActivated())
    {
        return false;
    }
    
    if(!isJoinedEndless() && !isInClaimState())
    {
        const std::string endlessJoinPopupSaveKey = "endlessModeJoinPopupSaveKey";
        std::string eventID = HBUserDefaults::getInstance()->getStringForKey(endlessJoinPopupSaveKey.c_str(), "");
        
        if(endlessConfigs[0].eventID != eventID)
        {
            HBUserDefaults::getInstance()->setStringForKey(endlessJoinPopupSaveKey.c_str(), endlessConfigs[0].eventID);
            
            showJoinPopup();
            
            return true;
        }
    }
    else if(isJoinedEndless() && isInClaimState())
    {
        showClaimPopup();
        return true;
    }

    return false;
}

int RemoteEndlessPortalManager::getNotificationCount()
{
    if(isSystemActivated() && isInClaimState())
    {
        return -1099;
    }
    
    return 0;
}

// MARK: Configs

EndlessPortalConfig* RemoteEndlessPortalManager::getEndlessById(std::string endlessID)
{
    for(int i = 0; i < (int)endlessConfigs.size(); ++i)
    {
        if(endlessConfigs[i].endlessID == endlessID)
        {
            return &endlessConfigs[i];
        }
    }
    
    return nullptr;
}

void RemoteEndlessPortalManager::showJoinPopup()
{
    if((int)endlessConfigs.size() > 0 && (int)endlessConfigs[0].rewards.size() > 0 && getRemainingTimeInSeconds(endlessConfigs[0].endlessID, endlessConfigs[0].eventID) > 0)
    {
        unscheduleNotification("EndlessPortalManagerStart");

        FormattedStringBuilder infoTextFormat = FormattedStringBuilder("Play the Endless Mode to climb up the leaderboard and earn extra rewards!");
        
        PopupLayer::current()->showPopup(ENDLESS_PORTAL_JOIN_POPUP, EndlessPortalJoinPopup::create()->setup(infoTextFormat));
    }
}

std::vector<RewardData> RemoteEndlessPortalManager::getEndlessRewardsAtIndex(std::string endlessID, int index)
{
    std::vector<RewardData> rewards;
    auto* config = getEndlessById(endlessID);
    
    if(config && (int)config->rewards.size() > index)
    {
        return config->rewards.at(index);
    }
    
    return rewards;
}

void RemoteEndlessPortalManager::joinEndless(bool silent)
{
    std::vector<EndlessPortalNpcData> oldNicknamePlayerData = savedEndlessConfig.playerData.npcList;
    
    savedEndlessConfig = endlessConfigs[0];
    
    savedEndlessConfig.playerData.endlessID = endlessConfigs[0].endlessID;
    
    
    if(silent && !oldNicknamePlayerData.empty())
    {
        savedEndlessConfig.playerData.npcList = oldNicknamePlayerData;
        savePlayerData(false);
    }
    else
    {
        savedEndlessConfig.playerData.npcList = generateNPCs(savedEndlessConfig.playerCount - 1);
        savePlayerData(false);
    }
    
    //
    
    if(!silent)
    {
        //
        
        savePlayerData();
        
        sendEvent("endless_mode_join",
        {
            {"endless_mode_name" , Value(savedEndlessConfig.endlessName) },
            {"join_state" , Value(1) }
        });
        
        //
        
        showEndlessInfoPopup(true);
    }
}

void RemoteEndlessPortalManager::showEndlessInfoPopup(bool fromJoin)
{
    ThreeStepConfig config;
//    config.title = savedEndlessConfig.title1Format.format();
    config.titleColor = Color4B(255, 255, 200, 255);
    config.titleEffectColor = Color4B(245, 209, 67, 255);
    
    config.iconPaths = {
        "ui/popup/endlessPortalPopup/Portal_endless.png",
        "ui/popup/endlessPortalPopup/Icon_Tournament.png",
        "ui/popup/endlessPortalPopup/Chest.png"
    };
    config.infoTexts = {
        "Play\nEndless Mode",
        "Beat more Stages \nthan Others",
        "Earn\nRewards"
    };
    
    config.buttonLabel = "Continue";
    config.onButtonClicked = [=](){
        if(fromJoin)
        {
            showPopup();
        }
    };
    
    PopupLayer::current()->showPopup(THREE_STEP_INFO_POPUP, ThreeStepInfoPopup::create()->setup(config));
}

std::string RemoteEndlessPortalManager::getTournamentID()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].eventID != "default")
    {
        return endlessConfigs[0].eventID;
    }
    
    if(!savedEndlessConfig.eventID.empty())
    {
        return savedEndlessConfig.eventID;
    }
    
    return "";
}

std::string RemoteEndlessPortalManager::getJoinBgPath()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].joinBg != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(endlessConfigs[0].joinBg);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/endlessPortalPopup/Background_EventStarted_Portal.png";
}

bool RemoteEndlessPortalManager::isJoinedEndless()
{
    return !savedEndlessConfig.endlessID.empty();
}

bool RemoteEndlessPortalManager::isInClaimState()
{
    if(!hasSavedData)
    {
        return false;
    }
    
    double currentTime = getCurrentTime();
    
    bool state = (currentTime > savedEndlessConfig.endlessEndDate) && (currentTime - savedEndlessConfig.endlessEndDate <= savedEndlessConfig.rewardClaimPeriodInHours * 3600);
    
    if(currentTime > savedEndlessConfig.endlessEndDate + savedEndlessConfig.rewardClaimPeriodInHours * 3600)
    {
        // Exceeded Claim Time
        clearPlayerData();
    }
    
    return state;
}

int RemoteEndlessPortalManager::getCalculatedTargetScore()
{
    if(savedEndlessConfig.targetScoreFormula.empty())
    {
        return 1000;
    }
    
    int missionNo = UserSettings::getInstance()->getMissionNo();
    
    MathFormula formula;
    formula.createVariable('x');
    formula.updateVariable('x', missionNo);
    
    return (int)(formula.calculate(savedEndlessConfig.targetScoreFormula));
}

std::vector<EndlessPortalNpcData> RemoteEndlessPortalManager::generateNPCs(int count)
{
    int maxTargetScore = getCalculatedTargetScore();
    
    std::vector<EndlessPortalNpcData> npcList;
    for(int i = 0; i < count; ++i)
    {
        EndlessPortalNpcData npc;
        npc.curveEasing = savedEndlessConfig.curveEasings.at(i % ((int)savedEndlessConfig.curveEasings.size()));
        npc.curveID = savedEndlessConfig.curveSettings.at(i % ((int)savedEndlessConfig.curveSettings.size()));
        npc.nickname = NicknameGeneratorManager::getInstance()->chooseNickname();
        npc.targetScore = cocos2d::random(0, maxTargetScore);
        
        npcList.push_back(npc);
    }
    
    return npcList;
}

int RemoteEndlessPortalManager::getNpcScore(EndlessPortalNpcData* npc)
{
    float endlessTime = savedEndlessConfig.endlessEndDate - savedEndlessConfig.endlessStartDate;
    float currentDiff = getCurrentTime() - savedEndlessConfig.endlessStartDate;
    
    float ratio = clampf(currentDiff / endlessTime, 0.f, 1.f);
    
    float val = npc->curveEasing(ratio);
    
    return (int)(npc->targetScore * val);
}

// MARK: Remaining Time

int RemoteEndlessPortalManager::getRemainingTimeInSeconds(std::string endlessID, std::string eventId)
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_MAC
    int remainingTime = LiveOpsManager::getInstance()->getRemainingTimeInSeconds(endlessID, eventId);
    if(remainingTime <= 0)
    {
        removeExpiredEndlessModes(endlessID);
    }
    return remainingTime;
#else
//    --remainingTimeTest;
//    return remainingTimeTest;
    return endlessConfigs[0].endlessEndDate - getCurrentTime();
#endif
}

int RemoteEndlessPortalManager::getRemainingTimeInSeconds()
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_MAC
    if(endlessConfigs.size() > 0)
    {
        int remainingTime = LiveOpsManager::getInstance()->getRemainingTimeInSeconds(endlessConfigs[0].endlessID, endlessConfigs[0].eventID);
        if(remainingTime <= 0)
        {
            removeExpiredEndlessModes(endlessConfigs[0].endlessID);
        }
        return remainingTime;
    }
    else
    {
        isEndlessModeAvailable = false;
        return 0;
    }
#else
    --remainingTimeTest;
    isEndlessModeAvailable = remainingTimeTest > 0;
    return remainingTimeTest;
#endif
}

//

std::string RemoteEndlessPortalManager::getEndlessLargeIconPath()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].iconLarge != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(endlessConfigs[0].iconLarge);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/endlessPortalPopup/Icon_PortalCurrency.png";
}

std::string RemoteEndlessPortalManager::getEndlessSmallIconPath()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].iconSmall != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(endlessConfigs[0].iconSmall);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/endlessPortalPopup/Icon_PortalCurrency.png";
}

std::string RemoteEndlessPortalManager::getMainMenuIconPath()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].mainMenuIcon != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(endlessConfigs[0].mainMenuIcon);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/endlessPortalPopup/Icon_Portal.png";
}

std::string RemoteEndlessPortalManager::getMainMenuIconWithTimerPath()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].mainMenuIconWithTimer != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(endlessConfigs[0].mainMenuIconWithTimer);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/tournamentInfo/Icon_TournamentEnded.png";
}

std::string RemoteEndlessPortalManager::getChestPath(int index)
{
    if(!endlessConfigs.empty())
    {
        std::string path = "default";
        if(index == 0)      path = endlessConfigs[0].chest_1;
        else if(index == 1) path = endlessConfigs[0].chest_2;
        else if(index == 2) path = endlessConfigs[0].chest_3;
        else                path = endlessConfigs[0].chestOther;
        
        if(path != "default")
        {
            std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(path);
            if(!iconPath.empty())
            {
                return iconPath;
            }
        }
    }
    
    int clampedIndex = (int)clampf(index, 0 , 3);
    
    return StringUtils::format("ui/popup/endlessPortalPopup/Chest_EndlessEvent_0%d.png", clampedIndex + 1);
}

int RemoteEndlessPortalManager::getReviveAmount()
{
    if(!endlessConfigs.empty() && endlessConfigs[0].reviveAmount > 1)
    {
        return endlessConfigs[0].reviveAmount;
    }
    
    return 1;
}

FormattedStringBuilder RemoteEndlessPortalManager::getEndlessCollectableName()
{
    if(endlessConfigs.empty())
    {
        return savedEndlessConfig.collectableNameFormat.ignoreSave();
    }
    
    return endlessConfigs[0].collectableNameFormat.ignoreSave();
}

FormattedStringBuilder RemoteEndlessPortalManager::getEventName()
{
    if(endlessConfigs.empty())
    {
        return savedEndlessConfig.eventNameFormat.ignoreSave();
    }
    
    return endlessConfigs[0].eventNameFormat.ignoreSave();
}

FormattedStringBuilder RemoteEndlessPortalManager::getPlayButtonText()
{
    if(endlessConfigs.empty())
    {
        return savedEndlessConfig.playButtonTextFormat.ignoreSave();
    }
    
    return endlessConfigs[0].playButtonTextFormat.ignoreSave();
}

//

double RemoteEndlessPortalManager::getCurrentTime()
{
    return TimeManager::getInstance()->getTime();
}

//

void RemoteEndlessPortalManager::syncAssetBuffer()
{
    if(endlessConfigs.empty())
    {
        return;
    }
    
    m_assetsList.clear();
    if(endlessConfigs[0].iconSmall != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].iconSmall);
    }
    
    if(endlessConfigs[0].iconLarge != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].iconLarge);
    }
    
    if(endlessConfigs[0].rewardBg != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].rewardBg);
    }
    
    if(endlessConfigs[0].mainMenuIcon != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].mainMenuIcon);
    }
    
    if(endlessConfigs[0].mainMenuIconWithTimer != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].mainMenuIconWithTimer);
    }
    
    if(endlessConfigs[0].joinBg != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].joinBg);
    }
    
    if(endlessConfigs[0].chest_1 != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].chest_1);
    }
    
    if(endlessConfigs[0].chest_2 != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].chest_2);
    }
    
    if(endlessConfigs[0].chest_3 != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].chest_3);
    }
    
    if(endlessConfigs[0].chestOther != "default")
    {
        m_assetsList.push_back(endlessConfigs[0].chestOther);
    }
    
    if(!m_assetsList.empty())
    {
        RemoteAssetsManager::getInstance()->addAssetsToAssetBuffer(m_assetsList);
    }
}

//

void RemoteEndlessPortalManager::syncToRemote()
{
    if(isSystemActivated() && hasSavedData)
    {
        std::vector<std::string> valueList = {savedEndlessConfig.eventID, std::to_string(savedEndlessConfig.playerData.stageCount)};
        std::string valueToSend = StorageManager::getInstance()->mergeValueArray(valueList);
        StorageManager::getInstance()->setData(StorageKey(StorageKeyType::ENDLESS_MODE), valueToSend);
    }
}

void RemoteEndlessPortalManager::syncFromRemote()
{
    if(isSystemActivated())
    {
        StorageManager::getInstance()->getData(StorageKey(StorageKeyType::ENDLESS_MODE), [=](std::string valueStr){
            if(valueStr.empty())
            {
                return;
            }
            
            std::vector<std::string> valueList;
            StorageManager::getInstance()->splitValueString(valueStr, &valueList);
            
            if((int)valueList.size() == 2)
            {
                std::string eventID = valueList.at(0);
                int endlessModeStageCount = StorageManager::getInt(valueList.at(1));
                
                for(int i = 0; i < (int)endlessConfigs.size(); ++i)
                {
                    if(endlessConfigs.at(i).eventID == eventID && getRemainingTimeInSeconds(endlessConfigs.at(i).endlessID, endlessConfigs.at(i).eventID) > 0)
                    {
                        joinEndless(true);
                        
                        savedEndlessConfig.playerData.stageCount = endlessModeStageCount;
                        
                        return;
                    }
                }
                
            }
        });
    }
}

void RemoteEndlessPortalManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::ENDLESS_MODE), STORAGE_EMPTY_VALUE);
}


void RemoteEndlessPortalManager::setStageCount(int stageCount) noexcept
{
    if(isEndlessModeAvailable && !isInClaimState() && hasSavedData)
    {
        if(savedEndlessConfig.playerData.stageCount < stageCount)
        {
            savedEndlessConfig.playerData.stageCount = stageCount;
            savePlayerData(true);
            
            if(ConfigManager::getInstance()->ENABLE_LIVE_LEADERBOARD == 1)
            {
                LeaderboardManager::getInstance()->setValue(RemoteEndlessPortalManager::getInstance()->getTournamentID(), stageCount);
            }
        }
    }
    
}

//

static RemoteEndlessPortalManager *sharedRemoteEndlessPortalManager = NULL;

RemoteEndlessPortalManager* RemoteEndlessPortalManager::getInstance(bool initialize)
{
    if (!sharedRemoteEndlessPortalManager)
    {
        sharedRemoteEndlessPortalManager = new RemoteEndlessPortalManager();
       
    }
    if (!initialize)
    {
        return sharedRemoteEndlessPortalManager;
    }
    if (!isInitialized)
    sharedRemoteEndlessPortalManager->init();

    return sharedRemoteEndlessPortalManager;
}

void RemoteEndlessPortalManager::dispose()
{
    isInitialized = false;

    if (sharedRemoteEndlessPortalManager)
    {
        Director::getInstance()->getScheduler()->unschedule("endlessModeUpdateData", sharedRemoteEndlessPortalManager);
        
        delete sharedRemoteEndlessPortalManager;
        sharedRemoteEndlessPortalManager = NULL;
    }
}

void RemoteEndlessPortalManager::sendEvent(const std::string& title, const std::map<std::string, Value>& params) const noexcept
{
    DataEventManager::getInstance()->send(title, params);
}

int RemoteEndlessPortalManager::getMaxReachedStage(int currentStage) const noexcept
{
    int savedStage =  HBUserDefaults::getInstance()->getIntegerForKey(MAX_REACHED_STAGE_KEY, 0);
    if(currentStage > savedStage)
    {
        HBUserDefaults::getInstance()->setIntegerForKey(MAX_REACHED_STAGE_KEY, currentStage);
        return currentStage;
    }
    
    return savedStage;
}

void RemoteEndlessPortalManager::onUpdateOSNotification(const Notification::IManager* manager)
{
    auto upcomingEndless = LiveOpsManager::getInstance()->getUpcomingEventsByTemplateId(ConfigManager::getInstance()->ENDLESS_MODE_TEMPLATE_IDS[0]);
    double currentTime = getCurrentTime();
    int remainingSeconds = 0;
    
    if(upcomingEndless.empty())
    {
        CCLOG("Upcoming endless not exist!");
    }
    else
    {
        remainingSeconds = upcomingEndless.at(0)->startDateSec - (int)currentTime;
        
        if(remainingSeconds > 0)
        {
            unscheduleNotification("EndlessPortalManagerStart");
            scheduleNotification("EndlessPortalManagerStart", "☠️ New endless portal is started. Come and destroy the stages for big rewards!", remainingSeconds + 2 * 60, true);
        }
    }
    
    if(endlessConfigs.empty() || !isJoinedEndless())
    {
        return;
    }
    
    int oneDay = 86400;
    remainingSeconds = endlessConfigs.at(0).endlessEndDate - currentTime - oneDay;
    if(remainingSeconds > 0)
    {
        unscheduleNotification("EndlessPortalManagerLastDay");
        scheduleNotification("EndlessPortalManagerLastDay", "☠️ Last day in endless portal. Be the best for great rewards!", remainingSeconds + 2 * 60, true);
    }
    
    remainingSeconds = endlessConfigs.at(0).endlessEndDate - currentTime;
    if(remainingSeconds > 0)
    {
        unscheduleNotification("EndlessPortalManagerClaim");
        scheduleNotification("EndlessPortalManagerClaim", "🏁 Endless portal is finished. Come and see your result.", remainingSeconds + 2 * 60, true);
    }
}
