using System.Collections.Generic;
using UnityEngine;

public class SnapPointManager : MonoBehaviour
{
    [SerializeField] private float m_magnitudeSnapArea = 1.0f;
    public List<Vector3> _snapPointPositions = new List<Vector3>();
    private List<WoodDeformationController> _woodDeformationControllers = new List<WoodDeformationController>();
    public Vector3 GetTheClosestPoint(Vector3 playerPoint)
    {
        foreach (var snapPointPosition in _snapPointPositions)
        {
            if (Vector3.Distance(snapPointPosition, playerPoint) < m_magnitudeSnapArea)
            {
                return snapPointPosition;
            }
        }

        return Vector3.zero;
    }

    public WoodDeformationController GetDeformationPointOfPoint(Vector3 snapPoint)
    {
        int indexValue = -1;

        for (int i = 0; i < _snapPointPositions.Count; i++)
        {
            if (_snapPointPositions[i] == snapPoint)
            {
                indexValue = i;
                break;
            }
        }

        if (indexValue < 0)
        {
            return null;
        }
        else
        {
            return _woodDeformationControllers[indexValue];
        }
    }

    public void AddSnapPoint(Vector3 transformPosition, WoodDeformationController woodDeformationController)
    {
        _snapPointPositions.Add(transformPosition);
        _woodDeformationControllers.Add(woodDeformationController);
    }

    public void RemoveSnapPoint(Vector3 position)
    {
        _snapPointPositions.Remove(position);
    }
}
