using System;
using Interfaces;
using UnityEngine;

namespace CameraMovement
{
    public class ViewRaycast : MonoBehaviour
    {
        [SerializeField] private float m_sphereSize;
        [SerializeField] private float m_rayDistance;

        private IInteractable _interactable;
        
        private void Update()
        {
            Ray ray = new Ray(transform.position,  transform.forward);
            RaycastHit raycastHit;
            Debug.DrawRay(ray.origin, ray.direction);

            if (Physics.Raycast(ray, out raycastHit, m_rayDistance))
            {
                var interactable = raycastHit.collider.GetComponent<IInteractable>();

                if (interactable != null)
                {
                    _interactable = interactable;
                    _interactable.ShowInteraction();
                }
            }
            else
            {
                _interactable?.HideInteraction();
            }
        }
    }
}