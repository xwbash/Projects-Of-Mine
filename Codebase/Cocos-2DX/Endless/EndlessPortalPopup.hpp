//
//  EndlessPortalPopup.hpp
//  HunterAssassin2
//
//  Created by Yiğithan Aydın on 8.11.2023.
//

#ifndef EndlessPortalPopup_hpp
#define EndlessPortalPopup_hpp

#include "../../Core.h"
#include "../../Menu/PopupNode.hpp"
#include "../../UI/ScrollNode.h"
#include "../../RemoteEndlessPortalManager.hpp"

struct EndlessPortalPlayerDataUI
{
    int index = -1;
    FormattedStringBuilder playerName;
    int score = 0;
    std::string playerID;
    bool isMine = false;
    std::vector<RewardData> rewards;
};

class EndlessPortalCommonNode : public cocos2d::Node
{
public:
    
    EndlessPortalCommonNode* setup(float width, EndlessPortalPlayerDataUI playerData);
    
    float getHeight();
    
    HBLabel* nameLabel = nullptr;
    
    Sprite* bg = nullptr;
    
    ButtonImage* chestButton = nullptr;
    
    CREATE_FUNC(EndlessPortalCommonNode);
    
private:
    
    float width;

    EndlessPortalPlayerDataUI playerData;
    
};

class EndlessPortalTopNode : public cocos2d::Node
{
public:
    
    EndlessPortalTopNode* setup(float width, EndlessPortalPlayerDataUI playerData);
    
    HBLabel* nameLabel = nullptr;
    
    CREATE_FUNC(EndlessPortalTopNode);
    
private:
    
    float width;
    
    EndlessPortalPlayerDataUI playerData;
    
};

class EndlessPortalRankPageNode : public Node
{
public:
    EndlessPortalRankPageNode* setup(std::function<void()> onClosed, EndlessPortalConfig* endlessPortalConfig);
    void fillPlayerDataList(std::function<void()> callback);
    CREATE_FUNC(EndlessPortalRankPageNode);

private:
    Rect scrollArea;
    ScrollNode* scrollNode = nullptr;
    Node* topNode = nullptr;
    std::vector<EndlessPortalPlayerDataUI> playerDataList;
    std::vector<EndlessPortalCommonNode*> endlessPortalCommonNodes;
    std::vector<EndlessPortalTopNode*> endlessPortalTopNodes;
    int currentPlayerIndex;
    EndlessPortalConfig* config = nullptr;
    
    EndlessPortalCommonNode* bottomNode = nullptr;
    EndlessPortalCommonNode* userNode = nullptr;
    EndlessPortalTopNode* userTopNode = nullptr;
    
    int remainingTime;

};

class EndlessPortalPopup : public PopupNode
{
    
public:
    
    EndlessPortalPopup* setup(EndlessPortalConfig* endlessPortalConfig);
    void onExit() override;
    CREATE_POPUP(EndlessPortalPopup);
    
private:
    EndlessPortalRankPageNode* endlessPortalRankPageNode = nullptr;
    void onPlayButtonPressed();
    ButtonImage* playButton = nullptr;
    int remainingTime;
    

};

#endif /* EndlessPortalPopup_hpp */
