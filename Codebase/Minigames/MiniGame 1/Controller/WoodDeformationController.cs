using _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts.Manager;
using MegaFiers;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Zenject;

public class WoodDeformationController : MonoBehaviour
{
    [SerializeField] private SnapPoint m_snapPoint;

    private float _lerpValue;
    private Animator _woodAnimator;
    private bool _isSwipingActive;
    private Rigidbody _rigidbody;
    private GameManager _gameManager;

    [Inject]
    private void Consturct(GameManager gameManager)
    {
        _gameManager = gameManager;
    }

    void Start()
    {
        _woodAnimator = GetComponent<Animator>();
        _rigidbody = GetComponent<Rigidbody>();
    }


    private void Update()
    {
        if (_isSwipingActive)
        {
            _woodAnimator.Play("Baseboard2", 0, _lerpValue);
        }
    }

    public void DeformWood(float value)
    {
        _lerpValue = value;
        _isSwipingActive = true;

        if (value >= 1)
        {
            KillWood();
        }
    }

    private void KillWood()
    {
        Debug.Log("Kill wood");
        _woodAnimator.enabled = false;
        _rigidbody.isKinematic = false;
        _rigidbody.AddForce(transform.forward * 80);
        m_snapPoint.RemovePoint();

        _gameManager.SkipNextStep();
    }
}
