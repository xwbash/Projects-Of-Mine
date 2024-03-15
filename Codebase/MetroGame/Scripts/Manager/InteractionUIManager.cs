using DG.Tweening;
using UnityEngine;

namespace Manager
{
    public class InteractionUIManager : MonoBehaviour
    {
        [SerializeField] private CanvasGroup m_canvasGroup;
        
        public void ShowInteractionUI()
        {
            m_canvasGroup.DOFade(1, .5f);
        }

        public void HideInteractionUI()
        {
            m_canvasGroup.DOFade(0, .5f);
        }
    }
}