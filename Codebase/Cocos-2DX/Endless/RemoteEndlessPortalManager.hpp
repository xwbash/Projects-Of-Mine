//
//  RemoteEndlessPortalManager.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#ifndef RemoteEndlessPortalManager_hpp
#define RemoteEndlessPortalManager_hpp

#include "Core.h"
#include "BeaconManager.h"
#include "RemoteRewardDataManager.h"
#include "easing.h"
#include "IStorageHandler.h"
#include "IAssetHandler.hpp"
#include "Notification/INotificationObserver.hpp"

#define TITLE_1_KEY "title1Loc"
#define EVENT_NAME "eventNameLoc"
#define PLAY_BUTTON_TEXT "playButtonTextLoc"
#define BATTLE_PASS_TOKENS "battlePassTokens"
#define REVIVE_AMOUNT "reviveAmount"
#define PLAYER_COUNT_KEY "playerCount"
#define REWARD_CLAIM_PERIOD_KEY "claimPeriod"
#define REWARDS_KEY "rewards"
#define CURVE_SETTINGS_KEY "curveSettings"
#define TARGET_SCORE_FORMULA_KEY "targetScoreFormula"
#define ICON_SMALL_KEY "iconSmall"
#define ICON_LARGE_KEY "iconLarge"
#define REWARD_BG_KEY "rewardBg"
#define MAIN_MENU_ICON_KEY "mainMenuIcon"
#define MAIN_MENU_ICON_TIMER_KEY "mainMenuIconWithTimer"
#define JOIN_BG_KEY "joinBg"
#define CHEST_1_KEY "chest_1"
#define CHEST_2_KEY "chest_2"
#define CHEST_3_KEY "chest_3"
#define CHEST_OTHER_KEY "chestOther"
#define COLLECTABLE_NAME_KEY "collectableNameLoc"
#define GAME_DIFFICULTY "gameDifficulty"


#define PLAYER_ENDLESS_MODE_DATA_SAVE_KEY "HA2_etf"

struct EndlessPortalNpcData
{
    FormattedStringBuilder nickname;
    std::string curveID;
    easingFunction curveEasing;
    int targetScore = 0;
    
    //
    
    void fromValueMap(cocos2d::ValueMap &map)
    {
        if(map.count("nickname") > 0)
        {
            nickname = FormattedStringBuilder(map["nickname"].asString());
        }
        
        if(map.count("curveID") > 0)
        {
            curveID = map["curveID"].asString();
            curveEasing = getEasingFunction( getEnumFromString(curveID) );
        }
        
        if(map.count("targetScore") > 0)
        {
            targetScore = map["targetScore"].asInt();
        }
    }
    
    //
    
    cocos2d::ValueMap toValueMap()
    {
        cocos2d::ValueMap npcMap;
        npcMap["nickname"] = nickname.format();
        npcMap["curveID"] = curveID;
        npcMap["targetScore"] = targetScore;
        return npcMap;
    }
};

struct EndlessPortalPlayerData
{
    std::string endlessID;
    int stageCount = 0;
    
    std::vector<EndlessPortalNpcData> npcList;
    
    //
    
    void fromValueMap(cocos2d::ValueMap &map)
    {
        npcList.clear();
        
        if(map.count("endlessID") > 0)
        {
            endlessID = map["endlessID"].asString();
        }
        
        if(map.count("stageCount") > 0)
        {
            stageCount = map["stageCount"].asInt();
        }
        
        if(map.count("npcList") > 0)
        {
            cocos2d::ValueVector npcs = map["npcList"].asValueVector();
            for(auto& npc :npcs)
            {
                EndlessPortalNpcData npcData;
                npcData.fromValueMap(npc.asValueMap());
                
                npcList.push_back(npcData);
            }
        }
    }
    
    //
    
