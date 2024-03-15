using System;
using DG.Tweening;
using UnityEngine;
using UnityEngine.EventSystems;

namespace UI
{
    public class UIButtonController : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler 
    {

        public void OnPointerEnter(PointerEventData eventData)
        {
            Debug.Log("<color=aqua>OnPointerEnter</color>");
            ScaleIn();
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            Debug.Log("<color=red>OnPointerExit</color>");
            ScaleOut();
        }

        private void ScaleIn()
        {
            transform.DOScale(1.2f, .8f)
                .SetEase(Ease.OutCubic);
        }

        private void ScaleOut()
        {
            transform.DOScale(1f, .8f)
                .SetEase(Ease.OutCubic);
        }
    }
}
