#ifndef RemoteTournamentManager_h
#define RemoteTournamentManager_h

#include "Core.h"
#include "BeaconManager.h"
#include "RemoteRewardDataManager.h"
#include "easing.h"
#include "IStorageHandler.h"
#include "IAssetHandler.hpp"
#include "Notification/INotificationObserver.hpp"

struct TournamentNpcData
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

struct TournamentPlayerData
{
    std::string tournamentID;
    int playerScore = 0;
    
    std::vector<TournamentNpcData> npcList;
    
    //
    
    void fromValueMap(cocos2d::ValueMap &map)
    {
        npcList.clear();
        
        if(map.count("tournamentID") > 0)
        {
            tournamentID = map["tournamentID"].asString();
        }
        
        if(map.count("playerScore") > 0)
        {
            playerScore = map["playerScore"].asInt();
        }
        
        if(map.count("npcList") > 0)
        {
            cocos2d::ValueVector npcs = map["npcList"].asValueVector();
            for(auto& npc :npcs)
            {
                TournamentNpcData npcData;
                npcData.fromValueMap(npc.asValueMap());
                
                npcList.push_back(npcData);
            }
        }
    }
    
    //
    
    cocos2d::ValueMap toValueMap()
    {
        cocos2d::ValueMap valueMap;
        
        valueMap["tournamentID"] = tournamentID;
        valueMap["playerScore"] = playerScore;
        
        cocos2d::ValueVector npcsVector;
        for(auto& npc : npcList)
        {
            npcsVector.push_back(cocos2d::Value(npc.toValueMap()));
        }
        valueMap["npcList"] = npcsVector;
        
        return valueMap;
    }
};

struct TournamentConfig
{
    std::string eventID;
    std::string tournamentID;
    std::string tournamentName;
    int tournamentStartDate = 0;
    int tournamentEndDate = 0;
    
    FormattedStringBuilder title1Format;
    FormattedStringBuilder title2Format;
    int playerCount;
    int rewardClaimPeriodInHours;
    std::vector<int> dropRates;
    std::string targetScoreFormula;
    
    std::vector<std::string> curveSettings;
    std::vector<easingFunction> curveEasings;
    
    std::vector<std::vector<RewardData>> rewards;
    std::string rewardsJson;
    
    std::string iconSmall = "default";
    std::string iconLarge = "default";
    std::string rewardBg = "default";
    std::string collectableName = "default";
    std::string mainMenuIcon = "default";
    std::string mainMenuIconWithTimer = "default";
    std::string joinBg = "default";
    std::string chest_1 = "default";
    std::string chest_2 = "default";
    std::string chest_3 = "default";
    std::string chestOther = "default";
    
    TournamentPlayerData playerData;
    
    //
    
