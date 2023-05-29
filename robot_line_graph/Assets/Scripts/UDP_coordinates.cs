using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using NativeWebSocket;

public class UDP_coordinates : MonoBehaviour
{
    WebSocket socket;
    private string dbName = "URI=file:robDB.db";
    // Start is called before the first frame update
    async void Start()
    {
        socket = new WebSocket("ws://localhost:3001");

        socket.OnOpen += () =>
        {
            Debug.Log("Connection open!");
        };

        socket.OnError += (e) =>
        {
            Debug.Log("Error! " + e);
        };

        socket.OnClose += (e) =>
        {
            Debug.Log("Connection closed!");
        };

        socket.OnMessage += (bytes) =>
        {
            Debug.Log("OnMessage!");
            Debug.Log(bytes);

            // getting the message as a string
            // var message = System.Text.Encoding.UTF8.GetString(bytes);
            // Debug.Log("OnMessage! " + message);
        };

        // Keep sending messages at every 0.3s
        InvokeRepeating("SendWebSocketMessage", 0.0f, 0.3f);

        // waiting for messages
        await socket.Connect();
    }

    // Update is called once per frame
    void Update()
    {
        #if !UNITY_WEBGL || UNITY_EDITOR
            socket.DispatchMessageQueue();
        #endif
    }
    async void SendWebSocketMessage()
    {
        if (socket.State == WebSocketState.Open)
        {
            // Sending bytes
            await socket.Send(new byte[] { 10, 20, 30 });

            // Sending plain text
            await socket.SendText("plain text message");
        }
    }
    private async void OnApplicationQuit()
    {
        await socket.Close();
    }
}
