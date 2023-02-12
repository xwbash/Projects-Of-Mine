using Lean.Touch;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class BaseStates
{
    public abstract void OnStateEnter(MovementSystem movementSystem, TutorialManager tutorialManager);
    public abstract void OnStateExit(MovementSystem movementSystem, TutorialManager tutorialManager);

    public abstract void OnEventEnter(LeanFinger leanTouch);
}
