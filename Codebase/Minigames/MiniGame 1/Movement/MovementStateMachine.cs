using _GameAssets.Core.Scripts.Tutorial;
using Lean.Touch;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Zenject;

public class MovementStateMachine : MovementBase
{
    [SerializeReference] private BaseStates m_currentState;

    private MovementSystem _movementSystem;
    private bool _isFingerDown;
    private TutorialManager _tutorialManager;
    private void OnEnable()
    {
        base.AssignEvent();
    }

    private void OnDisable()
    {
        base.DeAssignEvent();
    }

    [Inject]
    private void Construct(TutorialManager tutorialManager)
    {
        _tutorialManager = tutorialManager;
    }

    private void Start()
    {
        _movementSystem = GetComponent<MovementSystem>();
        ChangeState(new MoveState());
    }

    public void ChangeState(BaseStates states)
    {
        if (!ReferenceEquals(m_currentState, null))
        {
            m_currentState.OnStateExit(_movementSystem, _tutorialManager);
        }

        m_currentState = states;
        m_currentState.OnStateEnter(_movementSystem, _tutorialManager);
    }


    protected override void HandleInput(LeanFinger fingerData)
    {
        m_currentState.OnEventEnter(fingerData);
    }
}
