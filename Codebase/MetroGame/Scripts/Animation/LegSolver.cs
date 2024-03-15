using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LegSolver : MonoBehaviour
{
    [SerializeField] private float m_legDistance;
    [SerializeField] private float m_raycastHeight;
    [SerializeField] private LayerMask m_layerMask;
    [SerializeField] private float m_gizmoRadius;
    void Update()
    {
        Ray ray = new Ray(transform.position + (transform.forward * m_legDistance), -1 * transform.up);
        RaycastHit raycastHit;

        if (Physics.Raycast(ray, out raycastHit, m_raycastHeight, m_layerMask))
        {
            Debug.DrawRay(ray.origin, raycastHit.point, Color.red);

        }
    }

    private void CreateGizmo(Vector3 point)
    {
        Gizmos.DrawCube(point, Vector3.one * m_gizmoRadius);
    }
}
