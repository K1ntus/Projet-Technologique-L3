using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class movement : MonoBehaviour {

    public float speed = 5.0f;
    public float turnSpeed = 25.0f;

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.UpArrow))
        {
            gameObject.transform.Translate(Vector3.forward * speed * Time.deltaTime);
            
        }

        if (Input.GetKey(KeyCode.DownArrow))
            gameObject.transform.Translate(Vector3.forward * -speed * Time.deltaTime);

        if(Input.GetKey(KeyCode.UpArrow) && Input.GetKey(KeyCode.LeftArrow))
            gameObject.transform.Rotate(Vector3.up, -turnSpeed * Time.deltaTime);

        if (Input.GetKey(KeyCode.UpArrow) && Input.GetKey(KeyCode.RightArrow))
            gameObject.transform.Rotate(Vector3.up, turnSpeed * Time.deltaTime);
        
    }
}
