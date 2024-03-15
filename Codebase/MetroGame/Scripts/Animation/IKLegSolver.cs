using System;
using UnityEngine;

namespace Animation
{
    public class IKLegSolver : MonoBehaviour
    {
        [SerializeField] private Transform m_body;
        [SerializeField] private LayerMask m_terrainLayer;
        [SerializeField] private IKLegSolver m_ikLegSolver;
        [SerializeField] private float m_speed = 5, m_stepDistance = .3f, m_stepLenght = .3f, m_stepHeight = .3f;
        [SerializeField] private Vector3 m_footPosOffset, m_footRotOffset;

        
        private float _footSpacing, _lerp;
        private Vector3 _oldPos, _currentPos, _newPos;
        private Vector3 _oldNorm, _currentNorm, _newNorm;
        private bool _isFirstStep = true;

        private void Start()
        {
            _footSpacing = transform.localPosition.x;
            _currentPos = _newPos = _oldPos = transform.position;
            _currentNorm = _newNorm = _oldNorm = transform.up;
        }

        // Update is called once per frame

        void Update()
        {
            transform.position = _currentPos + m_footPosOffset;
            transform.rotation = Quaternion.LookRotation(_currentNorm) * Quaternion.Euler(m_footRotOffset);

            Ray ray = new Ray(m_body.position + (m_body.right * _footSpacing) + Vector3.up*2, Vector3.down);

            if (Physics.Raycast(ray, out RaycastHit hit, 10, m_terrainLayer.value))
            {
                Debug.DrawRay(ray.origin, ray.direction);
                if (_isFirstStep || (Vector3.Distance(_newPos, hit.point) > m_stepDistance && !m_ikLegSolver.IsMoving() && !IsMoving()))
                {
                    _lerp = 0;
                    int direction = m_body.InverseTransformPoint(hit.point).z > m_body.InverseTransformPoint(_newPos).z ? 1 : -1;
                    _newPos = hit.point + (m_body.forward * (m_stepLenght * direction));
                    _newNorm = hit.normal;
                }
            }

            if (_lerp < 1)
            {
                Vector3 tempPosition = Vector3.Lerp(_oldPos, _newPos, _lerp);
                tempPosition.y += Mathf.Sin(_lerp * Mathf.PI) * m_stepHeight;

                _currentPos = tempPosition;
                _currentNorm = Vector3.Lerp(_oldNorm, _newNorm, _lerp);
                _lerp += Time.deltaTime * m_speed;
            }
            else
            {
                _oldPos = _newPos;
                _oldNorm = _newNorm;
            }
        }

        private void OnDrawGizmos()
        {

            Gizmos.color = Color.red;
            Gizmos.DrawSphere(_newPos, 0.1f);
        }



        public bool IsMoving()
        {
            return _lerp < 1;
        }

    }
}
