using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cameraTarget : MonoBehaviour {

    public Transform target;
    public Vector3 offset = new Vector3(5.0f, 5.0f, 5.0f);
	
	// Update is called once per frame
	void Update () {
        transform.position = Vector3.Lerp(transform.position, target.position + offset, 5f);
        gameObject.transform.LookAt(target);
	}
}
