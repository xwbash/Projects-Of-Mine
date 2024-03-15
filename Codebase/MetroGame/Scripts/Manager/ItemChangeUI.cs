using UnityEngine;
using UnityEngine.UI;

namespace Manager
{
    public class ItemChangeUI : MonoBehaviour
    {
        [SerializeField] private Image m_itemImage;
        
        public void ChangeItem(Sprite item)
        {
            m_itemImage.sprite = item;
        }

        public Color GetColor()
        {
            return m_itemImage.color;
        }

        public void SetColor(Color opacityColor)
        {
            m_itemImage.color = opacityColor;
        }
    }
}
