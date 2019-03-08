using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class textureConverter : MonoBehaviour {


    public RenderTexture texture;
    private Texture2D tex;
    private TCPTestClient socketTest = new TCPTestClient();
    private int port = 25556;
    private byte[] texData = new byte[1024];

    private void Start()
    {
        tex = new Texture2D(0,0);
        socketTest.ConnectToTcpServer(port);

    }
    public void toTexture2D(RenderTexture RendTex)
    {
        RenderTexture currentActiveRT = RenderTexture.active;
        RenderTexture.active = RendTex;

        tex.Resize(RendTex.width, RendTex.height);
        tex.ReadPixels(new Rect(0, 0, tex.width, tex.height), 0, 0);
        tex.Apply();
        texData = tex.EncodeToJPG();
    //    System.IO.File.WriteAllBytes(Application.dataPath + "/HeadTexture.jpg", texData);
     //   Debug.Log(Application.dataPath);
    //    UnityEngine.Object.Destroy(tex); // release tex object
        RenderTexture.active = currentActiveRT;
    }

    public void sendTexture()
    {
        Debug.Log("test sending");
        socketTest.SendMessage(texData);
    }

    private void Update()
    {
        toTexture2D(texture);
   //     if (Input.GetKeyDown(KeyCode.Space))
            sendTexture();
        
        
    }


}
