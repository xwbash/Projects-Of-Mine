#ifndef TournamentPopup_h
#define TournamentPopup_h

#include "../../Core.h"
#include "../../UI/ScrollNode.h"
#include "../../UI/ButtonImage.h"
#include "../../UI/NotificationBadge.h"
#include "../../Menu/PopupNode.hpp"
#include "../../RemoteTournamentManager.h"
#include "../../UI/LabelWithIcon.h"

USING_NS_CC;

struct TournamentPlayerDataUI
{
    int index = -1;
    FormattedStringBuilder playerName;
    int score = 0;
    bool isMine = false;
    std::vector<RewardData> rewards;
};

//

class TournamentCommonNode : public cocos2d::Node
{
public:
    
    TournamentCommonNode* setup(float width, TournamentPlayerDataUI playerData);
    
    float getHeight();
    
    HBLabel* nameLabel = nullptr;
    
    Sprite* bg = nullptr;
    
    ButtonImage* chestButton = nullptr;
    
    CREATE_FUNC(TournamentCommonNode);
    
private:
    
    float width;
    
    TournamentPlayerDataUI playerData;
    
};

//

class TournamentTopNode : public cocos2d::Node
{
public:
    
    TournamentTopNode* setup(float width, TournamentPlayerDataUI playerData);
    
    HBLabel* nameLabel = nullptr;
    
    CREATE_FUNC(TournamentTopNode);
    
private:
    
    float width;
    
    TournamentPlayerDataUI playerData;
    
};

//

class TournamentPopup : public PopupNode
{
public:
    
    TournamentPopup* setup(TournamentConfig* config);
    
    void onExit() override;
    
    CREATE_POPUP(TournamentPopup);
    
private:
    
    void fillPlayerDataList();
    
    //
    
    float width;
    
    TournamentConfig* config;
    
    Rect scrollArea;
    
    ScrollNode* scrollNode = nullptr;
    
    Node* topNode = nullptr;
    
    int remainingTime = -1;
    
    std::vector<TournamentCommonNode*> tournamentCommonNodes;
    
    std::vector<TournamentTopNode*> tournamentTopNodes;
    
    TournamentCommonNode* bottomNode = nullptr;
    
    TournamentCommonNode* userNode = nullptr;
    
    TournamentTopNode* userTopNode = nullptr;
    
    std::vector<TournamentPlayerDataUI> playerDataList;
    
    int currentPlayerIndex;
    
    EventListenerTouchOneByOne* listener = nullptr;
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    
    void onUsernameChanged(std::string username);
    
};

#endif /* TournamentPopup_h */
