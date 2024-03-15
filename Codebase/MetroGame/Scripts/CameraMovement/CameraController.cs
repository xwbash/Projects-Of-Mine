using Cinemachine;
using Manager;
using UnityEngine;
using Zenject;

namespace CameraMovement
{
    public class CameraController : MonoBehaviour
    {
        public float Sensitivity 
        {
            get => m_cameraSensivity;
            set => m_cameraSensivity = value;
        }
        
        
        [Range(0.1f, 9f)][SerializeField] private float m_cameraSensivity = 2f;
        
        [Range(0f, 90f)][SerializeField] private float m_rotationLimit = 88f;

        private Vector2 _rotationValue = Vector2.zero;
        private const string XAxis = "Mouse X"; //Strings in direct code generate garbage, storing and re-using them creates no garbage
        private const string YAxis = "Mouse Y";

        private Transform _playerTransform;


        [Inject]
        private void Construct(PlayerManager playerManager)
        {
            var cinemachineVirtualCamera = GetComponent<CinemachineVirtualCamera>();
            cinemachineVirtualCamera.Follow = playerManager.transform;

            _playerTransform = playerManager.transform;
        }
        
        private void LateUpdate()
        {
            _rotationValue.x += Input.GetAxis(XAxis) * m_cameraSensivity;
            _rotationValue.y += Input.GetAxis(YAxis) * m_cameraSensivity;
            _rotationValue.y = Mathf.Clamp(_rotationValue.y, -m_rotationLimit, m_rotationLimit);
            var xQuat = Quaternion.AngleAxis(_rotationValue.x, Vector3.up);
            var yQuat = Quaternion.AngleAxis(_rotationValue.y, Vector3.left);

            transform.localRotation = xQuat * yQuat;

            if (!ReferenceEquals(_playerTransform, null))
            {
                _playerTransform.localRotation = xQuat;
            }
        }
    }
}
