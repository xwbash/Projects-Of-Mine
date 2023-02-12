//
//  SimulatorSelectionNode.h
//  Assassin
//
//  Created by Yiğithan Aydın on 17.10.2022.
//
#include "../Core.h"
#include "../UI/ButtonImage.h"
#include "../GameManager.h"

#ifndef LanguageSelectionNode_h
#define LanguageSelectionNode_h

struct LanguageData
{
    int ID;
    std::string name;
    LanguageType langType;
};

class LanguageSelectionNode : public Node
{
public:
    
    LanguageSelectionNode* setUp();
    
    CREATE_FUNC(LanguageSelectionNode);
    
private:
    ButtonImage* newItem(LanguageData data);
    
};
#endif /* LanguageSelectionNode_h */
