using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class textureConverter : MonoBehaviour {


    public RenderTexture texture;
    private byte[] texData {
        get
        {
            return texData;
        }
        set
        {
            texData = value;
        }
    }

    public void toTexture2D()
    {
        RenderTexture currentActiveRT = RenderTexture.active;
        RenderTexture.active = texture;
        Texture2D tex = new Texture2D(texture.width, texture.height);
        tex.ReadPixels(new Rect(0, 0, tex.width, tex.height), 0, 0);
        tex.Apply();
        var bytes = tex.EncodeToPNG();
        System.IO.File.WriteAllBytes(Application.dataPath + "/HeadTexture.png", bytes);
        Debug.Log(Application.dataPath);
        UnityEngine.Object.Destroy(tex);
        RenderTexture.active = currentActiveRT;
    }

    private void Update()
    {
        toTexture2D();
    }


}
