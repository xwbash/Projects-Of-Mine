using Manager;
using TMPro;
using UnityEngine;
using Zenject;

namespace Controllers
{
    public class TextMeshTranslator : MonoBehaviour
    {
        private TMP_Text _textToTranslate;

        [Inject]
        private void Construct(TranslateTextManager translateTextManager)
        {
            _textToTranslate = GetComponent<TMP_Text>();
            var translateText = translateTextManager.GetAndTranslateText(_textToTranslate.text);
            if (translateText != "")
            {
                _textToTranslate.text = translateText;
            }
        }
    }
}
