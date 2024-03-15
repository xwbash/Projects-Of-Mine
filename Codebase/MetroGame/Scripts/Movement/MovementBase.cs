using System;
using Movement.Inputs;
using UnityEngine;

namespace Movement
{
    public abstract class MovementBase : MonoBehaviour
    {
        [SerializeField] private InputBase m_inputSystem;
        public virtual void AssignInputEvents()
        {
            m_inputSystem.InputEvent.AddListener(MovePlayer);
        }
        
        public virtual void DeAssignInputEvents()
        {
            m_inputSystem.InputEvent.RemoveListener(MovePlayer);
        }

        public abstract void MovePlayer(Vector2 positionInput);
    }
}
