using Interfaces;
using Manager;
using UnityEngine;
using Zenject;

namespace In_Game_Usage
{
    public class Notepad : MonoBehaviour, IInteractable
    {
        private InteractionUIManager _interactionUIManager;
        
        [Inject]
        private void Construct(InteractionUIManager interactionUIManager)
        {
            _interactionUIManager = interactionUIManager;
        }
        
        public void ShowInteraction()
        {
            _interactionUIManager.ShowInteractionUI();
        }
        
        public void HideInteraction()
        {
            _interactionUIManager.HideInteractionUI();
        }

        public void Interact()
        {
            throw new System.NotImplementedException();
        }
    }
}
