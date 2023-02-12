using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Zenject;

public class SnapPoint : MonoBehaviour
{
    [SerializeField] private WoodDeformationController m_woodDeformationController;

    private SnapPointManager _snapPointManager;

    [Inject]
    private void Construct(SnapPointManager snapPointManager)
    {
        this._snapPointManager = snapPointManager;
    }

    private void Start()
    {
        _snapPointManager.AddSnapPoint(transform.position, m_woodDeformationController);
    }


    public void RemovePoint()
    {
        _snapPointManager.RemoveSnapPoint(transform.position);
    }
}
