
using System;

using System.Net;
using System.Net.Sockets;

using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Threading;

using UnityEngine;

public class NetworkManager{

	public static NetworkManager instance;

	public int port = 25556;
	private TcpClient client;
	private Socket socket;

	public NetworkManager (){
		if (this.connect ()) {
			Debug.Log ("Connexion successful");
		}
	}

	private bool connect(){
		try{
			client = new TcpClient();
			client.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"), port));
				
			} catch (SocketException e) {
				if (e.SocketErrorCode == SocketError.ConnectionRefused) {
					Debug.Log("Connexion impossible. Server is maybe unavailable");
				} else {
					Debug.Log("Socket error: "+ e);
				}


				return false;
			}
		socket = client.Client;
		return true;
	}

	public void send_stereo_image(byte [] message){
		//Send(tcpClient.Client, message, 0, message.Length, 5000);
		socket.Send(message);

	}

	public void receive_depth_map(){
		List<byte> received_depth_map = new List<byte>();

		//Read message DEPTH_MAP from qt

		//Send acquittement
		byte[] message = System.Text.Encoding.ASCII.GetBytes("ready");
		socket.Send (message);

		//Receive depth map
		//socket.Receive(receive_depth_map);
	
	}




	public static NetworkManager Instance(){
		if (instance == null)
			instance = new NetworkManager ();
		return instance;
	}
}

