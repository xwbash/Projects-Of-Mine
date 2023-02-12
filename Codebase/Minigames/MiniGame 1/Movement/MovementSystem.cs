using _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts.Controller;
using DG.Tweening;
using Lean.Touch;
using Sirenix.OdinInspector;
using System;
using UnityEngine;
using Zenject;

public class MovementSystem : MonoBehaviour
{
    private static readonly int CrowBarBlendTree = Animator.StringToHash("CrowBarBlendTree");

    [SerializeField] private Vector2 m_clampVertical, m_clampHorizontal;
    [SerializeField] private float m_speedValue;

    private CrowbarAnimatorController _crowbarAnimatorController;

    private SnapPointManager _snapPointManager;
    private MovementStateMachine _movementStateMachine;
    private Vector3 _defaultPosition;
    private WoodDeformationController _currentDeformationController;

    private float _lastSwipedelta;
    private bool _isMovementActive = false;
    private float _animationRate = 200;
    private float _animationRateDefault = 200;
    private float _animationLerpValue = 0;


    [Inject]
    private void Construct(SnapPointManager snapPointManager)
    {
        this._snapPointManager = snapPointManager;
    }

    private void Start()
    {
        _movementStateMachine = GetComponent<MovementStateMachine>();
        _crowbarAnimatorController = GetComponent<CrowbarAnimatorController>();
        _defaultPosition = transform.position;
    }

    public void MoveCrowbar(Vector2 scaledDelta)
    {
        if (!_isMovementActive)
        {
            return;
        }

        SetAnimationLerpValue(0);

        var deltaPosition = scaledDelta * (m_speedValue * Time.deltaTime);
        var position = transform.position;
        position += new Vector3(0, deltaPosition.y, -deltaPosition.x);

        position.z = Mathf.Clamp(position.z, m_clampVertical.x, m_clampVertical.y);
        position.y = Mathf.Clamp(position.y, m_clampHorizontal.x, m_clampHorizontal.y);
        transform.position = position;

        var closestSnapPoint = _snapPointManager.GetTheClosestPoint(transform.position);

        if (closestSnapPoint != Vector3.zero)
        {
            SnapToPoint(closestSnapPoint);
        }

    }

    public void SwipeTheCrowbar(float swipeDelta)
    {


        _animationRate += swipeDelta;
        SetAnimationLerpValue(Mathf.InverseLerp(_animationRateDefault, 0, _animationRate));
        //_crowbarAnimator.SetFloat(CrowBarBlendTree, swipeDelta);
        //_currentDeformationController.DeformWood(swipeDelta);


    }

    private void SetAnimationLerpValue(float value)
    {
        _animationLerpValue = value;
        _crowbarAnimatorController.SetAnimationValue(value);

        if (!ReferenceEquals(_currentDeformationController, null))
        {
            _currentDeformationController.DeformWood(value);
        }
        
        if (value >= 1)
        {
            Debug.Log("animation is high enough");
            _movementStateMachine.ChangeState(new MoveState());
        }
    }

    private void SnapToPoint(Vector3 closestSnapPoint)
    {
        _isMovementActive = false;

        //transform.DORotate(new Vector3(15, 0, 0), 1)
        //    .SetEase(Ease.InOutQuint);
        transform.DOJump(closestSnapPoint, 1, 1, 1.0f)
            .SetEase(Ease.InOutQuint)
            .OnComplete(() =>
            {
                _currentDeformationController = _snapPointManager.GetDeformationPointOfPoint(closestSnapPoint);
                _movementStateMachine.ChangeState(new SwipeState());
            });
    }

    [Button]
    private void SaveVerticalMinLimit()
    {
        m_clampVertical.x = transform.position.z;
    }

    [Button]
    private void SaveVerticalMaxLimit()
    {
        m_clampVertical.y = transform.position.z;
    }

    [Button]
    private void SaveHorizontalMinLimit()
    {
        m_clampHorizontal.x = transform.position.y;
    }

    [Button]
    private void SaveHorizontalMaxLimit()
    {
        m_clampHorizontal.y = transform.position.y;
    }

    public void SetMovementActivity(bool isMovementActive)
    {
        _isMovementActive = isMovementActive;
    }

    public void ExitSlideMode()
    {
        _isMovementActive = false;

        transform.DOMove(_defaultPosition, 1)
            .SetEase(Ease.InOutQuint);
    }

    public void MoveCrowbarTo(Vector3 positionTo)
    {
        transform.DOMove(positionTo, 1)
            .SetEase(Ease.InOutQuint);
    }
}
