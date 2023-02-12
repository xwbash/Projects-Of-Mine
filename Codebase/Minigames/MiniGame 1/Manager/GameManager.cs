using System.Collections.Generic;
using UnityEngine;
using Zenject;

namespace _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts.Manager
{
    public class GameManager : MonoBehaviour
    {
        private List<Vector3> _defaultPositions = new List<Vector3>();
        private int _currentIndex = 0;
        private SnapPointManager _snapPointManager;
        private MovementSystem _crowbarMovementSystem;

        [Inject]
        private void Construct(SnapPointManager snapPointManager, MovementSystem movementSystem)
        {
            _crowbarMovementSystem = movementSystem;
            _snapPointManager = snapPointManager;
        }

        public void SkipNextStep()
        {
            _currentIndex++;
            _crowbarMovementSystem.MoveCrowbarTo(_defaultPositions[_currentIndex%_defaultPositions.Count]);
        }

        public void AddDefaultPosition(Vector3 position)
        {
            _defaultPositions.Add(position);
        }


    }
}