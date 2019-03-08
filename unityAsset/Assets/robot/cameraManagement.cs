using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cameraManagement : MonoBehaviour {

    private float cameraPos = -1f;
    private float cameraDist = 2f;
    public Camera leftCamera;
    public Camera rightCamera;

    [SerializeField]
    public float positionCamera
    {
        get
        {
            return cameraPos;
        }

        set
        {
            Vector3 pos = leftCamera.transform.position;
            cameraPos = value;
            leftCamera.transform.Translate(Vector3.right * (cameraPos - pos.x));
        }
    }

    [SerializeField]
    public float distanceBetweenCamera {

        get
        {
            return cameraDist;
        }

        set
        {
            Vector3 pos = leftCamera.transform.position;
            Vector3 posR = rightCamera.transform.position;

            cameraDist = value;
            rightCamera.transform.Translate(Vector3.right * (pos.x + cameraDist - posR.x));
        }
           
    }

}
