#ifndef SimulatorSelectionNode_h
#define SimulatorSelectionNode_h

//
//  SimulatorSelectionNode.h
//  Assassin
//
//  Created by Yiğithan Aydın on 17.10.2022.
//
#include "../Core.h"
#include "../UI/ButtonImage.h"
#include "../GameManager.h"

struct SimulatorData
{
    std::string name;
    int resolutionX,resolutionY;
    bool isHasXFrame;
    bool isHasDynamicIsland;
};

class SimulatorSelectionNode : public Node
{
public:
    
    SimulatorSelectionNode* setUp();
    
    CREATE_FUNC(SimulatorSelectionNode);
    
private:
    ButtonImage* newItem(SimulatorData data);
    
};
#endif /* SimulatorSelectionNode_h */
