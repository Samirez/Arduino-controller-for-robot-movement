// setup server
var express = require('express');
var app = express();
var server = require('http').createServer(app);
var path = require('path')
app.use(express.static(path.join(__dirname, '')));
// server variables
var currentPotValue = 0;
var currentLEDValue = 0;
// start server
server.listen(3000, function(){
    console.log("Server listening on port: 300\n");
});
// on http get "/" (e.g.: http://localhost:3000/)
app.get('/', function (req, res){
    // send index html to client 
    res.sendFile(__dirname + '/index.html');
});
// find and print the server ip address
var networkInterfaces = require('os').networkInterfaces();
var address;

Object.keys(networkInterfaces).forEach(function(interface){
    networkInterfaces[interface].filter(function(details){
        if (details.family === 'IPv4' && details.internal === false){
            address = details.address;
        }
    })
});

console.log("Server ip address: "+address+"/n");

// setup UDP (user datagram protocol)
var UDP = require("dgram");
var UDPsocket = UDP.createSocket('udp4');
// setup the port to listen for UDP messages
UDPsocket.bind(3001);
// store arduino ip info
var arduinoIPAddress;
var arduinoPort;
// on error
UDPsocket.on('error', function(error){
    console.error('UDP error: '+error.stack+'\n');
});
// on start listening
UDPsocket.on('Listening', function(){
    var listenAtPort = UDPsocket.address().port;
    console.log('Server listening for UDP packets at port: '+listenAtPort+"/n");
});
// on received message
UDPsocket.on('message', (msg, senderInfo)=>{
    console.log("Received UDP message: "+msg);
    console.log("From addr: "+senderInfo.address+", at port: "+senderInfo.port+"/n");
    currentPotValue = msg.toString();
    arduinoIPAddress = senderInfo.address;
    arduinoPort = senderInfo.port;
    EmitPotValue();
    // send acknowledgment message
    // sendUDPMessage(arduinoIPAddress, arduinoPort, "SERVER: the message was received");
});
// send UDP message
function sendUDPMessage(receiverIPAdress, receiverPort, message){
    var UDPMessage = Buffer.from(message);
    UDPsocket.send(UDPMessage, receiverPort, receiverIPAddress, function(error){
        if (error){
            client.close();
        } else {
            console.log("UDP message sent: "+message+"\n");
        }
    });
}
// setup Socket IO
var io = require('socket.io')(server);
// on new client connection
io.on('connection', function(IOsocket){
    console.log("Client has connected"+"/n");
    // on client  disconnection
    IOsocket.on('disconnect', function(){
        console.log("Client has disconnected"+"\n");
    });
    // on "updateCurrentLEDValue"
    IOsocket.on('UpdateCurrentLEDValue', function(data){
        console.log("Current LED value received from client: "+data+"\n");
        currentLEDValue = data;
        io.emit('CurrentLEDValue', currentLEDValue);
        // If arduino, send LED value with UDP
        if (arduinoIPAddress != null && arduinoPort != nulle){
            sendUDPMessage(arduinoIPAddress, arduinoPort, currentLEDValue)
        }
    });
});
// emit "currentPotentiometerValue"
function EmitPotValue(){
    io.emit('CurrentPotentiometerValue', currentPotValue);
}