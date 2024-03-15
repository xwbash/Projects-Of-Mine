#include "RemoteTournamentManager.h"
#include "LiveOpsManager.h"
#include "UserSettings.h"
#include "TutorialManager.hpp"
#include "ConfigManager.h"
#include "DataEventManager.h"
#include "RapidJsonExtension.h"
#include "Menu/PopupLayer.h"
#include "Popups/TournamentPopup/TournamentPopup.h"
#include "Popups/TournamentPopup/TournamentJoinPopup.h"
#include "Popups/TournamentPopup/TournamentClaimRewardPopup.h"
#include "FormulaParser/MathFormula.hpp"
#include "RemoteAssetsManager.hpp"
#include "NicknameGeneratorManager.hpp"
#include "StorageManager.h"
#include "TimeManager.h"
#include "Popups/ThreeStepInfoPopup.hpp"
#include "Notification/Manager.hpp"

#define TITLE_1_KEY "title1Loc"
#define TITLE_2_KEY "title2Loc"
#define PLAYER_COUNT_KEY "playerCount"
#define REWARD_CLAIM_PERIOD_KEY "claimPeriod"
#define REWARDS_KEY "rewards"
#define CURVE_SETTINGS_KEY "curveSettings"
#define DROP_RATES_KEY "dropRates"
#define TARGET_SCORE_FORMULA_KEY "targetScoreFormula"
#define ICON_SMALL_KEY "assetIconSmall"
#define ICON_LARGE_KEY "assetIconLarge"
#define REWARD_BG_KEY "assetRewardBg"
#define MAIN_MENU_ICON_KEY "assetMainMenuIcon"
#define MAIN_MENU_ICON_TIMER_KEY "assetMainMenuIconWithTimer"
#define JOIN_BG_KEY "assetJoinBg"
#define CHEST_1_KEY "assetChest_1"
#define CHEST_2_KEY "assetChest_2"
#define CHEST_3_KEY "assetChest_3"
#define CHEST_OTHER_KEY "assetChestOther"
#define COLLECTABLE_NAME_KEY "collectableNameLoc"


#define PLAYER_TOURNAMENT_DATA_SAVE_KEY "HA2_ptd"

static bool isInitialized = false;

void RemoteTournamentManager::init()
{
    isInitialized = true;
    defaultJson =
    R"(
    {
      "title1Loc": "rem_tour_man_title1",
      "title2Loc": "rem_tour_man_title2",
      "playerCount": 20,
      "claimPeriod": 48,
      "dropRates": [1,2,3,4,5,6,7,8,9,10,15],
      "targetScoreFormula": "1000 - x",
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
        "curveSettings":
        [
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
        "assetIconSmall": "default",
        "assetIconLarge": "default",
        "assetRewardBg": "default",
        "assetMainMenuIcon": "default",
        "assetMainMenuIconWithTimer": "default",
        "assetJoinBg": "default",
        "assetChest_1": "default",
        "assetChest_2": "default",
        "assetChest_3": "default",
        "assetChestOther": "default",
        "collectableNameLoc": "rem_tour_man_collectable_name"
    }
    )";
    
    updateData();
}

void RemoteTournamentManager::updateData()
{
    tournamentConfigs.clear();
    
    if(LiveOpsManager::getInstance()->isLiveOpsReady)
    {
        setTournaments();
        
        loadPlayerData();
        
        syncAssetBuffer();
    }
    else
    {
        Director::getInstance()->getScheduler()->schedule([=](float) {
            
            CCLOG("Tournaments - updating data...");
            
            updateData();
            
        }, this, updateInterval, 0, 0.f, false, "tournamentUpdateData");
    }
}

