using _GameAssets.Core.Scripts.Tutorial;
using UnityEngine;

public class TutorialManager : MonoBehaviour
{

    private SlideTutorialHandler _slideTutorialHandler;
    private EightTutorialHandler _eightTutorialHandler;


    private bool _isEightTutorialInitilizedBefore = false;
    private bool _isSlideTutorialInitilizedBefore = false;
    void Start()
    {
        _eightTutorialHandler = GetComponent<EightTutorialHandler>();
        _slideTutorialHandler = GetComponent<SlideTutorialHandler>();
    }

    public void ShowSlideTutorialHandler()
    {
        if (_isSlideTutorialInitilizedBefore)
        {
            return;
        }

        _slideTutorialHandler.Appear();
    }

    public void ShowEightTutorialHandler()
    {
        if (_isEightTutorialInitilizedBefore)
        {
            return;
        }

        _eightTutorialHandler.Appear();
    }
}
