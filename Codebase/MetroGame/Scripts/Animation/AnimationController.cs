using System;
using Movement.Inputs;
using UnityEngine;

namespace Animation
{
    public class AnimationController : MonoBehaviour
    {
        private static readonly int MovementDirectionY = Animator.StringToHash("MovementDirectionY");
        private static readonly int MovementDirectionX = Animator.StringToHash("MovementDirectionX");
        private static readonly int IsMoving = Animator.StringToHash("IsMoving");

        
        [SerializeField] private InputBase m_inputSystem;
        private Animator _animator;


        private void OnEnable()
        {
            AssignInputEvents();
        }

        private void OnDisable()
        {
            DeAssignInputEvents();
        }

        private void Start()
        {
            _animator = GetComponent<Animator>();
        }
        public void AssignInputEvents()
        {
            m_inputSystem.InputEvent.AddListener(MovementAnimationUpdate);
        }
        
        public void DeAssignInputEvents()
        {
            m_inputSystem.InputEvent.RemoveListener(MovementAnimationUpdate);
        }
        private void MovementAnimationUpdate(Vector2 positionMove)
        {
            _animator.SetFloat(MovementDirectionX, positionMove.x);
            _animator.SetFloat(MovementDirectionY, positionMove.y);
        }

        public void SetIsMoving(bool isMoving)
        {
            if (_animator != null)
            {
                _animator.SetBool(IsMoving, isMoving);    
            }
        }

    }
}