bool RemoteTournamentManager::readData(TournamentConfig &updatedData, rapidjson::Document &document)
{
    if(document.IsNull()) return false;
    
    if(!document.HasMember(TITLE_1_KEY)                 ||
       !document.HasMember(TITLE_2_KEY)                 ||
       !document.HasMember(REWARD_CLAIM_PERIOD_KEY)     ||
       !document.HasMember(DROP_RATES_KEY)              ||
       !document.HasMember(REWARDS_KEY)                 ||
       !document.HasMember(PLAYER_COUNT_KEY)            ||
       !document.HasMember(TARGET_SCORE_FORMULA_KEY)    ||
       !document.HasMember(ICON_SMALL_KEY)              ||
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
       
    if(document.HasMember(TITLE_2_KEY) && document[TITLE_2_KEY].IsString())
    {
       updatedData.title2Format = FormattedStringBuilder(document[TITLE_2_KEY].GetString()).ignoreSave();
    }
    
    if(document.HasMember(PLAYER_COUNT_KEY) && document[PLAYER_COUNT_KEY].IsInt())
    {
        updatedData.playerCount = document[PLAYER_COUNT_KEY].GetInt();
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
    
    if(document.HasMember(DROP_RATES_KEY) && document[DROP_RATES_KEY].IsArray())
    {
        auto dropRatesArray = document[DROP_RATES_KEY].GetArray();
        
        for (auto& dropRate : dropRatesArray)
        {
            updatedData.dropRates.push_back(dropRate.GetInt());
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
       updatedData.collectableName = document[COLLECTABLE_NAME_KEY].GetString();
    }
    
    return true;
}

void RemoteTournamentManager::setTournaments()
{
    isTournamentAvailable = false;
    
    for(auto tournamentID : ConfigManager::getInstance()->TOURNAMENT_TEMPLATE_IDS)
    {
        auto* event = LiveOpsManager::getInstance()->getActiveEventByTemplateId(tournamentID);
        
        TournamentConfig config;
        config.tournamentID = tournamentID;
        
        if(event)
        {
            config.eventID = event->eventId;
            config.tournamentName = event->name;
            config.tournamentStartDate = event->startDateSec;
            config.tournamentEndDate = event->endDateSec;
            
            rapidjson::Document document;
            document.Parse(event->config.c_str());
            
            readData(config, document);
            
            tournamentConfigs.push_back(config);
            

            isTournamentAvailable = true;
        }
    }
    
    if(ADMIN_PLAYER && tournamentConfigs.size() <= 0 && ConfigManager::getInstance()->ENABLE_TOURNAMENTS > 1)
    {
        TournamentConfig config;
        config.tournamentID = "default json";
        config.eventID = "event_temp";
        config.tournamentName = "name_temp";
        config.tournamentStartDate = getCurrentTime() - 7200;
        config.tournamentEndDate = getCurrentTime() + 3600;
        
        rapidjson::Document document;
        document.Parse(defaultJson.c_str());
        
        readData(config, document);
        
        tournamentConfigs.push_back(config);
        
        isTournamentAvailable = true;
    }
}

bool RemoteTournamentManager::isSystemActivated()
{
    if(ConfigManager::getInstance()->ENABLE_TOURNAMENTS <= 0 ||
       !LiveOpsManager::getInstance()->isLiveOpsReady ||
       (!isTournamentAvailable && !isInClaimState()) ||
       !TutorialManager::checkConditionsMet(ConfigManager::getInstance()->TOURNAMENTS_ENABLE_CONDITIONS))
    {
        return false;
    }
    
    return true;
}

bool RemoteTournamentManager::isLocked()
{
    return TutorialManager::checkConditionsLocked(ConfigManager::getInstance()->TOURNAMENTS_ENABLE_CONDITIONS, "Tournaments_Key");
}

void RemoteTournamentManager::removeExpiredTournament(std::string tournamentID)
{
    for(int i = 0; i < (int)tournamentConfigs.size(); ++i)
    {
        if(tournamentConfigs[i].tournamentID == tournamentID)
        {
            tournamentConfigs.erase(tournamentConfigs.begin() + i);
            break;
        }
    }
    
    isTournamentAvailable = ((int)tournamentConfigs.size() <= 0);
}

// MARK: Load / Save

void RemoteTournamentManager::loadPlayerData()
{
    ValueMap tournamentPlayerData = FileUtils::getInstance()->getValueMapFromFile(FileUtils::getInstance()->getWritablePath() + PLAYER_TOURNAMENT_DATA_SAVE_KEY);
    
    if(!tournamentPlayerData.empty())
    {
        savedTournamentConfig.fromValueMap(tournamentPlayerData);
        
        if((int)tournamentConfigs.size() > 0 && (savedTournamentConfig.eventID.empty() || savedTournamentConfig.tournamentID.empty()))
        {
            savedTournamentConfig.eventID = tournamentConfigs[0].eventID;
            savedTournamentConfig.tournamentID = tournamentConfigs[0].tournamentID;
        }
        
        double currentTime = getCurrentTime();
        
        if(currentTime > savedTournamentConfig.tournamentEndDate + savedTournamentConfig.rewardClaimPeriodInHours * 3600)
        {
            clearPlayerData();
            return;
        }
        
        hasSavedData = true;
        
        CCLOG("RemoteTournamentManager - has player data");
        
        if(!tournamentConfigs.empty())
        {
            for(auto& tournamentConfig : tournamentConfigs)
            {
                if(tournamentConfig.eventID == savedTournamentConfig.eventID)
                {
                    savedTournamentConfig.tournamentName = tournamentConfig.tournamentName;
                    savedTournamentConfig.tournamentStartDate = tournamentConfig.tournamentStartDate;
                    savedTournamentConfig.tournamentEndDate = tournamentConfig.tournamentEndDate;
                    savedTournamentConfig.title1Format = tournamentConfig.title1Format;
                    savedTournamentConfig.title2Format = tournamentConfig.title2Format;
                    savedTournamentConfig.rewardClaimPeriodInHours = tournamentConfig.rewardClaimPeriodInHours;
                    savedTournamentConfig.dropRates = tournamentConfig.dropRates;
                    savedTournamentConfig.rewards = tournamentConfig.rewards;
                    savedTournamentConfig.rewardsJson = tournamentConfig.rewardsJson;
                    savedTournamentConfig.curveEasings = tournamentConfig.curveEasings;
                    savedTournamentConfig.curveSettings = tournamentConfig.curveSettings;
                    savedTournamentConfig.targetScoreFormula = tournamentConfig.targetScoreFormula;
                    savedTournamentConfig.iconSmall = tournamentConfig.iconSmall;
                    savedTournamentConfig.iconLarge = tournamentConfig.iconLarge;
                    savedTournamentConfig.rewardBg = tournamentConfig.rewardBg;
                    savedTournamentConfig.collectableName = tournamentConfig.collectableName;
                    
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
    
    CCLOG("RemoteTournamentManager - loaded data");
}

void RemoteTournamentManager::savePlayerData(bool syncToRemote)
{
    auto valueMap = savedTournamentConfig.toValueMap();
    
    FileUtils::getInstance()->writeValueMapToFile(valueMap, FileUtils::getInstance()->getWritablePath() + PLAYER_TOURNAMENT_DATA_SAVE_KEY);
    
    hasSavedData = true;
    
    //
    
    if(syncToRemote)
    {
        std::vector<std::string> valueList = {savedTournamentConfig.eventID, std::to_string(savedTournamentConfig.playerData.playerScore)};
        std::string valueToSend = StorageManager::mergeValueArray(valueList);
        StorageManager::getInstance()->setData(StorageKey(StorageKeyType::TOURNAMENT), valueToSend);
    }
    
    //
    
    CCLOG("RemoteTournamentManager - saved data");
}

void RemoteTournamentManager::clearPlayerData()
{
    FileUtils::getInstance()->removeFile(FileUtils::getInstance()->getWritablePath() + PLAYER_TOURNAMENT_DATA_SAVE_KEY);
    
    savedTournamentConfig = {};
    hasSavedData = false;
    
    CCLOG("RemoteTournamentManager - cleared data");
}

// MARK: Popup Control

void RemoteTournamentManager::tournamentButtonClicked()
{
    if(hasSavedData)
    {
        if(isTournamentAvailable && !isInClaimState())
        {
            showHighscorePopup();
        }
        else if(isInClaimState())
        {
            showClaimPopup();
        }
    }
    else if(isTournamentAvailable)
    {
        showJoinPopup();
    }
}

void RemoteTournamentManager::showHighscorePopup()
{
    if((int)tournamentConfigs.size() > 0 && (int)tournamentConfigs[0].rewards.size() > 0 && getRemainingTimeInSeconds(tournamentConfigs[0].tournamentID, tournamentConfigs[0].eventID) > 0)
    {
        PopupLayer::current()->showPopup(TOURNAMENT_POPUP, TournamentPopup::create()->setup(&savedTournamentConfig));
    }
}

void RemoteTournamentManager::showJoinPopup()
{
    if((int)tournamentConfigs.size() > 0 && (int)tournamentConfigs[0].rewards.size() > 0 && getRemainingTimeInSeconds(tournamentConfigs[0].tournamentID, tournamentConfigs[0].eventID) > 0)
    {
        unscheduleNotification("TournamentManagerStart");
        
        FormattedStringBuilder infoTextBuilder = FormattedStringBuilder("Eliminate enemies, collect tags, and climb up the leaderboard for greater rewards!");
        PopupLayer::current()->showPopup(TOURNAMENT_JOIN_POPUP, TournamentJoinPopup::create()->setup(infoTextBuilder));
    }
}

void RemoteTournamentManager::showTournamentInfoPopup(bool fromJoin)
{
    ThreeStepConfig config;
//    config.title = savedTournamentConfig.title1Format.format();
    config.titleColor = Color4B(255, 255, 200, 255);
    config.titleEffectColor = Color4B(245, 209, 67, 255);
    
    config.iconPaths = {
        "ui/popup/tournamentInfo/Icon_Skulls_TournamentInfo.png",
        getTournamentLargeIconPath(),
        getChestPath(0)
    };
    
    config.infoTexts = {
        "Kill\nEnemies",
        FormattedStringBuilder("Collect\n%s", getTournamentCollectableName().saveInLocalizationMap().format().c_str()).format(),
        "Earn\nRewards"
    };
    
    config.buttonLabel = "Continue";
    config.onButtonClicked = [=](){
        if(fromJoin)
        {
            showHighscorePopup();
        }
    };
    
    PopupLayer::current()->showPopup(THREE_STEP_INFO_POPUP, ThreeStepInfoPopup::create()->setup(config));
}

void RemoteTournamentManager::showClaimPopup()
{
    CCLOG("RemoteTournamentManager - Show Claim Popup");
    std::vector<int> scores;
    for(auto npc : savedTournamentConfig.playerData.npcList)
    {
        scores.push_back(getNpcScore(&npc));
    }
    
    std::sort(scores.begin(), scores.end(), [this](const int& a, const int& b) {

        return a > b;
    });
    
    int playerPlacement = (int)scores.size();
    for(int i = 0; i < (int)scores.size(); ++i)
    {
        if(savedTournamentConfig.playerData.playerScore >= scores.at(i))
        {
            playerPlacement = i;
            break;
        }
    }
    
    std::vector<RewardData> rewards;
    if(playerPlacement < (int)savedTournamentConfig.rewards.size())
    {
        rewards = savedTournamentConfig.rewards.at(playerPlacement);
    }
    
    std::vector<RewardData> actualRewards;
    if(!rewards.empty())
    {
        // claim rewards
        for(int i = 0; i < (int)rewards.size(); ++i)
        {
            auto _rewardList = UserSettings::getInstance()->collectReward(rewards.at(i), "tournament");
            actualRewards.insert(actualRewards.end(), _rewardList.begin(), _rewardList.end());
        }
    }
    
    int playerScore = savedTournamentConfig.playerData.playerScore;
    
    //
    
    DataEventManager::getInstance()->send("tournament_completed", {
        
        {"tournament_name" , Value(savedTournamentConfig.tournamentName) },
        {"reached_tournament_collectable" , Value(playerScore) },
        {"finish_rank" , Value(playerPlacement + 1) }
    });
    
    //
    
    clearPlayerData();
    
    updateData();
    
    unscheduleNotification("TournamentManagerClaim");
    
    PopupLayer::current()->showPopup(TOURNAMENT_CLAIM_REWARD_POPUP, TournamentClaimRewardPopup::create()->setup(playerPlacement, playerScore, actualRewards));
    
    if(MenuNode::current())
    {
        MenuNode::current()->playButton->hideTournamentButton();
    }
}

bool RemoteTournamentManager::checkShowPopup()
{
    if(!isSystemActivated())
    {
        return false;
    }
    
    if(!isJoinedTournament() && !isInClaimState())
    {
        const std::string tournamentJoinPopupSaveKey = "tournamentJoinPopupSaveKey";
        std::string eventID = HBUserDefaults::getInstance()->getStringForKey(tournamentJoinPopupSaveKey.c_str(), "");
        
        if(tournamentConfigs[0].eventID != eventID)
        {
            HBUserDefaults::getInstance()->setStringForKey(tournamentJoinPopupSaveKey.c_str(), tournamentConfigs[0].eventID);
            
            showJoinPopup();
            
            return true;
        }
    }
    else if(isJoinedTournament() && isInClaimState())
    {
        showClaimPopup();
        return true;
    }

    return false;
}

int RemoteTournamentManager::getNotificationCount()
{
    if(isSystemActivated() && isInClaimState())
    {
        return -1099;
    }
    
    return 0;
}

// MARK: Configs

TournamentConfig* RemoteTournamentManager::getTournamentById(std::string tournamentID)
{
    for(int i = 0; i < (int)tournamentConfigs.size(); ++i)
    {
        if(tournamentConfigs[i].tournamentID == tournamentID)
        {
            return &tournamentConfigs[i];
        }
    }
    
    return nullptr;
}

std::vector<RewardData> RemoteTournamentManager::getTournamentRewardsAtIndex(std::string tournamentID, int index)
{
    std::vector<RewardData> rewards;
    auto* config = getTournamentById(tournamentID);
    
    if(config && (int)config->rewards.size() > index)
    {
        return config->rewards.at(index);
    }
    
    return rewards;
}

void RemoteTournamentManager::joinTournament(bool silent)
{
    std::vector<TournamentNpcData> oldNicknamePlayerData = savedTournamentConfig.playerData.npcList;
    savedTournamentConfig = tournamentConfigs[0];
    
    savedTournamentConfig.playerData.tournamentID = tournamentConfigs[0].tournamentID;
    
    if(silent && !oldNicknamePlayerData.empty())
    {
        savedTournamentConfig.playerData.npcList = oldNicknamePlayerData;
        savePlayerData(false);
    }
    else
    {
        savedTournamentConfig.playerData.npcList = generateNPCs(savedTournamentConfig.playerCount - 1);
        savePlayerData(false);
    }
    
    //
    
    if(!silent)
    {
        //
        
        savePlayerData();
        
        DataEventManager::getInstance()->send("tournament_join", {
            
            {"tournament_name" , Value(savedTournamentConfig.tournamentName) },
            {"join_state" , Value(1) }
        });
        
        //
        showTournamentInfoPopup(true);
    }
}

bool RemoteTournamentManager::isJoinedTournament()
{
    return !savedTournamentConfig.tournamentID.empty();
}

bool RemoteTournamentManager::isInClaimState()
{
    if(!hasSavedData)
    {
        return false;
    }
    
    double currentTime = getCurrentTime();
    
    bool state = (currentTime > savedTournamentConfig.tournamentEndDate) && (currentTime - savedTournamentConfig.tournamentEndDate <= savedTournamentConfig.rewardClaimPeriodInHours * 3600);
    
    if(currentTime > savedTournamentConfig.tournamentEndDate + savedTournamentConfig.rewardClaimPeriodInHours * 3600)
    {
        // Exceeded Claim Time
        clearPlayerData();
    }
    
    return state;
}

int RemoteTournamentManager::getCalculatedTargetScore()
{
    if(savedTournamentConfig.targetScoreFormula.empty())
    {
        return 1000;
    }
    
    int missionNo = UserSettings::getInstance()->getMissionNo();
    
    MathFormula formula;
    formula.createVariable('x');
    formula.updateVariable('x', missionNo);
    
    return (int)(formula.calculate(savedTournamentConfig.targetScoreFormula));
}

std::vector<TournamentNpcData> RemoteTournamentManager::generateNPCs(int count)
{
    int maxTargetScore = getCalculatedTargetScore();
    
    std::vector<TournamentNpcData> npcList;
    for(int i = 0; i < count; ++i)
    {
        TournamentNpcData npc;
        npc.curveEasing = savedTournamentConfig.curveEasings.at(i % ((int)savedTournamentConfig.curveEasings.size()));
        npc.curveID = savedTournamentConfig.curveSettings.at(i % ((int)savedTournamentConfig.curveSettings.size()));
        npc.nickname = NicknameGeneratorManager::getInstance()->chooseNickname();
        npc.targetScore = cocos2d::random(0, maxTargetScore);
        
        npcList.push_back(npc);
    }
    
    return npcList;
}

int RemoteTournamentManager::getNpcScore(TournamentNpcData* npc)
{
    float tournamentTime = savedTournamentConfig.tournamentEndDate - savedTournamentConfig.tournamentStartDate;
    float currentDiff = getCurrentTime() - savedTournamentConfig.tournamentStartDate;
    
    float ratio = clampf(currentDiff / tournamentTime, 0.f, 1.f);
    
    float val = npc->curveEasing(ratio);
    
    return (int)(npc->targetScore * val);
}

int RemoteTournamentManager::updatePlayerScore(int amount)
{
    if(isTournamentAvailable && !isInClaimState() && hasSavedData)
    {
        savedTournamentConfig.playerData.playerScore += amount;
        savePlayerData();
    }
    
    return -1;
}

// MARK: Remaining Time

int RemoteTournamentManager::getRemainingTimeInSeconds(std::string tournamentID, std::string eventId)
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_MAC
    int remainingTime = LiveOpsManager::getInstance()->getRemainingTimeInSeconds(tournamentID, eventId);
    if(remainingTime <= 0)
    {
        removeExpiredTournament(tournamentID);
    }
    return remainingTime;
#else
//    --remainingTimeTest;
//    return remainingTimeTest;
    return tournamentConfigs[0].tournamentEndDate - getCurrentTime();
#endif
}

int RemoteTournamentManager::getRemainingTimeInSeconds()
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_MAC
    if(tournamentConfigs.size() > 0)
    {
        int remainingTime = LiveOpsManager::getInstance()->getRemainingTimeInSeconds(tournamentConfigs[0].tournamentID, tournamentConfigs[0].eventID);
        if(remainingTime <= 0)
        {
            removeExpiredTournament(tournamentConfigs[0].tournamentID);
        }
        return remainingTime;
    }
    else
    {
        isTournamentAvailable = false;
        return 0;
    }
#else
    --remainingTimeTest;
    isTournamentAvailable = remainingTimeTest > 0;
    return remainingTimeTest;
#endif
}


//

std::string RemoteTournamentManager::getTournamentLargeIconPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].iconLarge != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].iconLarge);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/tournamentInfo/Icon_DogTag_DropShadow.png";
}

std::string RemoteTournamentManager::getTournamentSmallIconPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].iconSmall != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].iconSmall);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/tournament/Icon_TorunamentReward.png";
}

