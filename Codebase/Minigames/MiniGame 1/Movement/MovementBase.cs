using Lean.Touch;
using RubyGames.ProgressBarSystem;
using UnityEngine;

public abstract class MovementBase : MonoBehaviour
{
    protected void AssignEvent()
    {
        LeanTouch.OnFingerUpdate += HandleInput;
    }

    protected void DeAssignEvent()
    {
        LeanTouch.OnFingerUpdate -= HandleInput;
    }

    protected abstract void HandleInput(LeanFinger fingerData);
}
