using Lean.Touch;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class SwipeState : BaseStates
{
    private MovementSystem _movementSystem;
    public float _sensivityValue = 2.0f;
    public float _minimumDeltaValue = -5;

    public  override void OnStateEnter(MovementSystem movementSystem, TutorialManager tutorialManager)
    {
        Debug.Log("Currently on swipe state");
        _movementSystem = movementSystem;
        movementSystem.SetMovementActivity(true);
        tutorialManager.ShowSlideTutorialHandler();
    }

    public  override void OnStateExit(MovementSystem movementSystem, TutorialManager tutorialManager)
    {
        movementSystem.ExitSlideMode();
    }

    public override void OnEventEnter(LeanFinger leanTouch)
    {
        //var targetValue = ;
        var clampedValue = Mathf.Clamp(leanTouch.ScreenDelta.y, _minimumDeltaValue, 0);
        var swipeForce = clampedValue / _sensivityValue;

        _movementSystem.SwipeTheCrowbar(swipeForce);

    }
}