    cocos2d::ValueMap toValueMap()
    {
        cocos2d::ValueMap valueMap;
        
        valueMap["endlessID"] = endlessID;
        valueMap["stageCount"] = stageCount;
        
        cocos2d::ValueVector npcsVector;
        for(auto& npc : npcList)
        {
            npcsVector.push_back(cocos2d::Value(npc.toValueMap()));
        }
        valueMap["npcList"] = npcsVector;
        
        return valueMap;
    }
};

struct EndlessPortalConfig
{
    std::string eventID;
    std::string endlessID;
    std::string endlessName;
    int endlessStartDate = 0;
    int endlessEndDate = 0;
    
    FormattedStringBuilder title1Format;
    FormattedStringBuilder eventNameFormat;
    FormattedStringBuilder playButtonTextFormat;
    int playerCount;
    int reviveAmount;
    int rewardClaimPeriodInHours;
    std::string targetScoreFormula;
    
    std::vector<int> battlePassTokens;
    std::vector<int> gameDifficulty;
    
    std::vector<std::string> curveSettings;
    std::vector<easingFunction> curveEasings;
    
    std::vector<std::vector<RewardData>> rewards;
    std::string rewardsJson;
    
    std::string iconSmall = "default";
    std::string iconLarge = "default";
    std::string rewardBg = "default";
    FormattedStringBuilder collectableNameFormat;
    std::string mainMenuIcon = "default";
    std::string mainMenuIconWithTimer = "default";
    std::string joinBg = "default";
    std::string chest_1 = "default";
    std::string chest_2 = "default";
    std::string chest_3 = "default";
    std::string chestOther = "default";
    
    EndlessPortalPlayerData playerData;
    
    //
    
    void fromValueMap(cocos2d::ValueMap &map)
    {
        if(map.count("eventID") > 0)
        {
            eventID = map["eventID"].asString();
        }
        
        if(map.count("endlessID") > 0)
        {
            endlessID = map["endlessID"].asString();
        }
        
        if(map.count("endlessName") > 0)
        {
            endlessName = map["endlessName"].asString();
        }
        
        if(map.count("endlessStartDate") > 0)
        {
            endlessStartDate = map["endlessStartDate"].asInt();
        }
        
        if(map.count("endlessEndDate") > 0)
        {
            endlessEndDate = map["endlessEndDate"].asInt();
        }
        
        if(map.count(TITLE_1_KEY) > 0)
        {
            title1Format = FormattedStringBuilder(map[TITLE_1_KEY].asString());
        }
        
        
        if(map.count("playerCount") > 0)
        {
            playerCount = map["playerCount"].asInt();
        }
        
        if(map.count(REVIVE_AMOUNT) > 0)
        {
            reviveAmount = map[REVIVE_AMOUNT].asInt();
        }
        
        if(map.count(BATTLE_PASS_TOKENS) > 0)
        {
            for (auto& val : map[BATTLE_PASS_TOKENS].asValueVector())
            {
                battlePassTokens.push_back(val.asInt());
            }
            
        }
        
        if(map.count("gameDifficulty") > 0)
        {
            for (auto& val : map["gameDifficulty"].asValueVector())
            {
                gameDifficulty.push_back(val.asInt());
            }
            
        }
        
        if(map.count(EVENT_NAME))
        {
            eventNameFormat = FormattedStringBuilder(map[EVENT_NAME].asString()).ignoreSave();
        }
        
        if(map.count(PLAY_BUTTON_TEXT))
        {
            playButtonTextFormat = FormattedStringBuilder(map[PLAY_BUTTON_TEXT].asString()).ignoreSave();
        }
        
        if(map.count("claimPeriod") > 0)
        {
            rewardClaimPeriodInHours = map["claimPeriod"].asInt();
        }

        
        if(map.count("targetScoreFormula") > 0)
        {
            targetScoreFormula = map["targetScoreFormula"].asString();
        }
        
        if(map.count("rewards") > 0)
        {
            rewardsJson = map["rewards"].asString();
            
            //
            
            rapidjson::Document document;
            document.Parse(rewardsJson.c_str());

            auto rewardsArray = document.GetArray();
            
            for (auto& reward : rewardsArray)
            {
                auto rewardsData = RemoteRewardDataManager::getInstance()->getRewardDatas( RemoteRewardDataManager::getInstance()->deserializeRewardJson(reward.GetObject()));
                
                if(!rewardsData.empty() && rewardsData.size() > 0)
                {
                    rewards.push_back(rewardsData);
                }
            }
            
        }
        
        if(map.count("curveSettings") > 0)
        {
            auto curveArray = map["curveSettings"].asValueVector();
            for (auto& curve : curveArray)
            {
                curveSettings.push_back(curve.asString());
                curveEasings.push_back(getEasingFunction( getEnumFromString(curve.asString()) ));
            }
        }
        
        if(map.count("playerData") > 0)
        {
            auto playerValueMap = map["playerData"].asValueMap();
            playerData.fromValueMap(playerValueMap);
        }
        
        if(map.count("iconSmall") > 0)
        {
            iconSmall = map["iconSmall"].asString();
        }
        
        if(map.count("iconLarge") > 0)
        {
            iconLarge = map["iconLarge"].asString();
        }
        
        if(map.count("rewardBg") > 0)
        {
            rewardBg = map["rewardBg"].asString();
        }
        
        if(map.count(COLLECTABLE_NAME_KEY) > 0)
        {
            collectableNameFormat = FormattedStringBuilder(map[COLLECTABLE_NAME_KEY].asString()).ignoreSave();
        }
        
        if(map.count("mainMenuIcon") > 0)
        {
            mainMenuIcon = map["mainMenuIcon"].asString();
        }
        
        if(map.count("mainMenuIconWithTimer") > 0)
        {
            mainMenuIconWithTimer = map["mainMenuIconWithTimer"].asString();
        }
        
        if(map.count("joinBg") > 0)
        {
            joinBg = map["joinBg"].asString();
        }
        
        if(map.count("chest_1") > 0)
        {
            chest_1 = map["chest_1"].asString();
        }
        
        if(map.count("chest_2") > 0)
        {
            chest_2 = map["chest_2"].asString();
        }
        
        if(map.count("chest_3") > 0)
        {
            chest_3 = map["chest_3"].asString();
        }
        
        if(map.count("chestOther") > 0)
        {
            chestOther = map["chestOther"].asString();
        }
    }
    