    void fromValueMap(cocos2d::ValueMap &map)
    {
        if(map.count("eventID") > 0)
        {
            eventID = map["eventID"].asString();
        }
        
        if(map.count("tournamentID") > 0)
        {
            tournamentID = map["tournamentID"].asString();
        }
        
        if(map.count("tournamentName") > 0)
        {
            tournamentName = map["tournamentName"].asString();
        }
        
        if(map.count("tournamentStartDate") > 0)
        {
            tournamentStartDate = map["tournamentStartDate"].asInt();
        }
        
        if(map.count("tournamentEndDate") > 0)
        {
            tournamentEndDate = map["tournamentEndDate"].asInt();
        }
        
        if(map.count("title1") > 0)
        {
            title1Format = FormattedStringBuilder(map["title1"].asString()).ignoreSave();
        }
        
        if(map.count("title2") > 0)
        {
            title2Format = FormattedStringBuilder(map["title2"].asString()).ignoreSave();
        }
        
        if(map.count("playerCount") > 0)
        {
            playerCount = map["playerCount"].asInt();
        }
        
        if(map.count("claimPeriod") > 0)
        {
            rewardClaimPeriodInHours = map["claimPeriod"].asInt();
        }
        
        if(map.count("dropRates") > 0)
        {
            auto dropRatesVector = map["dropRates"].asValueVector();
            for(auto& dropRate : dropRatesVector)
            {
                dropRates.push_back(dropRate.asInt());
            }
        }
        
        if(map.count("targetScoreFormula") > 0)
        {
            targetScoreFormula = map["targetScoreFormula"].asString();
        }
        
        if(map.count("rewards") > 0)
        {
            rewardsJson = map["rewards"].asString();
            
            //
            
            if(!rewardsJson.empty())
            {
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
        
        if(map.count("collectableName") > 0)
        {
            collectableName = map["collectableName"].asString();
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
        valueMap["tournamentID"] = tournamentID;
        valueMap["tournamentName"] = tournamentName;
        valueMap["tournamentStartDate"] = tournamentStartDate;
        valueMap["tournamentEndDate"] = tournamentEndDate;
        valueMap["title1"] = title1Format.getOriginalString();
        valueMap["title2"] = title2Format.getOriginalString();
        valueMap["playerCount"] = playerCount;
        valueMap["claimPeriod"] = rewardClaimPeriodInHours;
        valueMap["rewards"] = rewardsJson;
        valueMap["targetScoreFormula"] = targetScoreFormula;
        valueMap["iconSmall"] = iconSmall;
        valueMap["iconLarge"] = iconLarge;
        valueMap["rewardBg"] = rewardBg;
        valueMap["collectableName"] = collectableName;
        valueMap["mainMenuIcon"] = mainMenuIcon;
        valueMap["mainMenuIconWithTimer"] = mainMenuIconWithTimer;
        valueMap["joinBg"] = joinBg;
        valueMap["chest_1"] = chest_1;
        valueMap["chest_2"] = chest_2;
        valueMap["chest_3"] = chest_3;
        valueMap["chestOther"] = chestOther;
        
        cocos2d::ValueVector dropRatesVector;
        for(auto& dropRate : dropRates)
        {
            dropRatesVector.push_back(cocos2d::Value(dropRate));
        }
        valueMap["dropRates"] = dropRatesVector;
        
        cocos2d::ValueVector curveSettingsVector;
        for(auto& curve : curveSettings)
        {
            curveSettingsVector.push_back(cocos2d::Value(curve));
        }
        valueMap["curveSettings"] = curveSettingsVector;
        
        valueMap["playerData"] = playerData.toValueMap();
        
        return valueMap;
    }
};


class RemoteTournamentManager : public IStorageHandler, public IAssetHandler, public Notification::INotificationObserver
{
public:
    
    void init();

    void updateData();

    bool readData(TournamentConfig& updatedData, rapidjson::Document& document);
    
    void setTournaments();
    
    bool isSystemActivated();
    
    bool isLocked();
    
    void removeExpiredTournament(std::string tournamentID);
    
    //
    
    void loadPlayerData();
    
    void savePlayerData(bool syncToRemote = true);
    
    void clearPlayerData();
    
    //
    
    void tournamentButtonClicked();
    
    void showHighscorePopup();
    
    void showJoinPopup();
    
    void showTournamentInfoPopup(bool fromJoin = false);
    
    void showClaimPopup();
    
    bool checkShowPopup();
    
    int getNotificationCount();
    
    //
    
    TournamentConfig* getTournamentById(std::string tournamentID);
    
    std::vector<RewardData> getTournamentRewardsAtIndex(std::string tournamentID, int index);
    
    void joinTournament(bool silent = false);
    
    bool isJoinedTournament();
    
    bool isInClaimState();
    
    int getCalculatedTargetScore();
    
    std::vector<TournamentNpcData> generateNPCs(int count);
    
    int getNpcScore(TournamentNpcData* npc);
    
    int updatePlayerScore(int amount);
    
    //
    
    int getRemainingTimeInSeconds(std::string tournamentID, std::string eventId);
    
    int getRemainingTimeInSeconds();
    
    //
    
    std::string getTournamentLargeIconPath();
    
    std::string getTournamentSmallIconPath();
    
    std::string getTournamentRewardBgPath();
    
    FormattedStringBuilder getTournamentCollectableName();
    
    std::string getMainMenuIconPath();
    
    std::string getMainMenuIconWithTimerPath();
    
    std::string getJoinBgPath();
    
    std::string getChestPath(int index);
    
    int getBossKillRewardAmount();
    
    int getDropRate(int stageNo);
    
    int getAllStageRewards();
    
    //
    
    void onUpdateOSNotification(const Notification::IManager* manager) override;
    
    //
    
    bool isTournamentAvailable = false;
    
    //
    
    void syncAssetBuffer() override;
    
    //

    static RemoteTournamentManager* getInstance(bool initialize = true);
    
    void syncToRemote() override;
    
    void syncFromRemote() override;
    
    void eraseRemote() override;
    
    static void dispose();
    
private:
    
    double getCurrentTime();
    
    void checkTournamentEnd();
    
    std::string defaultJson;
    
    float updateInterval = 2;
    
    std::vector<TournamentConfig> tournamentConfigs;
    
    TournamentConfig savedTournamentConfig;
    
    bool hasSavedData = false;
    
    std::vector<std::string> m_assetsList;
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    int remainingTimeTest = 300;
#endif
            
};

#endif /* RemoteTournamentManager_h */
