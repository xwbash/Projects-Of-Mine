using System;
using UnityEngine;
using Zenject;

namespace _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts.Manager.PositionAssign
{
    public class AssignPosition : MonoBehaviour
    {
        private GameManager _gameManager;

        [Inject]
        private void Construct(GameManager gameManager)
        {
            _gameManager = gameManager;
        }

        private void Start()
        {
            _gameManager.AddDefaultPosition(transform.position);
        }
    }
}