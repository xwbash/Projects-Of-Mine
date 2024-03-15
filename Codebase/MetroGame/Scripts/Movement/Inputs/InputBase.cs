using UnityEngine;
using UnityEngine.Events;

namespace Movement.Inputs
{
    public abstract class InputBase : MonoBehaviour
    {
        public UnityEvent<Vector2> InputEvent; 
        public abstract void AssignEvent(UnityAction<Vector2> unityAction);
        public abstract void DeAssignEvent(UnityAction<Vector2> unityAction);
    }
}