    //
    
    cocos2d::ValueMap toValueMap()
    {
        cocos2d::ValueMap valueMap;
        
        valueMap["eventID"] = eventID;
        valueMap["endlessID"] = endlessID;
        valueMap["endlessName"] = endlessName;
        valueMap["endlessStartDate"] = endlessStartDate;
        valueMap["endlessEndDate"] = endlessEndDate;
        valueMap[TITLE_1_KEY] = title1Format.getOriginalString();
        valueMap[EVENT_NAME] = eventNameFormat.getOriginalString();
        valueMap[PLAY_BUTTON_TEXT] = playButtonTextFormat.getOriginalString();
        valueMap["playerCount"] = playerCount;
        valueMap["claimPeriod"] = rewardClaimPeriodInHours;
        valueMap["rewards"] = rewardsJson;
        valueMap[REVIVE_AMOUNT] = reviveAmount;
        valueMap["targetScoreFormula"] = targetScoreFormula;
        valueMap["iconSmall"] = iconSmall;
        valueMap["iconLarge"] = iconLarge;
        valueMap["rewardBg"] = rewardBg;
        valueMap[COLLECTABLE_NAME_KEY] = collectableNameFormat.getOriginalString();
        valueMap["mainMenuIcon"] = mainMenuIcon;
        valueMap["mainMenuIconWithTimer"] = mainMenuIconWithTimer;
        valueMap["joinBg"] = joinBg;
        valueMap["chest_1"] = chest_1;
        valueMap["chest_2"] = chest_2;
        valueMap["chest_3"] = chest_3;
        valueMap["chestOther"] = chestOther;
        
        
        cocos2d::ValueVector curveSettingsVector;
        for(auto& curve : curveSettings)
        {
            curveSettingsVector.push_back(cocos2d::Value(curve));
        }
        valueMap["curveSettings"] = curveSettingsVector;

        cocos2d::ValueVector battlePassTokensVector;
        for(auto& battlePassToken : battlePassTokens)
        {
            battlePassTokensVector.push_back(cocos2d::Value(battlePassToken));
        }
        valueMap[BATTLE_PASS_TOKENS] = battlePassTokensVector;

        cocos2d::ValueVector gameDifficultyVector;
        for(auto& gameDifficultyValue : gameDifficultyVector)
        {
            gameDifficultyVector.push_back(cocos2d::Value(gameDifficultyValue));
        }
        valueMap["gameDifficulty"] = gameDifficultyVector;

        
        valueMap["playerData"] = playerData.toValueMap();
        
        return valueMap;
    }
};

