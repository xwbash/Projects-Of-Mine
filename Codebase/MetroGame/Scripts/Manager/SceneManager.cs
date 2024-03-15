using UnityEngine;
using UnityEngine.SceneManagement;
using Zenject;

namespace Manager
{
    public class SceneManager : MonoBehaviour
    {
        private ZenjectSceneLoader _zenjectSceneLoader;
        
        [Inject]
        private void Construct(ZenjectSceneLoader zenjectSceneLoader)
        {
            _zenjectSceneLoader = zenjectSceneLoader;
        }

        public void LoadGameScene()
        {
            _zenjectSceneLoader.LoadSceneAsync("GameScene", LoadSceneMode.Additive);
        }
    }
}
