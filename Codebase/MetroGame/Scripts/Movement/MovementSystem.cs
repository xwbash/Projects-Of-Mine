using System;
using System.Collections.Generic;
using Animation;
using DG.Tweening;
using Sirenix.OdinInspector;
using UnityEditor.Animations;
using UnityEngine;

namespace Movement
{

    public enum CharacterBodyPart
    {
        Leg,
        LeftArm,
        RightArm,
        Head,
        Chest
    }
    
    [Serializable]
    public struct CharacterPoint
    {
        [EnumToggleButtons, HideLabel]
        public CharacterBodyPart CharacterBodyPart;
        public Transform Transform;
    }
    
    public class MovementSystem : MovementBase
    {
        [InfoBox("Define player speed")]
        [Range(0, 10)] [SerializeField] 
        private float m_playerSpeed;


        [SerializeField] 
        private List<CharacterPoint> m_characterPoint = new List<CharacterPoint>();

        [SerializeField] private float m_gravityMultiplier = 3.0f;

        private CharacterController _playerCharacterController;

        

        #region BodyPartTransform

        private Transform _legTransform;

        #endregion


        #region Variables
        
         
        private const float GravityValue = -9.73f;
        [SerializeField] private float _sprintMultiplier;
        private float _defaultSpeed;
        private float _velocityValue;
        private AnimationController _animatorController;
        private Vector3 _targetPosition = new Vector3(0, 0, 0);
        

        private KeyCode _keyCode = KeyCode.LeftShift;
        
        #endregion

        private void OnEnable()
        {
            base.AssignInputEvents();
        }

        private void Start()
        {
            _animatorController = GetComponent<AnimationController>();
            _defaultSpeed = m_playerSpeed;
            _legTransform = m_characterPoint.Find(x => x.CharacterBodyPart == CharacterBodyPart.Leg).Transform;
            _playerCharacterController = gameObject.AddComponent<CharacterController>();
            _playerCharacterController.center = Vector3.up;
            MovePlayer(Vector3.zero);
        }

        private void OnDisable()
        {
            base.DeAssignInputEvents();
        }
        
        public override void MovePlayer(Vector2 positionInput)
        {
            CheckSprint();
            if (_animatorController != null)
            {
                _animatorController.SetIsMoving(positionInput != Vector2.zero);    
            }
            
            _targetPosition = positionInput.x * transform.right + transform.up * ApplyGravity() + positionInput.y * transform.forward;
            _playerCharacterController.Move(new Vector3(_targetPosition.x * Time.deltaTime * m_playerSpeed, _targetPosition.y,_targetPosition.z * Time.deltaTime * m_playerSpeed));
            
            
        }

        private void CheckSprint()
        {
            if (Input.GetKeyDown(_keyCode))
            {
                IncreaseSpeed();
            }
            else if (Input.GetKeyUp(_keyCode))
            {
                SetSpeedToNormal();
            }
        }

        private void SetSpeedToNormal()
        {
            DOTween.To(x => m_playerSpeed = x, m_playerSpeed, _defaultSpeed, 0.8f);
        }

        private void IncreaseSpeed()
        {
            var targetValue = _defaultSpeed * _sprintMultiplier;
            DOTween.To(x => m_playerSpeed = x, m_playerSpeed, targetValue, 0.8f)
                .OnUpdate(OnRunning);
        }

        private void OnRunning()
        {
            // Increase sound of player running and increase avoidance area
        }

        private float ApplyGravity()
        {
            _velocityValue = GravityValue * m_gravityMultiplier * Time.deltaTime;
            return _velocityValue;
        }

    }
}