class RemoteEndlessPortalManager : public IStorageHandler, public IAssetHandler, public Notification::INotificationObserver
{
public:
    
    void init();

    void updateData();

    bool readData(EndlessPortalConfig& updatedData, rapidjson::Document& document);
    
    void setEndlessModeConfigs();
    
    bool isSystemActivated();
    
    bool isLocked();
    
    void onGameFinished(int stageNo) noexcept;
    
    void removeExpiredEndlessModes(std::string endlessID);
    
    void setStageCount(int stageCount) noexcept;
    //
    
    void loadPlayerData();
    
    void savePlayerData(bool syncToRemote = true);
    
    void clearPlayerData();
    
    //
    
    void onUpdateOSNotification(const Notification::IManager* manager) override;
    
    //
    
    void endlessButtonClicked();
    
    void showPopup();
    
    void showJoinPopup();
        
    void showClaimPopup();
    
    void showEndlessInfoPopup(bool fromJoin = false);

    bool checkShowPopup();
    
    int getNotificationCount();
    
    int getBattlePassToken(int currentQuest);
    std::vector<int> getDifficulty();
    
    int getReviveAmount();
    //
    
    EndlessPortalConfig* getEndlessById(std::string endlessID);
    
    std::vector<RewardData> getEndlessRewardsAtIndex(std::string endlessID, int index);
    
    void joinEndless(bool silent = false);
    
    bool isJoinedEndless();
    
    bool isInClaimState();
    
    int getCalculatedTargetScore();
    
    std::vector<EndlessPortalNpcData> generateNPCs(int count);
    
    int getNpcScore(EndlessPortalNpcData* npc);
    
    //
    
    int getRemainingTimeInSeconds(std::string endlessID, std::string eventId);
    
    int getRemainingTimeInSeconds();
    
    //
    //
    
    std::string getEndlessLargeIconPath();
    
    std::string getEndlessSmallIconPath();
        
    FormattedStringBuilder getEndlessCollectableName();
    
    std::string getMainMenuIconPath();
    
    FormattedStringBuilder getEventName();
    
    FormattedStringBuilder getPlayButtonText();
    
    std::string getJoinBgPath();
    
    std::string getMainMenuIconWithTimerPath();
        
    std::string getChestPath(int index);
    
    std::string getTournamentID();
    
    //
    
    bool isEndlessModeAvailable = false;
    
    //
    
    void syncAssetBuffer() override;
    
    //

    static RemoteEndlessPortalManager* getInstance(bool initialize = true);
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    
    static void dispose();
    
private:
    
    double getCurrentTime();
    
    int getMaxReachedStage(int currentStage) const noexcept;
    
    void sendEvent(const std::string& title,
                   const std::map<std::string, Value>& params) const noexcept;
    
private:
    
    std::string defaultJson;
    
    float updateInterval = 2;
    
    std::vector<EndlessPortalConfig> endlessConfigs;
    
    EndlessPortalConfig savedEndlessConfig;
    
    bool hasSavedData = false;
    
    std::vector<std::string> m_assetsList;
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    int remainingTimeTest = 300;
#endif
            
};

#endif /* RemoteEndlessPortalManager_hpp */
