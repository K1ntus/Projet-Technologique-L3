using UnityEngine;
using System.Collections;
using UnityEditor;

[CustomEditor(typeof(cameraManagement))]
public class CameraManagementEditor : Editor {

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        cameraManagement myTarget = (cameraManagement)target;
        myTarget.distanceBetweenCamera = EditorGUILayout.FloatField("Distance between camera", myTarget.distanceBetweenCamera);
        myTarget.positionCamera = EditorGUILayout.FloatField("Camera position", myTarget.positionCamera);


    }

}
