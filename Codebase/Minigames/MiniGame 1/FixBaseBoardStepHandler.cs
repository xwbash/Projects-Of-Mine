using Zenject;
using RubyGames;
using UnityEngine;
using RubyGames.Utilities;
using _GameAssets.Core.Scripts.StepSystem;

namespace _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts
{
    public class FixBaseBoardStepHandler : StepHandlerBase<FixBaseBoardStep>
    {
        [Inject] private FixBaseBoardMiniGameHandler _miniGameHandler;

        protected override void Init()
        {
           // Debug.Log($"{GetType()} Init: {Data.StepID}".ToColor(FlatUIColors.PeterRiver));
        }

        protected override void StepStarted()
        {
            //  Debug.Log($"{GetType()} StepStarted: {Data.StepID}".ToColor(FlatUIColors.PeterRiver));
        }

        protected override void StepCompleted(bool isFail)
        {
           // Debug.Log($"{GetType()} StepCompleted: {Data.StepID} - isFail: {isFail}".ToColor(FlatUIColors.PeterRiver));
        }


        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.P))
            {
                CompleteStep(false);
            }
        }
    }
}