std::string RemoteTournamentManager::getTournamentRewardBgPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].rewardBg != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].rewardBg);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/common/container/reward/Container_Yellow.png";
}

std::string RemoteTournamentManager::getMainMenuIconPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].mainMenuIcon != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].mainMenuIcon);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/mainMenu/Icon_Tournament.png";
}

std::string RemoteTournamentManager::getMainMenuIconWithTimerPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].mainMenuIconWithTimer != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].mainMenuIconWithTimer);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/tournamentInfo/Icon_TournamentEnded.png";
}

std::string RemoteTournamentManager::getJoinBgPath()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].joinBg != "default")
    {
        std::string iconPath =  RemoteAssetsManager::getInstance()->getAssetPath(tournamentConfigs[0].joinBg);
        if(!iconPath.empty())
        {
            return iconPath;
        }
    }
    
    return "ui/popup/tournamentInfo/Background_EventStarted_Tournament.png";
}

std::string RemoteTournamentManager::getChestPath(int index)
{
    if(!tournamentConfigs.empty())
    {
        std::string path = "default";
        if(index == 0)      path = tournamentConfigs[0].chest_1;
        else if(index == 1) path = tournamentConfigs[0].chest_2;
        else if(index == 2) path = tournamentConfigs[0].chest_3;
        else                path = tournamentConfigs[0].chestOther;
        
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
    
    return StringUtils::format("ui/popup/tournament/Chest_Tournament_0%d_2x.png", clampedIndex + 1);
}

FormattedStringBuilder RemoteTournamentManager::getTournamentCollectableName()
{
    if(!tournamentConfigs.empty() && tournamentConfigs[0].collectableName != "default")
    {
        return FormattedStringBuilder(tournamentConfigs[0].collectableName);
    }
    
    return FormattedStringBuilder("Tags");
}

int RemoteTournamentManager::getBossKillRewardAmount()
{
    if(isSystemActivated() && isJoinedTournament())
    {
        if(!savedTournamentConfig.dropRates.empty())
        {
            return savedTournamentConfig.dropRates.back();
        }
        
        return 10;
    }
    
    return 0;
}

int RemoteTournamentManager::getDropRate(int stageNo)
{
    
    if(isSystemActivated() && isJoinedTournament())
    {
        if(!savedTournamentConfig.dropRates.empty() && stageNo < (int)savedTournamentConfig.dropRates.size())
        {
            return savedTournamentConfig.dropRates.at(stageNo);
        }
    }
    
    return 0;
}

// For cheat only
int RemoteTournamentManager::getAllStageRewards()
{
    if(savedTournamentConfig.dropRates.empty())
    {
        return 100;
    }
    
    int total = 0;
    for(auto& dropRate : savedTournamentConfig.dropRates)
    {
        total += dropRate;
    }
    return total;
}

//

void RemoteTournamentManager::onUpdateOSNotification(const Notification::IManager* manager)
{
    auto upcomingTournaments = LiveOpsManager::getInstance()->getUpcomingEventsByTemplateId(ConfigManager::getInstance()->TOURNAMENT_TEMPLATE_IDS[0]);
    double currentTime = getCurrentTime();
    int remainingSeconds = 0;
    
    if(upcomingTournaments.empty())
    {
        CCLOG("Upcoming tournaments not exist!");
    }
    else
    {
        remainingSeconds = upcomingTournaments.at(0)->startDateSec - (int)currentTime;
        
        if(remainingSeconds > 0)
        {
            unscheduleNotification("TournamentManagerStart");
            scheduleNotification("TournamentManagerStart", "☠️ New manhunt is started. Come and hunt for big rewards!", remainingSeconds + 2 * 60, true);
        }
    }
    
    if(tournamentConfigs.empty() || !isJoinedTournament())
    {
        return;
    }
    
    int oneDay = 86400;
    remainingSeconds = tournamentConfigs.at(0).tournamentEndDate - currentTime - oneDay;
    if(remainingSeconds > 0)
    {
        unscheduleNotification("TournamentManagerLastDay");
        scheduleNotification("TournamentManagerLastDay", "☠️ Last day in manhunt. Be the best for great rewards!", remainingSeconds + 2 * 60, true);
    }
    
    remainingSeconds = tournamentConfigs.at(0).tournamentEndDate - currentTime;
    if(remainingSeconds > 0)
    {
        unscheduleNotification("TournamentManagerClaim");
        scheduleNotification("TournamentManagerClaim", "🏁 Manhunt is finished. Come and see your result.", remainingSeconds + 2 * 60, true);
    }
}

//

double RemoteTournamentManager::getCurrentTime()
{
    return TimeManager::getInstance()->getTime();
}

//

void RemoteTournamentManager::syncAssetBuffer()
{
    if(tournamentConfigs.empty())
    {
        return;
    }
    
    if(tournamentConfigs[0].iconSmall != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].iconSmall);
    }
    
    if(tournamentConfigs[0].iconLarge != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].iconLarge);
    }
    
    if(tournamentConfigs[0].rewardBg != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].rewardBg);
    }
    
    if(tournamentConfigs[0].mainMenuIcon != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].mainMenuIcon);
    }
    
    if(tournamentConfigs[0].mainMenuIconWithTimer != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].mainMenuIconWithTimer);
    }
    
    if(tournamentConfigs[0].joinBg != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].joinBg);
    }
    
    if(tournamentConfigs[0].chest_1 != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].chest_1);
    }
    
    if(tournamentConfigs[0].chest_2 != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].chest_2);
    }
    
    if(tournamentConfigs[0].chest_3 != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].chest_3);
    }
    
    if(tournamentConfigs[0].chestOther != "default")
    {
        m_assetsList.push_back(tournamentConfigs[0].chestOther);
    }
    
    if(!m_assetsList.empty())
    {
        RemoteAssetsManager::getInstance()->addAssetsToAssetBuffer(m_assetsList);
    }
}

