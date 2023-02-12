using System;
using UnityEngine;

namespace _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts.Controller
{
    public class CrowbarAnimatorController : MonoBehaviour
    {
        private bool _isSwipingActive = false;
        private Animator _crowbarAnimator;
        private float _animationLerpValue;

        private void Start()
        {
            _crowbarAnimator = GetComponent<Animator>();
        }

        private void Update()
        {
            if (_isSwipingActive)
            {
                _crowbarAnimator.Play("Crowbar2 0", 0, _animationLerpValue);
            }
        }

        public void SetAnimationValue(float value)
        {
            if (value > 0)
            {
                _isSwipingActive = true;
                _animationLerpValue = value;
            }
            else
            {
                _isSwipingActive = false;
                _animationLerpValue = 0;
            }
        }
    }
}