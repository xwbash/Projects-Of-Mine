using Lean.Touch;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveState : BaseStates
{
    private MovementSystem _movementSystem;

    public override void OnStateEnter(MovementSystem movementSystem, TutorialManager tutorialManager)
    {
        Debug.Log("Currently on move state");
        _movementSystem = movementSystem;
        _movementSystem.SetMovementActivity(true);
        tutorialManager.ShowEightTutorialHandler();
    }

    public  override void OnStateExit(MovementSystem movementSystem, TutorialManager tutorialManager)
    {

    }

    public override void OnEventEnter(LeanFinger leanTouch)
    {
        _movementSystem.MoveCrowbar(leanTouch.ScaledDelta);
    }
}