//

void RemoteTournamentManager::syncToRemote()
{
    if(isSystemActivated() && hasSavedData)
    {
        std::vector<std::string> valueList = {savedTournamentConfig.eventID, std::to_string(savedTournamentConfig.playerData.playerScore)};
        std::string valueToSend = StorageManager::mergeValueArray(valueList);
        StorageManager::getInstance()->setData(StorageKey(StorageKeyType::TOURNAMENT), valueToSend);
    }
}

void RemoteTournamentManager::syncFromRemote()
{
    if(isSystemActivated())
    {
        StorageManager::getInstance()->getData(StorageKey(StorageKeyType::TOURNAMENT), [=](std::string valueStr){
            if(valueStr.empty())
            {
                return;
            }
            
            std::vector<std::string> valueList;
            StorageManager::splitValueString(valueStr, &valueList);
            
            if((int)valueList.size() == 2)
            {
                std::string eventID = valueList.at(0);
                int tournamentCollectable = StorageManager::getInt(valueList.at(1));
                
                for(int i = 0; i < (int)tournamentConfigs.size(); ++i)
                {
                    if(tournamentConfigs.at(i).eventID == eventID && getRemainingTimeInSeconds(tournamentConfigs.at(i).tournamentID, tournamentConfigs.at(i).eventID) > 0)
                    {
                        joinTournament(true);
                        
                        savedTournamentConfig.playerData.playerScore = tournamentCollectable;
                        
                        return;
                    }
                }
                
            }
        });
    }
}

void RemoteTournamentManager::eraseRemote()
{
    StorageManager::getInstance()->setData(StorageKey(StorageKeyType::TOURNAMENT), STORAGE_EMPTY_VALUE);
}

//

static RemoteTournamentManager *sharedRemoteTournamentManager = NULL;

RemoteTournamentManager* RemoteTournamentManager::getInstance(bool initialize)
{
    if (!sharedRemoteTournamentManager)
    {
        sharedRemoteTournamentManager = new RemoteTournamentManager();
       
    }
    if (!initialize)
    {
        return sharedRemoteTournamentManager;
    }
    if (!isInitialized)
    sharedRemoteTournamentManager->init();

    return sharedRemoteTournamentManager;
}

void RemoteTournamentManager::dispose()
{
    isInitialized = false;

    Notification::Manager::getInstance().unbindListener(sharedRemoteTournamentManager);
    
    if (sharedRemoteTournamentManager)
    {
        Director::getInstance()->getScheduler()->unschedule("tournamentUpdateData", sharedRemoteTournamentManager);
        
        delete sharedRemoteTournamentManager;
        sharedRemoteTournamentManager = NULL;
    }
}
