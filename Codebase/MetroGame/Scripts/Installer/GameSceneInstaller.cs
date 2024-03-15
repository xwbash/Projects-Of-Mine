using CameraMovement;
using EvolveGames;
using Manager;
using UnityEngine;
using Zenject;

namespace Installer
{
    public class GameSceneInstaller : MonoInstaller
    {
        private ItemChangeUI _itemChangeUI;
        private InteractionUIManager _interactionUIManager;
        
        
        public override void InstallBindings()
        {
            Container.Bind<ItemChangeUI>().AsSingle();
            Container.BindInterfacesTo<ItemChange>().AsSingle();
            
            Container.Bind<InteractionUIManager>().AsSingle();
            Container.BindInterfacesTo<ItemChange>().AsSingle();
        }
    }
}