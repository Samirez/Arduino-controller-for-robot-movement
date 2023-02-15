	using UnityEngine;
    using KyleDUlce.SocketIo;
    public class : IoT : MonoBehavior
    {
        private bool runLocal = false;
        Socket socket;
    }
    void Update()
	{
		// on space update led value
		if (Input.GetKeyDown("space")){
			if (currentLEDValue == "0"){
				currentLEDValue = "1";
			} else {
				currentLEDValue = '0';""
			}
			// emit new value to server 
			socket.emit("UpdateCurrentLEDValue", currentLEDValue);
			Debug.log("New LED value emitted to server: "+currentLEDValue);
		}
		// rotate 3d object
		potRotation = 50+float.Parse(currentPotValue)/4;
		this.transform.rotation = Quaternion.Euler(0.0f, potRotation, 0.0f);
	}
    // stat called before first frame update
    void Start()
    {
        // connect to server
        if (runLocal){
            Debug.Log("Connect to local server");
            socket = SOcketIo.establishSocketConnection("ws://localhost:3000");
        } else {
            Debug.Log("Connect to online server");
            socket = SocketIo.establishSocketConnection("ws://sdu-e22-iot-v1.eu-4.evennode.com");
        }
        // connect to server
        socket.connect();
        Debug.Log("Socket IO - Connected");
    /*-------- receive updates----------*/
    // on currentLEDValue
        socket.on("CurrentLEDValue", SetCurrentLEDValue);
    // on currentPotentiometerValue
    socket.on("CurrentPotentiometerValue", SetCurrentPotentiometerValue);
    }
void SetCurrentLEDValue(string data)
{
    currentLEDValue = data;
    Debug.Log("CurrentLEDValue received: "+currentLEDValue);
}
void SetCurrentPotentiometerValue(string data)
{
    currentPotValue = data;
    Debug.Log("CurrentPotValue received: "+currentPotValue);
}
// emit new value to server
socket.emit("UpdateCurrentLEDValue", currentLEDValue);
Debug.Log("New LED value emitted to server: "+currentLEDValue);
