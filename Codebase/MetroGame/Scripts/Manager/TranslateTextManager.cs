using TranslateData;
using UnityEngine;

namespace Manager
{
    public class TranslateTextManager : MonoBehaviour
    {
        [SerializeField] private TranslateData.TranslateData m_translateData;

        [SerializeField] private Language_Category _currentLanguage;
        
        public string GetAndTranslateText(string wordToTranslate)
        {
            var translatedText = "";
            
            foreach (var wholeData in m_translateData.LanguageList)
            {
                if (translatedText != "") break;
                
                if (wholeData.ListOfLanguages[0].TranslatedWord == wordToTranslate)
                {
                    var data = wholeData.ListOfLanguages.Find(x => x.LanguageCategory == _currentLanguage);
                    translatedText = data.TranslatedWord;
                    break;
                }
            }

            return translatedText;
        }
        
    }
}
