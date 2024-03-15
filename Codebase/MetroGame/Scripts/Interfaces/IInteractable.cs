using Manager;
using Zenject;

namespace Interfaces
{
    public interface IInteractable
    {
        
        public virtual void ShowInteraction()
        {
        
        }

        public virtual void HideInteraction()
        {
            
        }

        public abstract void Interact();
    }
}
