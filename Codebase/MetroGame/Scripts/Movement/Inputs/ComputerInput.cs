using System;
using UnityEngine;
using UnityEngine.Events;

namespace Movement.Inputs
{
    public class ComputerInput : InputBase
    {
        public override void AssignEvent(UnityAction<Vector2> unityAction)
        {
            InputEvent.AddListener(unityAction);
        }

        public override void DeAssignEvent(UnityAction<Vector2> unityAction)
        {
            InputEvent.RemoveListener(unityAction);
        }


        private void Update()
        {
            var inputPosition = new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"));
            

            InputEvent.Invoke(inputPosition);
        }
    }
}