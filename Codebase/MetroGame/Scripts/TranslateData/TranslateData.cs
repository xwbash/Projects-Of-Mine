using System;
using System.Collections.Generic;
using Sirenix.OdinInspector;
using UnityEngine;

namespace TranslateData
{
    [CreateAssetMenu(fileName = "Translate Data", menuName = "Scriptable Object/Translate Data", order = 0)]
    public class TranslateData : ScriptableObject
    {
        public List<LanguageList> LanguageList = new List<LanguageList>();
    }


    [Serializable]
    public class LanguageList
    {
        [Title("Title")] [HideLabel] [TableList] public List<Languages> ListOfLanguages = new List<Languages>();
    }
    
    [Serializable]
    public class Languages
    {
        [EnumPaging] public Language_Category LanguageCategory;
        [LabelWidth(22)] public string TranslatedWord;
    }

    public enum Language_Category
    {
        English = 1,
        Italian = 2,
        Russian = 3,
        Japanese = 4,
        Turkish = 5,
        German = 6,
        Spanish = 7,
        France = 8
    }
}