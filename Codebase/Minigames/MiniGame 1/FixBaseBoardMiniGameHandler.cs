using Zenject;
using RubyGames;
using UnityEngine;
using RubyGames.Utilities;
using RubyGames.ProgressBarSystem;
using _GameAssets.Core.Scripts.MiniGameSystem;
using Cysharp.Threading.Tasks;

namespace _GameAssets.Houses.BedRoom_House.MiniGames.FixBaseBoardMiniGame.Scripts
{
    public class FixBaseBoardMiniGameHandler : MiniGameHandlerBase<FixBaseBoardMiniGame>
    {
        [Inject] private ProgressBarService _progressBarService;
        protected override void Init()
        {
            Debug.Log($"{GetType()} Init: {Data.ID}".ToColor(FlatUIColors.Turquoise));
            _progressBarService.Show<MiniGameProgressBar>().Forget();
        }

        protected override void MiniGameStarted()
        {
            Debug.Log($"{GetType()} MiniGameStarted: {Data.ID}".ToColor(FlatUIColors.Turquoise));
        }

        protected override void MiniGameCompleted(bool isFail)
        {
            Debug.Log($"{GetType()} MiniGameCompleted: {Data.ID} - isFail: {isFail}".ToColor(FlatUIColors.Turquoise));

        }
        protected override void OnDisable()
        {
            _progressBarService.Hide<MiniGameProgressBar>().Forget();
        }
    }
}
