using System;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class SoundSliderController : MonoBehaviour
    {
        [SerializeField] private TMP_Text[] m_valueChangeText;
        private Slider _slider;

        private void Start()
        {
            _slider = GetComponent<Slider>();
            OnValueChanged();
        }

        public void OnValueChanged()
        {
            var sliderValue = _slider.value;
            foreach (var valueChangeText in m_valueChangeText)
            {
                valueChangeText.text = ((int)(sliderValue*100)).ToString();
            }
        }

        
    }
}
